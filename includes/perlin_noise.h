#pragma once

#include <memory>
#include <vector>

#include "template_types.h"


namespace tplgen {

    constexpr double RAND_MIN_PN = -1.0;
    constexpr double RAND_MAX_PN = 1.0;
    constexpr size_t DEFAULT_OUTPUT = 64;


    double dotProduct(const Vec2d &vec_1, const Vec2d &vec_2);
    double interpolationCubic(double x, double y, double weigth);
    double interpolationLinear(double a, double b, double weight);

    std::unique_ptr<pn_1d_t> calcPNoise1D(size_t number_of_elem, size_t octaves_count = 4, double increase_amp = 2.0);
    std::unique_ptr<pn_crack> calcPNoiseCrack(size_t number_of_elem, size_t octaves_count = 4, double increase_amp = 2.0);

    std::unique_ptr<pn_2d_t> calcPNoise2DEmphasis(size_t grid_size, size_t octaves_count = 4, double contrast = 1.0);
    std::unique_ptr<pn_2d_t> calcPNoise2DRandom(size_t grid_size, size_t output_h, size_t output_w, size_t octaves_count = 4, double contrast = 1.0);

    std::unique_ptr<pn_vec_t> calcPNoiseCircle(size_t grid_size, size_t output_h, size_t output_w, size_t circle_samples, size_t octaves_count = 4, double contrast = 1.0);

}
