#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Metal/Metal.h>

int main(void) {
    @autoreleasepool {
        printf("=== Metal Hello Debug ===\n");

        // ------------------------------------------------------------
        // 1. Enumerate all Metal devices visible to this process
        // ------------------------------------------------------------
        NSArray *allDevices = MTLCopyAllDevices();
        NSUInteger count = [allDevices count];
        printf("MTLCopyAllDevices() -> %lu device(s)\n", (unsigned long)count);
        for (NSUInteger i = 0; i < count; i++) {
            id<MTLDevice> d = [allDevices objectAtIndex:i];
            printf("  [%lu] %s\n", (unsigned long)i, d.name.UTF8String);
            printf("       lowPower: %s  removable: %s  headless: %s\n",
                   d.isLowPower ? "yes" : "no",
                   d.isRemovable ? "yes" : "no",
                   d.isHeadless ? "yes" : "no");
            printf("       registryID: 0x%llx\n", d.registryID);
        }

        // ------------------------------------------------------------
        // 2. Try to get the system default device first
        // ------------------------------------------------------------
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if (!device && count > 0) {
            printf("⚙️  MTLCreateSystemDefaultDevice() returned nil; "
                   "falling back to first enumerated device.\n");
            device = [allDevices objectAtIndex:0];
        }

        if (!device) {
            fprintf(stderr, "❌  No usable Metal device available.\n");
            return 1;
        }

        printf("✅ Using device: %s\n", device.name.UTF8String);

        // ------------------------------------------------------------
        // 3. Create a command queue (like a CUDA stream)
        // ------------------------------------------------------------
        id<MTLCommandQueue> queue = [device newCommandQueue];
        if (!queue) {
            fprintf(stderr, "❌  Failed to create command queue.\n");
            return 1;
        }
        printf("✅ Command queue created.\n");

        // ------------------------------------------------------------
        // 4. Load compiled Metal library (.metallib)
        // ------------------------------------------------------------
        NSError *err = nil;
        id<MTLLibrary> lib = [device newLibraryWithFile:@"build/hello.metallib" error:&err];
        if (!lib) {
            fprintf(stderr, "❌  Failed to load build/hello.metallib: %s\n",
                    err ? err.localizedDescription.UTF8String : "(unknown error)");
            return 1;
        }
        printf("✅ Library loaded successfully.\n");

        // ------------------------------------------------------------
        // 5. Load kernel function "hello"
        // ------------------------------------------------------------
        id<MTLFunction> fn = [lib newFunctionWithName:@"hello"];
        if (!fn) {
            fprintf(stderr, "❌  Kernel function 'hello' not found.\n");
            return 1;
        }
        printf("✅ Kernel function 'hello' found.\n");

        // ------------------------------------------------------------
        // 6. Create compute pipeline
        // ------------------------------------------------------------
        id<MTLComputePipelineState> pso = [device newComputePipelineStateWithFunction:fn error:&err];
        if (!pso) {
            fprintf(stderr, "❌  Failed to create compute pipeline: %s\n",
                    err ? err.localizedDescription.UTF8String : "(unknown)");
            return 1;
        }
        printf("✅ Compute pipeline ready.\n");

        // ------------------------------------------------------------
        // 7. Allocate output buffer (shared memory)
        // ------------------------------------------------------------
        const size_t bufSize = 256;
        id<MTLBuffer> buf = [device newBufferWithLength:bufSize options:MTLResourceStorageModeShared];
        if (!buf) {
            fprintf(stderr, "❌  Failed to create buffer.\n");
            return 1;
        }
        printf("✅ Shared buffer created (%zu bytes).\n", bufSize);

        // ------------------------------------------------------------
        // 8. Create and encode command buffer
        // ------------------------------------------------------------
        id<MTLCommandBuffer> cb = [queue commandBuffer];
        if (!cb) {
            fprintf(stderr, "❌  Failed to create command buffer.\n");
            return 1;
        }

        id<MTLComputeCommandEncoder> enc = [cb computeCommandEncoder];
        if (!enc) {
            fprintf(stderr, "❌  Failed to create compute encoder.\n");
            return 1;
        }

        [enc setComputePipelineState:pso];
        [enc setBuffer:buf offset:0 atIndex:0];

        MTLSize threadsPerGrid = MTLSizeMake(1, 1, 1);
        MTLSize threadsPerTG   = MTLSizeMake(1, 1, 1);
        [enc dispatchThreads:threadsPerGrid threadsPerThreadgroup:threadsPerTG];
        [enc endEncoding];

        printf("🚀 Dispatching GPU work...\n");
        [cb commit];
        [cb waitUntilCompleted];
        printf("✅ GPU work completed.\n");

        // ------------------------------------------------------------
        // 9. Read the result
        // ------------------------------------------------------------
        const char *result = (const char *)buf.contents;
        if (result && result[0])
            printf("Result buffer: \"%s\"\n", result);
        else
            printf("⚠️  Result buffer empty or not written.\n");

        printf("=== Metal Hello Complete ===\n");
    }

    return 0;
}
