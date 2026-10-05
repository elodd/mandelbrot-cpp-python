import pytest

from draw import area


def test_area_is_centered_and_keeps_aspect_ratio():
    x_min, x_max, y_min, y_max = area((0.5, -0.25), 1.0, 300, 200)
    assert (x_min + x_max) / 2 == pytest.approx(0.5)
    assert (y_min + y_max) / 2 == pytest.approx(-0.25)
    assert (x_max - x_min) / (y_max - y_min) == pytest.approx(300 / 200)


def test_zoom_shrinks_the_area():
    x_min, x_max, _, _ = area((0.0, 0.0), 4.0, 100, 100)
    assert x_max - x_min == pytest.approx(3.6 / 4)


def test_zoom_must_be_positive():
    with pytest.raises(ValueError):
        area((0.0, 0.0), 0.0, 100, 100)
