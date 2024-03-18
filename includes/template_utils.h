#pragma once

#include <vector>
#include <memory>
#include <optional>

#include "template_types.h"


namespace tplgen {

    std::unique_ptr<std::string> getMessage(const std::string &msg, const char* file, int line);
    void printInfo(const std::string &msg, const char* file, int line);
    void printError(const std::string &msg, const char* file, int line);

#ifdef __linux__
    pid_t createFork();
#endif

    double getRandomNumber(double min, double max);
    size_t getRandomNumber(size_t min, size_t max);

    std::unique_ptr<std::vector<double>> getRandomVec1D(size_t count, double min_val, double max_val);
    std::unique_ptr<mat_1d_t> getRandomMat1d(size_t count_h, size_t count_w, double min_val, double max_val);

    std::unique_ptr<std::vector<Vec2d>> getRandomVec2D(size_t count, double min_val, double max_val);
    std::unique_ptr<mat_2d_t> getRandomMat2d(size_t count_h, size_t count_w, double min_val, double max_val);    
    std::unique_ptr<std::vector<std::unique_ptr<mat_2d_t>>> getXRandomMat2d(size_t count_mat, size_t count_h, size_t count_w, 
            double min_val, double max_val);

    std::unique_ptr<std::vector<Vec2d>> getVec2D(size_t count, double val_x, double val_y);
    std::unique_ptr<mat_2d_t> getMat2d(size_t count_h, size_t count_w, double val_x, double val_y);
    std::unique_ptr<std::vector<std::unique_ptr<mat_2d_t>>> getPNoiseBackground(size_t count_mat, size_t count_h, size_t count_w,
            double val_x, double val_y);

    std::unique_ptr<pn_2d_t> getTemplateBackground(size_t w, size_t h, double val);

    void createDirectory(const std::string &dir_path);
    bool directoryExists(const std::string &dir_path);

}

