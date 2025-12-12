// multiply_metal.m
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <stdio.h>

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // --------------------------------------------------
        // Python:
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
        // print("Using:", device)
        printf("Using device: %s\n", [[device name] UTF8String]);


        

        id<MTLBuffer> dataBuffer;
        {
            // Python:
            // x = torch.tensor(input_list, dtype=torch.float32, device=device)
            id<MTLCommandQueue> queue = [device newCommandQueue];
            if (!queue) {
                printf("Failed to create command queue\n");
                return 1;
            }
            printf("Command queue created.\n");

            {
                // -------------------------------------------------- Input
                // Python:
                // input_list = [
                //     0, 1, 0, 1, 1, 0, 0, 1,
                //     1, 1, 0, 0, 1, 0, 1, 0
                // ]
                float input_list[16] = {0,1,0,1,1,0,0,1,1,1,0,0,1,0,1,0};
                printf("Input:  ");
                for (int i=0; i<16; i++) printf("%d ", (int)input_list[i]);
                printf("\n");
                // Python:
                // x on GPU; buffers correspond to tensor and factor
                dataBuffer = [device newBufferWithBytes:input_list
                                                            length:sizeof(input_list)
                                                            options:MTLResourceStorageModeShared];
                {
                    id<MTLCommandBuffer> commandBuffer;
                    {
                        // -------------------------------------------------- Kernel
                        // Python:
                        // y = x * factor
                        id<MTLComputePipelineState> pipeline;
                        {
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

                            pipeline = [device newComputePipelineStateWithFunction:function error:&error];
                            if (!pipeline) {
                                NSLog(@"Failed to create pipeline: %@", error);
                                return 1;
                            }
                            printf("Pipeline created.\n");
                        }
                        id<MTLComputeCommandEncoder> encoder;
                        {
                            commandBuffer = [queue commandBuffer];
                            encoder = [commandBuffer computeCommandEncoder];
                            [encoder setComputePipelineState:pipeline];
                            [encoder setBuffer:dataBuffer offset:0 atIndex:0];

                            {
                                // -------------------------------------------------- Factor
                                // Python:
                                // factor = int(sys.argv[1]) if len(sys.argv) > 1 else 3
                                // TODO: we can move this to just before we need it
                                float factor = 3.0f;
                                if (argc > 1) factor = atof(argv[1]);
                                printf("Factor: %.0f\n", factor);
                                id<MTLBuffer> factorBuffer = [device newBufferWithBytes:&factor
                                                                                length:sizeof(float)
                                                                                options:MTLResourceStorageModeShared];
                                [encoder setBuffer:factorBuffer offset:0 atIndex:1];
                            }
                        }

                        {
                            MTLSize gridSize = MTLSizeMake(16, 1, 1);
                            NSUInteger threadGroupSize = pipeline.maxTotalThreadsPerThreadgroup;
                            if (threadGroupSize > 16) threadGroupSize = 16;
                            MTLSize threadsPerGroup = MTLSizeMake(threadGroupSize, 1, 1);

                            [encoder dispatchThreads:gridSize threadsPerThreadgroup:threadsPerGroup];
                        }
                        [encoder endEncoding];
                    }
                    [commandBuffer commit];
                    [commandBuffer waitUntilCompleted];
                }
            }
        }

        // -------------------------------------------------- Result
        // Convert to int before printing
        // (-) Move the tensor from GPU memory to CPU memory
        //     (because most Python operations (like list() or printing) cannot directly access GPU memory.)
        // (-) Change the data type of the tensor from float32 to int32.
        //     (because your original tensor is floating-point because GPU operations often prefer floats, but you want integer outputs like your Python list of 0s and 1s multiplied by the factor.)
        // Python:
        // output_list = y.to("cpu").to(torch.int32).tolist()
        float *output = (float*)[dataBuffer contents];
        // --------------------------------------------------
        
        // print("Output:", output_list)
        printf("\nOutput: ");
        for (int i=0; i<16; i++) printf("%d ", (int)output[i]);
        printf("\n");
        // --------------------------------------------------
    }
    return 0;
}
