#include <math.h>
#include <iostream>
#include "interpolation.h"


namespace tplgen {

    static std::pair<double, double> calcFunction(const Vec2d& point_1, const Vec2d& point_2) {
        double slope;
        double intercept;

        if (point_1.x == point_2.x) {
            slope = 0.0;
            intercept = point_1.y;
        }
        else if (point_1.x < point_2.x) {
            slope = (point_2.y - point_1.y)/(point_2.x - point_1.x);
            intercept = point_1.y - slope*point_1.x;
        } 
        else {
            slope = (point_1.y - point_2.y)/(point_1.x - point_2.x);
            intercept = point_2.y - slope*point_2.x;
        }

        return std::make_pair(slope, intercept);
    }

    std::unique_ptr<std::vector<Vec2d>> calcLinearizationPoints(const Vec2d &point_1, const Vec2d &point_2) {
        auto results = std::make_unique<std::vector<Vec2d>>();

        std::pair<double, double> lin_func = calcFunction(point_1, point_2);
        double slope = lin_func.first;
        double intercept = lin_func.second;
        // rounding is necessary to avoid holes in the linearization
        if (std::abs(point_1.x - point_2.x) > std::abs(point_1.y - point_2.y)) {  // y linerarization
            double x_start = std::floor(point_1.x);
            double x_end = std::ceil(point_2.x);
            if (point_1.x > point_2.x) {
                x_start = std::floor(point_2.x);
                x_end = std::ceil(point_1.x);
            }
            for (double x = x_start; x <= x_end; x += 1.0) {
                double y = x*slope + intercept;
                results->push_back({x, y});
            }
        } else {  // x linerarization
            double y_start = std::floor(point_1.y);
            double y_end = std::ceil(point_2.y);
            if (point_1.y > point_2.y) {
                y_start = std::floor(point_2.y);
                y_end = std::ceil(point_1.y);
            }
            for (double y = y_start; y < y_end; y += 1.0) {
                double x = (y - intercept)/slope;
                results->push_back({x, y});
            }
        }
        
        return results;
    }

    std::unique_ptr<std::vector<Vec2d>> calcLinearizationPoints(const CrackPoint &crack_1, const CrackPoint &crack_2) {
        Vec2d point_1 = {static_cast<double>(crack_1.x), static_cast<double>(crack_1.y)};
        Vec2d point_2 = {static_cast<double>(crack_2.x), static_cast<double>(crack_2.y)};

        return calcLinearizationPoints(point_1, point_2);
    }


    double calcLinearizationPoint(const Vec2d& start, const Vec2d& end, double x) {
        std::pair<double, double> lin_func = calcFunction(start, end);
        double slope = lin_func.first;
        double intercept = lin_func.second;

        return x*slope + intercept;
    }

    double calcLinearizationPoint(double a, double b, double x) {
        if (a > b) {
            double distance = a - b; 
            return distance*x + b;
        }
        else {
            double distance = b - a;
            return distance*x + a;
        }
    }

    double quadraticInterpolation(double a, double b, double x) {
        if (a > b) {
            double distance = a - b; 
            return distance*x + b;
        }
        else {
            double distance = b - a;
            return distance*x + a;
        }
    }

}