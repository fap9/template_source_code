#pragma once

#include <memory>
#include <utility>
#include "template_types.h"


namespace tplgen {

    std::unique_ptr<std::vector<Vec2d>> calcLinearizationPoints(const Vec2d &point_1, const Vec2d &point_2);
    std::unique_ptr<std::vector<Vec2d>> calcLinearizationPoints(const CrackPoint &crack_1, const CrackPoint &crack_2);
    double calcLinearizationPoint(const Vec2d& point_1, const Vec2d& point_2, double x);
    double calcLinearizationPoint(double a, double b, double x);
    double quadraticInterpolation(double a, double b, double x);

}