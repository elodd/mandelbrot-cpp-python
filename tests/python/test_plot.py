import numpy as np

from plot import shade, show

MAX_ITER = 100


def sample_image() -> np.ndarray:
    """Small image like the one C++ sends: float32, some pixels inside the set."""
    img = np.linspace(0, MAX_ITER, 12, dtype=np.float32).reshape(3, 4)
    img[1, 1] = MAX_ITER
    return img


def test_shade_marks_inside_points_as_nan():
    img = sample_image()
    shaded = shade(img, MAX_ITER)
    assert np.isnan(shaded[img >= MAX_ITER]).all()
    assert np.isfinite(shaded[img < MAX_ITER]).all()


def test_shade_keeps_shape_and_does_not_change_input():
    img = sample_image()
    before = img.copy()
    assert shade(img, MAX_ITER).shape == img.shape
    assert np.array_equal(img, before)


def test_show_saves_png(tmp_path, capsys):
    out = tmp_path / "out.png"
    show(sample_image(), MAX_ITER, str(out))
    assert out.read_bytes().startswith(b"\x89PNG")
    assert "Python: received 4x3 float32 array" in capsys.readouterr().out
