// Python module "mandelbrot": exposes the C++ calculation (mandelbrot.hpp) to Python.
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include <stdexcept>

#include "mandelbrot.hpp"

namespace py = pybind11;

// Returns a float32 NumPy array (height, width). C++ writes directly
// into the array's memory, so Python gets the result without a copy.
py::array_t<float> compute(int width, int height, double x_min, double x_max,
                           double y_min, double y_max, int max_iter, int threads) {
    if (width <= 0 || height <= 0) throw std::invalid_argument("width and height must be > 0");
    py::array_t<float> image({height, width});
    float* out = image.mutable_data();
    {
        py::gil_scoped_release release;   // other Python threads keep running
        mandelbrot::compute(out, width, height, {x_min, x_max, y_min, y_max}, max_iter, threads);
    }
    return image;
}

PYBIND11_MODULE(mandelbrot, m) {
    m.doc() = "Mandelbrot set computed in C++";
    m.def("compute", &compute,
          py::arg("width"), py::arg("height"),
          py::arg("x_min"), py::arg("x_max"), py::arg("y_min"), py::arg("y_max"),
          py::arg("max_iter"), py::arg("threads") = 0,
          "Escape values as a float32 array (height, width); max_iter = inside the set.");
    m.def("escape", &mandelbrot::escape, py::arg("cr"), py::arg("ci"), py::arg("max_iter"),
          "Smooth escape value of a single point.");
}
