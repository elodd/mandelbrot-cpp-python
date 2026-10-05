"""Python program: calls the C++ calculation (mandelbrot module), then draws with plot.py."""

import argparse
import time

import mandelbrot
from plot import show

FULL_HALF_WIDTH = 1.8   # half-width of the view at zoom 1


def area(center: tuple[float, float], zoom: float, width: int, height: int):
    """(x_min, x_max, y_min, y_max) around center, keeping pixels square."""
    if zoom <= 0:
        raise ValueError("zoom must be > 0")
    half_w = FULL_HALF_WIDTH / zoom
    half_h = half_w * height / width
    cx, cy = center
    return cx - half_w, cx + half_w, cy - half_h, cy + half_h


def main() -> None:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--width", type=int, default=1200)
    p.add_argument("--height", type=int, default=800)
    p.add_argument("--center", type=float, nargs=2, default=(-0.6, 0.0), metavar=("X", "Y"))
    p.add_argument("--zoom", type=float, default=1.0)
    p.add_argument("--iter", type=int, default=500)
    p.add_argument("--out", default="", help="save to this PNG instead of opening a window")
    a = p.parse_args()

    t0 = time.perf_counter()
    image = mandelbrot.compute(a.width, a.height, *area(tuple(a.center), a.zoom, a.width, a.height), a.iter)
    print(f"C++: computed {a.width}x{a.height} in {time.perf_counter() - t0:.2f}s")
    show(image, a.iter, a.out)


if __name__ == "__main__":
    main()
