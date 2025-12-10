#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <glob.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

#define WIDTH 640
#define HEIGHT 480
#define FPS 30
#define DURATION 2 // seconds per image

void check(int ret, const char *msg) {
    if (ret < 0) {
        char buf[1024];
        av_strerror(ret, buf, sizeof(buf));
        fprintf(stderr, "%s: %s\n", msg, buf);
        exit(1);
    }
}

int main() {
    glob_t globbuf;
    int ret = glob("*.png", 0, NULL, &globbuf);
    if (ret != 0 || globbuf.gl_pathc == 0) {
        fprintf(stderr, "No PNG files found in current directory.\n");
        return 1;
    }

    int num_images = globbuf.gl_pathc;
    char **images = globbuf.gl_pathv;

    AVFormatContext *fmt_ctx;
    AVStream *video_st;
    AVCodecContext *codec_ctx;
    const AVCodec *codec;
    AVFrame *frame;
    AVPacket pkt;

    // Allocate output format context (MKV)
    ret = avformat_alloc_output_context2(&fmt_ctx, NULL, "matroska", "slideshow.mkv");
    check(ret, "avformat_alloc_output_context2");

    codec = avcodec_find_encoder(AV_CODEC_ID_RAWVIDEO);
    if (!codec) { fprintf(stderr,"Rawvideo codec not found\n"); return 1; }

    video_st = avformat_new_stream(fmt_ctx, codec);
    if (!video_st) { fprintf(stderr,"Could not create stream\n"); return 1; }

    codec_ctx = avcodec_alloc_context3(codec);
    codec_ctx->codec_id = AV_CODEC_ID_RAWVIDEO;
    codec_ctx->width = WIDTH;
    codec_ctx->height = HEIGHT;
    codec_ctx->time_base = (AVRational){1,FPS};
    codec_ctx->framerate = (AVRational){FPS,1};
    codec_ctx->pix_fmt = AV_PIX_FMT_RGB24;
    codec_ctx->gop_size = 12;

    if (fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    ret = avcodec_open2(codec_ctx, codec, NULL);
    check(ret, "avcodec_open2");

    ret = avcodec_parameters_from_context(video_st->codecpar, codec_ctx);
    check(ret, "avcodec_parameters_from_context");
    video_st->time_base = codec_ctx->time_base;

    if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&fmt_ctx->pb, "slideshow.mkv", AVIO_FLAG_WRITE);
        check(ret, "avio_open");
    }

    ret = avformat_write_header(fmt_ctx, NULL);
    check(ret, "avformat_write_header");

    frame = av_frame_alloc();
    frame->format = codec_ctx->pix_fmt;
    frame->width = codec_ctx->width;
    frame->height = codec_ctx->height;
    ret = av_frame_get_buffer(frame, 32);
    check(ret, "av_frame_get_buffer");

    struct SwsContext *sws_ctx = sws_getContext(
        WIDTH, HEIGHT, AV_PIX_FMT_RGB24,   // input
        WIDTH, HEIGHT, AV_PIX_FMT_RGB24,   // output
        SWS_BILINEAR, NULL, NULL, NULL
    );

    int64_t pts = 0;

    for (int i = 0; i < num_images; i++) {
        int w,h,channels;
        unsigned char *pixels = stbi_load(images[i], &w,&h,&channels,3);
        if (!pixels) { fprintf(stderr,"Failed to load %s\n", images[i]); return 1; }

        uint8_t *rgb_buffer = malloc(WIDTH*HEIGHT*3);
        memset(rgb_buffer, 0, WIDTH*HEIGHT*3);

        int copy_w = w<WIDTH?w:WIDTH;
        int copy_h = h<HEIGHT?h:HEIGHT;
        int x_off = (WIDTH - copy_w)/2;
        int y_off = (HEIGHT - copy_h)/2;

        for(int y=0;y<copy_h;y++) {
            memcpy(rgb_buffer + ((y+y_off)*WIDTH + x_off)*3,
                   pixels + y*w*3,
                   copy_w*3);
        }

        stbi_image_free(pixels);

        // Repeat frames for DURATION seconds
        int frames_per_image = FPS * DURATION;
        for (int f = 0; f < frames_per_image; f++) {
            uint8_t *in_data[1] = { rgb_buffer };
            int in_linesize[1] = { 3*WIDTH };
            sws_scale(sws_ctx, in_data, in_linesize, 0, HEIGHT, frame->data, frame->linesize);

            frame->pts = pts++;  // increment per frame

            av_init_packet(&pkt);
            pkt.data = NULL;
            pkt.size = 0;

            ret = avcodec_send_frame(codec_ctx, frame);
            check(ret, "avcodec_send_frame");

            while (ret >= 0) {
                ret = avcodec_receive_packet(codec_ctx, &pkt);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    break;
                check(ret, "avcodec_receive_packet");

                pkt.stream_index = video_st->index;
                ret = av_interleaved_write_frame(fmt_ctx, &pkt);
                av_packet_unref(&pkt);
                check(ret, "av_interleaved_write_frame");
            }
        }

        free(rgb_buffer);
    }

    // Flush encoder
    avcodec_send_frame(codec_ctx, NULL);
    while(avcodec_receive_packet(codec_ctx, &pkt) == 0) {
        pkt.stream_index = video_st->index;
        av_interleaved_write_frame(fmt_ctx, &pkt);
        av_packet_unref(&pkt);
    }

    av_write_trailer(fmt_ctx);
    av_frame_free(&frame);
    avcodec_free_context(&codec_ctx);
    if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&fmt_ctx->pb);
    avformat_free_context(fmt_ctx);
    sws_freeContext(sws_ctx);

    printf("Slideshow saved to slideshow.mkv\n");
    return 0;
}
