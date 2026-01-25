#include <metal_stdlib>
using namespace metal;

kernel void hello(device char *out [[buffer(0)]],
                  uint tid [[thread_position_in_grid]]) {
    if (tid == 0) {
        const char msg[] = "Hello from Metal (C host)!";
        for (uint i = 0; i < sizeof(msg); ++i)
            out[i] = msg[i];
    }
}
