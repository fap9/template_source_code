#pragma once

#include <vector>
#include <memory>
#include <map>

namespace tplgen {

    struct Vec2d {
        double x;
        double y;
    };

    struct PN1d {
        size_t x;
        double y;
    };

    struct PN2d {
        size_t x;
        size_t y;
        double value;
    };

    struct CrackPoint {
        size_t x;
        double y;
        double height;
    };

    // types for random data
    typedef std::vector<std::unique_ptr<std::vector<double>>> mat_1d_t;
    typedef std::vector<std::unique_ptr<std::vector<Vec2d>>> mat_2d_t;
    // Perlin Noise calculation result types
    typedef std::vector<PN1d> pn_1d_t;
    typedef std::vector<PN2d> pn_2d_t;
    typedef std::map<size_t, Vec2d> pn_vec_t;
    typedef std::vector<CrackPoint> pn_crack;

}
