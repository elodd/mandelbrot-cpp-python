# mandelbrot: C++ ↔ Python

The Mandelbrot set is computed in C++ and drawn in Python, in two directions:

- **C++ → Python:** the C++ program `mandelbrot` computes the image and sends it to Python to draw.
- **Python → C++:** the Python program `draw.py` calls the C++ calculation through a Python module and draws the result.

Both use the same calculation (`cpp/mandelbrot.hpp`) and the same drawing code (`python/plot.py`).

```
cpp/mandelbrot.hpp       the calculation
cpp/main.cpp             C++ program: computes, sends the image to Python
cpp/module.cpp           Python module "mandelbrot" (wraps the calculation)
python/plot.py           draws an image (used by both programs)
python/draw.py           Python program: calls the C++ module, then draws
tests/cpp/               C++ tests (GoogleTest) + end-to-end test
tests/python/            Python tests (pytest)
```

## From start to end

### 1. Install the tools

macOS:

```sh
xcode-select --install          # C++ compiler
brew install cmake python
```

Ubuntu/Debian:

```sh
sudo apt install build-essential cmake python3-dev python3-venv
```

### 2. Get the code

```sh
unzip mandelbrot-cpp-python.zip
cd mandelbrot-cpp-python
```

### 3. Create a Python environment and install packages

```sh
python3 -m venv .venv
source .venv/bin/activate
pip install pybind11 numpy matplotlib pytest
```

### 4. Build

```sh
cmake -B build
cmake --build build
```

This creates:

- `build/mandelbrot`: the C++ program
- `build/python/mandelbrot.*.so`: the Python module
- the C++ tests (the first build downloads GoogleTest)

### 5. Plot

Run from the project folder (not from `build/`), with the environment from step 3 active.

C++ computes, Python draws:

```sh
./build/mandelbrot              # opens a window
./build/mandelbrot out.png      # saves the image to out.png
```

Python calls C++, then draws:

```sh
export PYTHONPATH=build/python
python3 python/draw.py
python3 python/draw.py --center -0.743643887 0.131825904 --zoom 3000 --iter 2000 --out zoom.png
```

`draw.py` options: `--width`, `--height`, `--center X Y`, `--zoom`, `--iter`, `--out`.

### 6. Change the picture

For the C++ program, edit the constants at the top of `cpp/main.cpp` (size, iterations, area of the plane), then rebuild:

```sh
cmake --build build
./build/mandelbrot
```

For `draw.py`, use its command-line options; no rebuild needed. Changes to `python/plot.py` (colors, layout) also need no rebuild.

### 7. Run the tests

```sh
ctest --test-dir build --output-on-failure   # C++ tests + end-to-end run
pytest                                       # Python tests
```

| Test | Checks |
|---|---|
| `tests/cpp/test_mandelbrot.cpp` | Inside/outside points, every pixel filled, symmetry, same result for any thread count, invalid input rejected |
| `end_to_end` (CTest) | The C++ program runs: C++ computes, Python saves the image |
| `tests/python/test_module.py` | Python module returns a correct NumPy array, same result for any thread count, bad input raises `ValueError` |
| `tests/python/test_draw.py` | `draw.py` computes the right area for center and zoom |
| `tests/python/test_plot.py` | Shading marks inside points, input not modified, PNG is saved |

## Troubleshooting

| Problem | Fix |
|---|---|
| `No module named 'plot'` | Run from the project folder, not from `build/`. |
| `No module named 'mandelbrot'` (draw.py) | Build first, then `export PYTHONPATH=build/python`. |
| `No module named 'numpy'` or `'matplotlib'` | Activate the environment (`source .venv/bin/activate`) and run again. |
| CMake can't find `pybind11` | Activate the environment, delete `build/`, run `cmake -B build` again. |
| `pytest` can't find a module | Build first, and run `pytest` from the project folder. |
| No window appears (SSH, container) | Save to a file instead: `--out out.png` or `./build/mandelbrot out.png`. |
