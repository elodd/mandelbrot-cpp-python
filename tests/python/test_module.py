"""Tests for the Python module built from cpp/module.cpp."""

import numpy as np
import pytest

import mandelbrot

AREA = (-2.4, 1.2, -1.2, 1.2)


def test_returns_float32_array_of_requested_shape():
    image = mandelbrot.compute(40, 30, *AREA, 100)
    assert isinstance(image, np.ndarray)
    assert image.shape == (30, 40)
    assert image.dtype == np.float32


def test_values_are_in_range():
    image = mandelbrot.compute(40, 30, *AREA, 100)
    assert image.min() >= 0
    assert image.max() == 100          # some pixels are inside the set


def test_escape_inside_and_outside():
    assert mandelbrot.escape(0.0, 0.0, 100) == 100
    assert mandelbrot.escape(2.0, 2.0, 100) < 3


def test_thread_count_does_not_change_result():
    one = mandelbrot.compute(64, 48, *AREA, 200, threads=1)
    many = mandelbrot.compute(64, 48, *AREA, 200, threads=8)
    assert np.array_equal(one, many)


@pytest.mark.parametrize(
    "args",
    [
        (0, 10, *AREA, 100),               # bad size
        (10, 10, *AREA, 0),                # bad max_iter
        (10, 10, 1.0, -2.0, -1.0, 1.0, 100),  # x_max < x_min
    ],
)
def test_invalid_input_raises_value_error(args):
    with pytest.raises(ValueError):
        mandelbrot.compute(*args)
