#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Metal/Metal.h>

int main(void) {
    // ------------------------------------------------------------
    // 1. Create a Metal device (like choosing a CUDA GPU)
    // ------------------------------------------------------------
    @autoreleasepool {
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if (!device) {
            fprintf(stderr, "No Metal device found\n");
            return 1;
        }

        // ------------------------------------------------------------
        // 2. Load compiled Metal library (similar to loading a PTX/CUBIN)
        // ------------------------------------------------------------
        NSError *err = nil;
        id<MTLLibrary> lib = [device newLibraryWithFile:@"build/hello.metallib" error:&err];
        if (!lib) {
            fprintf(stderr, "Library load failed: %s\n", err.localizedDescription.UTF8String);
            return 1;
        }

        // ------------------------------------------------------------
        // 3. Get the kernel function entry point ("hello" kernel)
        // ------------------------------------------------------------
        id<MTLFunction> fn = [lib newFunctionWithName:@"hello"];
        if (!fn) {
            fprintf(stderr, "Function 'hello' not found.\n");
            return 1;
        }

        // ------------------------------------------------------------
        // 4. Create a pipeline (like building a CUDA kernel launch configuration)
        // ------------------------------------------------------------
        id<MTLComputePipelineState> pso = [device newComputePipelineStateWithFunction:fn error:&err];
        if (!pso) {
            fprintf(stderr, "Pipeline creation failed: %s\n", err.localizedDescription.UTF8String);
            return 1;
        }

        // ------------------------------------------------------------
        // 5. Allocate a shared buffer (like cudaMallocManaged)
        // ------------------------------------------------------------
        const size_t bufSize = 256;
        id<MTLBuffer> buf = [device newBufferWithLength:bufSize options:MTLResourceStorageModeShared];
        if (!buf) {
            fprintf(stderr, "Failed to create buffer\n");
            return 1;
        }

        // ------------------------------------------------------------
        // 6. Create a command queue and command buffer
        //    (like CUDA streams and launch queues)
        // ------------------------------------------------------------
        id<MTLCommandQueue> q = [device newCommandQueue];
        id<MTLCommandBuffer> cb = [q commandBuffer];

        // ------------------------------------------------------------
        // 7. Create a compute command encoder (like a kernel launch context)
        // ------------------------------------------------------------
        id<MTLComputeCommandEncoder> enc = [cb computeCommandEncoder];

        // Bind the pipeline and the buffer to argument index 0
        [enc setComputePipelineState:pso];
        [enc setBuffer:buf offset:0 atIndex:0];

        // ------------------------------------------------------------
        // 8. Launch 1 thread (like <<<1,1>>>)
        // ------------------------------------------------------------
        MTLSize threadsPerGrid = MTLSizeMake(1, 1, 1);
        MTLSize threadsPerTG   = MTLSizeMake(1, 1, 1);
        [enc dispatchThreads:threadsPerGrid threadsPerThreadgroup:threadsPerTG];

        // Finish encoding GPU commands
        [enc endEncoding];

        // ------------------------------------------------------------
        // 9. Submit and wait for completion (synchronous)
        // ------------------------------------------------------------
        [cb commit];
        [cb waitUntilCompleted];

        // ------------------------------------------------------------
        // 10. Read the result from the shared buffer
        // ------------------------------------------------------------
        printf("%s\n", (char*)buf.contents);
    }

    return 0;
}
