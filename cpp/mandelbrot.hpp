#pragma once
// Mandelbrot calculation, used by the C++ program (main.cpp) and the Python module (module.cpp).
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <thread>
#include <vector>

namespace mandelbrot {

// Area of the complex plane to draw
struct Area {
    double x_min, x_max, y_min, y_max;
};

// Smooth escape count for point c = cr + ci*i. Returns max_iter for points inside the set.
inline float escape(double cr, double ci, int max_iter) {
    double zr = 0, zi = 0;
    int n = 0;
    while (zr * zr + zi * zi <= 256.0 && n < max_iter) {
        const double t = zr * zr - zi * zi + cr;
        zi = 2 * zr * zi + ci;
        zr = t;
        ++n;
    }
    if (n == max_iter) return static_cast<float>(max_iter);
    return static_cast<float>(n + 1 - std::log2(std::log2(zr * zr + zi * zi) / 2.0));
}

// Fills out[height * width] row by row; row 0 is the top of the area.
// threads <= 0 uses all CPU cores. Throws std::invalid_argument on bad input.
inline void compute(float* out, int width, int height, const Area& a, int max_iter, int threads = 0) {
    if (out == nullptr) throw std::invalid_argument("out must not be null");
    if (width <= 0 || height <= 0) throw std::invalid_argument("width and height must be > 0");
    if (max_iter <= 0) throw std::invalid_argument("max_iter must be > 0");
    if (!(a.x_max > a.x_min) || !(a.y_max > a.y_min))
        throw std::invalid_argument("area max must be greater than min");

    if (threads <= 0) threads = static_cast<int>(std::max(1u, std::thread::hardware_concurrency()));
    threads = std::min(threads, height);

    const double dx = (a.x_max - a.x_min) / width;
    const double dy = (a.y_max - a.y_min) / height;

    auto worker = [&](int t) {
        for (int y = t; y < height; y += threads) {   // interleaved rows balance the load
            const double ci = a.y_max - (y + 0.5) * dy;
            for (int x = 0; x < width; ++x)
                out[y * width + x] = escape(a.x_min + (x + 0.5) * dx, ci, max_iter);
        }
    };

    std::vector<std::thread> pool;
    pool.reserve(threads);
    for (int t = 0; t < threads; ++t) pool.emplace_back(worker, t);
    for (auto& th : pool) th.join();
}

// Same, returning a new buffer.
inline std::vector<float> compute(int width, int height, const Area& area, int max_iter, int threads = 0) {
    if (width <= 0 || height <= 0) throw std::invalid_argument("width and height must be > 0");
    std::vector<float> out(static_cast<std::size_t>(width) * height);
    compute(out.data(), width, height, area, max_iter, threads);
    return out;
}

}  // namespace mandelbrot
