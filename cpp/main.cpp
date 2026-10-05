// C++ computes the Mandelbrot set (mandelbrot.hpp),
// then sends the data to Python (python/plot.py) to draw it.
#include <pybind11/embed.h>
#include <pybind11/numpy.h>

#include <iostream>
#include <string>

#include "mandelbrot.hpp"

namespace py = pybind11;

constexpr int kWidth = 1200;
constexpr int kHeight = 800;
constexpr int kMaxIter = 500;
constexpr mandelbrot::Area kArea{-2.4, 1.2, -1.2, 1.2};

int main(int argc, char* argv[]) {
    py::scoped_interpreter python;                                        // start Python
    py::module_::import("sys").attr("path").attr("insert")(0, "python");  // find python/plot.py

    try {
        // 1. C++: compute into a NumPy array (Python reads this memory directly, no copy)
        py::array_t<float> image({kHeight, kWidth});
        mandelbrot::compute(image.mutable_data(), kWidth, kHeight, kArea, kMaxIter);
        std::cout << "C++: computed " << kWidth << "x" << kHeight << "\n";

        // 2. Send the data to Python
        const std::string out = argc > 1 ? argv[1] : "";
        py::module_::import("plot").attr("show")(image, kMaxIter, out);
    } catch (const py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
