"""Receives the Mandelbrot data from C++ and draws it."""

import matplotlib.pyplot as plt
import numpy as np


def shade(image: np.ndarray, max_iter: int) -> np.ndarray:
    """Compress the value range for nicer colors; points inside the set become NaN (black)."""
    shaded = np.log1p(image)
    shaded[image >= max_iter] = np.nan
    return shaded


def show(image: np.ndarray, max_iter: int, out: str = "") -> None:
    print(f"Python: received {image.shape[1]}x{image.shape[0]} {image.dtype} array")

    cmap = plt.get_cmap("twilight_shifted").with_extremes(bad="black")
    fig = plt.figure(figsize=(12, 8))
    plt.imshow(shade(image, max_iter), cmap=cmap)
    plt.axis("off")
    plt.tight_layout()

    if out:
        fig.savefig(out, dpi=100)
        plt.close(fig)
        print(f"Python: saved {out}")
    else:
        plt.show()
