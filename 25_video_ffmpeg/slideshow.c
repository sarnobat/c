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
#define DURATION 3 // seconds per image

// Helper: log error and exit
void check(int ret, const char *msg) {
    if (ret < 0) {
        char buf[1024];
        av_strerror(ret, buf, sizeof(buf));
        fprintf(stderr, "%s: %s\n", msg, buf);
        exit(1);
    }
}

int main() {
    // Find all PNG files in current directory
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
    AVCodec *codec;
    AVFrame *frame;
    AVPacket pkt;

    // Allocate format context
    ret = avformat_alloc_output_context2(&fmt_ctx, NULL, NULL, "slideshow.mp4");
    check(ret, "avformat_alloc_output_context2");

    // Find encoder
    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) { fprintf(stderr,"H.264 codec not found\n"); return 1; }

    // Create video stream
    video_st = avformat_new_stream(fmt_ctx, codec);
    if (!video_st) { fprintf(stderr,"Could not create stream\n"); return 1; }

    codec_ctx = avcodec_alloc_context3(codec);
    codec_ctx->codec_id = AV_CODEC_ID_H264;
    codec_ctx->width = WIDTH;
    codec_ctx->height = HEIGHT;
    codec_ctx->time_base = (AVRational){1,FPS};
    codec_ctx->framerate = (AVRational){FPS,1};
    codec_ctx->gop_size = 12;
    codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    codec_ctx->max_b_frames = 2;

    if (fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    // Open codec
    ret = avcodec_open2(codec_ctx, codec, NULL);
    check(ret, "avcodec_open2");

    // Copy codec parameters to stream (fixes "codec none" error)
    ret = avcodec_parameters_from_context(video_st->codecpar, codec_ctx);
    check(ret, "avcodec_parameters_from_context");

    video_st->time_base = codec_ctx->time_base;

    // Open output file
    if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&fmt_ctx->pb, "slideshow.mp4", AVIO_FLAG_WRITE);
        check(ret, "avio_open");
    }

    // Write header
    ret = avformat_write_header(fmt_ctx, NULL);
    check(ret, "avformat_write_header");

    // Allocate frame
    frame = av_frame_alloc();
    frame->format = codec_ctx->pix_fmt;
    frame->width = codec_ctx->width;
    frame->height = codec_ctx->height;
    ret = av_frame_get_buffer(frame, 32);
    check(ret, "av_frame_get_buffer");

    // Scaling context (RGB -> YUV420P)
    struct SwsContext *sws_ctx = sws_getContext(
        WIDTH, HEIGHT, AV_PIX_FMT_RGB24,
        WIDTH, HEIGHT, AV_PIX_FMT_YUV420P,
        SWS_BILINEAR, NULL, NULL, NULL
    );

    int pts = 0;

    for (int i = 0; i < num_images; i++) {
        int w,h,channels;
        unsigned char *pixels = stbi_load(images[i], &w,&h,&channels,3);
        if (!pixels) { fprintf(stderr,"Failed to load %s\n", images[i]); return 1; }

        // Center-crop or letterbox to output size
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

        // Write each frame multiple times for duration
        for (int f=0; f<FPS*DURATION; f++) {
            uint8_t *in_data[1] = { rgb_buffer };
            int in_linesize[1] = { 3*WIDTH };
            sws_scale(sws_ctx, in_data, in_linesize, 0, HEIGHT, frame->data, frame->linesize);

            frame->pts = pts++;

            av_init_packet(&pkt);
            pkt.data = NULL;
            pkt.size = 0;

            ret = avcodec_send_frame(codec_ctx, frame);
            check(ret, "avcodec_send_frame");

            ret = avcodec_receive_packet(codec_ctx, &pkt);
            if (ret == 0) {
                pkt.stream_index = video_st->index;
                ret = av_interleaved_write_frame(fmt_ctx, &pkt);
                av_packet_unref(&pkt);
                check(ret, "av_interleaved_write_frame");
            } else if (ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
                check(ret, "avcodec_receive_packet");
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

    // Write trailer and clean up
    av_write_trailer(fmt_ctx);
    av_frame_free(&frame);
    avcodec_free_context(&codec_ctx);
    if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&fmt_ctx->pb);
    avformat_free_context(fmt_ctx);
    sws_freeContext(sws_ctx);

    printf("Slideshow saved to slideshow.mp4\n");
    return 0;
}
