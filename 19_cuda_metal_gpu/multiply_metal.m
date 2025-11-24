// multiply_metal.m
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <stdio.h>

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // === Python equivalent ===
        // device = torch.device("mps" if torch.backends.mps.is_available() else "cpu")
        NSArray<id<MTLDevice>> *devices = MTLCopyAllDevices();
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if (!device) {
            NSLog(@"MTLCreateSystemDefaultDevice() returned nil; falling back to first enumerated device.");
            device = devices[0];
        }
        if (!device) {
            printf("No Metal device found!\n");
            return 1;
        }
        printf("Using device: %s\n", [[device name] UTF8String]);

        id<MTLCommandQueue> queue = [device newCommandQueue];
        if (!queue) {
            printf("Failed to create command queue\n");
            return 1;
        }
        printf("Command queue created.\n");

        // === Python input list ===
        float input_list[16] = {0,1,0,1,1,0,0,1,1,1,0,0,1,0,1,0};

        // === factor from arg or default 3 ===
        float factor = 3.0f;
        if (argc > 1) factor = atof(argv[1]);
        printf("Factor: %.0f\n", factor);

        // === Inline Metal kernel (string literal) ===
        NSString *kernelSrc = @
        "using namespace metal;\n"
        "kernel void multiply(device float* data [[ buffer(0) ]],\n"
        "                     device float* factor [[ buffer(1) ]],\n"
        "                     uint id [[ thread_position_in_grid ]]) {\n"
        "    if (id < 16) data[id] *= factor[0];\n"
        "}";

        NSError *error = nil;
        id<MTLLibrary> library = [device newLibraryWithSource:kernelSrc options:nil error:&error];
        if (!library) {
            NSLog(@"Failed to compile kernel: %@", error);
            return 1;
        }

        id<MTLFunction> function = [library newFunctionWithName:@"multiply"];
        if (!function) {
            printf("Failed to get function\n");
            return 1;
        }

        id<MTLComputePipelineState> pipeline = [device newComputePipelineStateWithFunction:function error:&error];
        if (!pipeline) {
            NSLog(@"Failed to create pipeline: %@", error);
            return 1;
        }
        printf("Pipeline created.\n");

        // === Buffers ===
        id<MTLBuffer> dataBuffer = [device newBufferWithBytes:input_list
                                                       length:sizeof(input_list)
                                                      options:MTLResourceStorageModeShared];
        id<MTLBuffer> factorBuffer = [device newBufferWithBytes:&factor
                                                         length:sizeof(float)
                                                        options:MTLResourceStorageModeShared];

        // === Encode and run ===
        id<MTLCommandBuffer> commandBuffer = [queue commandBuffer];
        id<MTLComputeCommandEncoder> encoder = [commandBuffer computeCommandEncoder];
        [encoder setComputePipelineState:pipeline];
        [encoder setBuffer:dataBuffer offset:0 atIndex:0];
        [encoder setBuffer:factorBuffer offset:0 atIndex:1];

        MTLSize gridSize = MTLSizeMake(16, 1, 1);
        NSUInteger threadGroupSize = pipeline.maxTotalThreadsPerThreadgroup;
        if (threadGroupSize > 16) threadGroupSize = 16;
        MTLSize threadsPerGroup = MTLSizeMake(threadGroupSize, 1, 1);

        [encoder dispatchThreads:gridSize threadsPerThreadgroup:threadsPerGroup];
        [encoder endEncoding];
        [commandBuffer commit];
        [commandBuffer waitUntilCompleted];

        // === Print results ===
        float *output = (float*)[dataBuffer contents];
        printf("Input:  ");
        for (int i=0; i<16; i++) printf("%d ", (int)input_list[i]);
        printf("\nOutput: ");
        for (int i=0; i<16; i++) printf("%d ", (int)output[i]);
        printf("\n");
    }
    return 0;
}
