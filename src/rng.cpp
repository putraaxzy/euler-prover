#include "rng.h"
#include <mutex>

SecureRNG::SecureRNG(uint64_t seed) {
    auto splitmix = [](uint64_t& x) {
        x += 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    };
    
    for (auto& s : state) s = splitmix(seed);
}

uint64_t SecureRNG::next() {
    std::lock_guard<std::mutex> lock(mtx);
    const uint64_t result = rotl(state[1] * 5, 7) * 9;
    const uint64_t t = state[1] << 17;
    
    state[2] ^= state[0];
    state[3] ^= state[1];
    state[1] ^= state[2];
    state[0] ^= state[3];
    
    state[2] ^= t;
    state[3] = rotl(state[3], 45);
    
    return result;
}
