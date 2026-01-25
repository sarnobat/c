#include <stdio.h>
#include <stdlib.h>
#include <Metal/Metal.h>

static void print_matrix(const float* m, int rows, int cols, const char* name) {
    printf("%s =\n", name);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++)
            printf("%6.2f ", m[i * cols + j]);
        printf("\n");
    }
}

int main(void) {
    @autoreleasepool {
        printf("=== Metal Matrix Multiply ===\n");

        // ------------------------------------------------------------
        // 1. Device setup with fallback (works in iTerm etc.)
        // ------------------------------------------------------------
        NSArray *allDevices = MTLCopyAllDevices();
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if (!device && [allDevices count] > 0)
            device = [allDevices objectAtIndex:0];
        if (!device) { fprintf(stderr, "No Metal device found.\n"); return 1; }

        printf("Using GPU: %s\n", device.name.UTF8String);

        // ------------------------------------------------------------
        // 2. Create command queue and load kernel
        // ------------------------------------------------------------
        id<MTLCommandQueue> queue = [device newCommandQueue];
        NSError *err = nil;
        id<MTLLibrary> lib = [device newLibraryWithFile:@"build/matmul.metallib" error:&err];
        if (!lib) { fprintf(stderr, "Failed to load metallib: %s\n", err.localizedDescription.UTF8String); return 1; }
        id<MTLFunction> fn = [lib newFunctionWithName:@"matmul"];
        if (!fn) { fprintf(stderr, "Kernel not found.\n"); return 1; }

        id<MTLComputePipelineState> pso = [device newComputePipelineStateWithFunction:fn error:&err];
        if (!pso) { fprintf(stderr, "Pipeline error: %s\n", err.localizedDescription.UTF8String); return 1; }

        // ------------------------------------------------------------
        // 3. Define matrices
        // ------------------------------------------------------------
        const uint M = 3, K = 2, N = 3;
        float A[M*K], B[K*N], C[M*N];

        for (int i = 0; i < M*K; i++) A[i] = i + 1;         // A = [1 2; 3 4; 5 6]
        for (int i = 0; i < K*N; i++) B[i] = (i + 1) * 0.5; // B = [0.5 1.0 1.5; 2.0 2.5 3.0]
        for (int i = 0; i < M*N; i++) C[i] = 0;

        print_matrix(A, M, K, "A");
        print_matrix(B, K, N, "B");

        // ------------------------------------------------------------
        // 4. Allocate GPU buffers
        // ------------------------------------------------------------
        id<MTLBuffer> bufA = [device newBufferWithBytes:A length:sizeof(A) options:MTLResourceStorageModeShared];
        id<MTLBuffer> bufB = [device newBufferWithBytes:B length:sizeof(B) options:MTLResourceStorageModeShared];
        id<MTLBuffer> bufC = [device newBufferWithLength:sizeof(C) options:MTLResourceStorageModeShared];
        id<MTLBuffer> bufM = [device newBufferWithBytes:&M length:sizeof(M) options:MTLResourceStorageModeShared];
        id<MTLBuffer> bufN = [device newBufferWithBytes:&N length:sizeof(N) options:MTLResourceStorageModeShared];
        id<MTLBuffer> bufK = [device newBufferWithBytes:&K length:sizeof(K) options:MTLResourceStorageModeShared];

        // ------------------------------------------------------------
        // 5. Encode and dispatch
        // ------------------------------------------------------------
        id<MTLCommandBuffer> cb = [queue commandBuffer];
        id<MTLComputeCommandEncoder> enc = [cb computeCommandEncoder];
        [enc setComputePipelineState:pso];
        [enc setBuffer:bufA offset:0 atIndex:0];
        [enc setBuffer:bufB offset:0 atIndex:1];
        [enc setBuffer:bufC offset:0 atIndex:2];
        [enc setBuffer:bufM offset:0 atIndex:3];
        [enc setBuffer:bufN offset:0 atIndex:4];
        [enc setBuffer:bufK offset:0 atIndex:5];

        // One thread per element of C
        MTLSize grid = MTLSizeMake(N, M, 1);
        MTLSize tg   = MTLSizeMake(1, 1, 1);
        [enc dispatchThreads:grid threadsPerThreadgroup:tg];
        [enc endEncoding];
        [cb commit];
        [cb waitUntilCompleted];

        // ------------------------------------------------------------
        // 6. Read back and print
        // ------------------------------------------------------------
        memcpy(C, bufC.contents, sizeof(C));
        print_matrix(C, M, N, "C = A×B");

        printf("=== Done ===\n");
    }
    return 0;
}
