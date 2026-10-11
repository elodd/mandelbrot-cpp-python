# mandelbrot-cpp-python

[![tests](https://github.com/elodd/mandelbrot-cpp-python/actions/workflows/tests.yml/badge.svg)](https://github.com/elodd/mandelbrot-cpp-python/actions/workflows/tests.yml)

The Mandelbrot set in two steps: **C++ calculates, Python draws.**

1. **Calculate (C++):** `cpp/mandelbrot.hpp` computes the image on all CPU cores.
2. **Draw (Python):** `python/plot.py` shades it and draws it with matplotlib.

The image is a NumPy `float32` array that C++ writes into directly, so it reaches Python without a copy. [pybind11](https://github.com/pybind/pybind11) connects the two.

There are two ways to run the pipeline. Both do the same thing; they differ only in which language starts it:

| Started from | Entry point | How it works |
|---|---|---|
| **C++** | `build/mandelbrot` | A C++ program calculates the image, then embeds Python to call `python/plot.py`. |
| **Python** | `python/draw.py` | A Python script calls the C++ calculation through the module `mandelbrot`, then calls `python/plot.py`. |

## Layout

```
cpp/mandelbrot.hpp     calculation (header-only, multithreaded)
cpp/main.cpp           C++ program: embeds Python, computes, calls plot.py
cpp/module.cpp         Python module "mandelbrot" wrapping the calculation
python/plot.py         shading and drawing, used by both directions
python/draw.py         Python program: calls the C++ module, then plot.py
tests/cpp/             GoogleTest unit tests + end-to-end CTest
tests/python/          pytest tests
```

## Requirements

- C++17 compiler, CMake ≥ 3.18
- Python 3 with development headers
- Python packages: `pybind11`, `numpy`, `matplotlib` (and `pytest` for tests)

macOS:

```sh
xcode-select --install
brew install cmake python
```

Ubuntu/Debian:

```sh
sudo apt install build-essential cmake python3-dev python3-venv
```

## Build

```sh
git clone https://github.com/elodd/mandelbrot-cpp-python.git
cd mandelbrot-cpp-python

python3 -m venv .venv
source .venv/bin/activate
pip install pybind11 numpy matplotlib pytest

cmake -B build
cmake --build build
```

Outputs:

- `build/mandelbrot`: the C++ program
- `build/python/mandelbrot.*.so`: the Python module
- `build/tests/cpp/test_mandelbrot`: the C++ tests (the first configure downloads GoogleTest)

Pass `-DBUILD_TESTS=OFF` to skip the C++ tests and the GoogleTest download.

## Run

Run from the project root (not `build/`) with `.venv` active.

### C++ → Python

```sh
./build/mandelbrot            # open a window
./build/mandelbrot out.png    # save to out.png
```

Image size, iteration count and area are constants at the top of `cpp/main.cpp`; rebuild after changing them.

### Python → C++

```sh
export PYTHONPATH=build/python
python3 python/draw.py
python3 python/draw.py --center -0.743643887 0.131825904 --zoom 3000 --iter 2000 --out zoom.png
```

| Option | Default | Meaning |
|---|---|---|
| `--width`, `--height` | `1200`, `800` | Image size in pixels |
| `--center X Y` | `-0.6 0.0` | Center of the view in the complex plane |
| `--zoom` | `1.0` | Magnification; zoom 1 spans 3.6 units horizontally |
| `--iter` | `500` | Maximum iterations |
| `--out` | — | Save to this PNG instead of opening a window |

Colors and layout live in `python/plot.py`; changes there need no rebuild.

### Using the module directly

```python
import mandelbrot

image = mandelbrot.compute(width=800, height=600,
                           x_min=-2.4, x_max=1.2, y_min=-1.2, y_max=1.2,
                           max_iter=500, threads=0)   # threads=0: all cores
# image: float32 array (600, 800); value == max_iter means inside the set

mandelbrot.escape(-0.5, 0.0, 500)   # smooth escape value of one point
```

Invalid arguments raise `ValueError`. The GIL is released during the calculation.

## Test

```sh
ctest --test-dir build --output-on-failure   # C++ unit tests + end-to-end
pytest                                       # Python tests
```

| Test | Checks |
|---|---|
| `tests/cpp/test_mandelbrot.cpp` | Inside/outside points, every pixel filled, symmetry, identical result for any thread count, invalid input rejected |
| `end_to_end` (CTest) | `build/mandelbrot` runs and Python saves the image |
| `tests/python/test_module.py` | Module returns a correct NumPy array, identical result for any thread count, bad input raises `ValueError` |
| `tests/python/test_draw.py` | `draw.py` computes the right area from center and zoom |
| `tests/python/test_plot.py` | Shading marks inside points, input isn't modified, PNG is saved |

CI runs both suites on Ubuntu and macOS (`.github/workflows/tests.yml`).

## Troubleshooting

| Problem | Fix |
|---|---|
| `No module named 'plot'` | Run from the project root, not `build/`. |
| `No module named 'mandelbrot'` (draw.py) | Build first, then `export PYTHONPATH=build/python`. |
| `No module named 'numpy'` / `'matplotlib'` | Activate the environment: `source .venv/bin/activate`. |
| CMake can't find `pybind11` | Activate the environment, delete `build/`, run `cmake -B build` again. |
| `pytest` can't import a module | Build first and run `pytest` from the project root. |
| No window appears (SSH, container) | Save to a file: `./build/mandelbrot out.png` or `--out out.png`. |
