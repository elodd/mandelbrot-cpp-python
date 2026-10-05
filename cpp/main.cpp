// C++ computes the Mandelbrot set (mandelbrot.hpp),
// then sends the data to Python (python/plot.py) to draw it.
#include <pybind11/embed.h>
#include <pybind11/numpy.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "mandelbrot.hpp"

namespace py = pybind11;

constexpr int kWidth = 1200;
constexpr int kHeight = 800;
constexpr int kMaxIter = 500;
constexpr mandelbrot::Area kArea{-2.4, 1.2, -1.2, 1.2};

// Python config that uses the active virtual environment (VIRTUAL_ENV), if any.
// Embedded Python otherwise picks its packages based on where this program lives,
// which on macOS means the system/Homebrew Python instead of .venv.
PyConfig python_config() {
    PyConfig config;
    PyConfig_InitPythonConfig(&config);
    if (const char* venv = std::getenv("VIRTUAL_ENV")) {
#ifdef _WIN32
        const std::string exe = std::string(venv) + "\\Scripts\\python.exe";
#else
        const std::string exe = std::string(venv) + "/bin/python3";
#endif
        PyConfig_SetBytesString(&config, &config.executable, exe.c_str());
    }
    return config;
}

int main(int argc, char* argv[]) {
    PyConfig config = python_config();
    py::scoped_interpreter python{&config};                               // start Python
    py::module_::import("sys").attr("path").attr("insert")(0, "python");  // find python/plot.py

    try {
        // 1. C++: compute into a NumPy array (Python reads this memory directly, no copy)
        py::array_t<float> image({kHeight, kWidth});
        mandelbrot::compute(image.mutable_data(), kWidth, kHeight, kArea, kMaxIter);
        std::cout << "C++: computed " << kWidth << "x" << kHeight << std::endl;

        // 2. Send the data to Python
        const std::string out = argc > 1 ? argv[1] : "";
        py::module_::import("plot").attr("show")(image, kMaxIter, out);
    } catch (const py::error_already_set& e) {
        std::cerr << "Python error: " << e.what() << "\n";
        if (e.matches(PyExc_ImportError))
            std::cerr << "Hint: activate the environment first: source .venv/bin/activate\n";
        return 1;
    }
    return 0;
}
