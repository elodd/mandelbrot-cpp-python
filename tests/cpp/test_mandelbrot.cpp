#include <gtest/gtest.h>

#include <vector>

#include "mandelbrot.hpp"

using namespace mandelbrot;

TEST(Escape, PointsInsideTheSetReachMaxIter) {
    EXPECT_EQ(escape(0.0, 0.0, 100), 100.0f);    // center of the main body
    EXPECT_EQ(escape(-1.0, 0.0, 100), 100.0f);   // center of the left bulb
}

TEST(Escape, PointsFarOutsideEscapeQuickly) {
    EXPECT_LT(escape(2.0, 2.0, 100), 3.0f);
}

TEST(Escape, ValueGrowsCloserToTheSet) {
    EXPECT_LT(escape(1.0, 0.0, 500), escape(0.3, 0.0, 500));
}

TEST(Compute, FillsEveryPixel) {
    const int w = 30, h = 20;
    std::vector<float> img(w * h, -1.0f);
    compute(img.data(), w, h, Area{-2.4, 1.2, -1.2, 1.2}, 100);
    for (float v : img) {
        EXPECT_GE(v, 0.0f);
        EXPECT_LE(v, 100.0f);
    }
}

TEST(Compute, ImageIsMirroredAroundTheRealAxis) {
    // The set is symmetric, so with y_min = -y_max the top and bottom halves match
    const int w = 40, h = 20;
    std::vector<float> img(w * h);
    compute(img.data(), w, h, Area{-2.0, 1.0, -1.0, 1.0}, 200);
    for (int y = 0; y < h / 2; ++y)
        for (int x = 0; x < w; ++x)
            EXPECT_NEAR(img[y * w + x], img[(h - 1 - y) * w + x], 1e-3f);
}

TEST(Compute, RejectsInvalidArguments) {
    std::vector<float> img(4);
    const Area ok{-2, 1, -1, 1};
    EXPECT_THROW(compute(img.data(), 0, 2, ok, 10), std::invalid_argument);
    EXPECT_THROW(compute(img.data(), 2, 2, ok, 0), std::invalid_argument);
    EXPECT_THROW(compute(img.data(), 2, 2, Area{1, -2, -1, 1}, 10), std::invalid_argument);
}

TEST(Compute, ThreadCountDoesNotChangeTheResult) {
    const Area area{-2.4, 1.2, -1.2, 1.2};
    EXPECT_EQ(compute(64, 48, area, 200, 1), compute(64, 48, area, 200, 7));
}

TEST(Compute, VectorOverloadMatchesBufferVersion) {
    const int w = 30, h = 20;
    const Area area{-2.4, 1.2, -1.2, 1.2};
    std::vector<float> buf(w * h);
    compute(buf.data(), w, h, area, 100);
    EXPECT_EQ(compute(w, h, area, 100), buf);
}

TEST(Compute, RejectsNullBuffer) {
    EXPECT_THROW(compute(nullptr, 2, 2, Area{-2, 1, -1, 1}, 10), std::invalid_argument);
}
