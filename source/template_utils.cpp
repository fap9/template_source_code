#include <random>
#include <iostream>
#include <memory>

#ifdef _WIN32
    #include <direct.h>
    #include <windows.h>
#else
    #include <sys/stat.h>
    #include <unistd.h>
#endif

#include "template_utils.h"


namespace tplgen {

    template <typename D>
    static void checkParaVec(D &min_val, D &max_val) {
        if (min_val > max_val) {
            printInfo("Minimun value cannot be greater than maximum value, changed to 0 and 1!", __FILE__, __LINE__);
            min_val = 0.;
            max_val = 1.;            
        }
    }

    std::unique_ptr<std::string> getMessage(const std::string &msg, const char* file, int line) {
        auto info = std::make_unique<std::string>(std::string(msg) + 
         " (" + std::string(file) + ": line " + std::to_string(line) + ")");
        return info;
    }

    void printInfo(const std::string &msg, const char* file, int line) {
        std::cout << *getMessage("[INFO] " + msg, file, line) << std::endl;
    }

    void printError(const std::string &msg, const char* file, int line) {
        std::cerr << *getMessage("[ERROR] " + msg, file, line) << std::endl;
    }


#ifdef __linux__
    pid_t createFork() {
        pid_t pid = fork();
        switch(pid) {
            case -1:
                printError("Fork failed!", __FILE__, __LINE__);
                exit(EXIT_FAILURE);
            case 0:  // child 
                return pid;
            default:  // parent
                return pid;
        }
    }
#endif


    double getRandomNumber(double min, double max) {
        std::random_device rand_dev;  // seed dor rand. numbers
        std::mt19937 rand_generator(rand_dev());  // pseudorandom number generator
        std::uniform_real_distribution<double> rand_distribution(min, max);  // setting range 

        return rand_distribution(rand_generator);
    }

    size_t getRandomNumber(size_t min, size_t max) {
        std::random_device rand_dev;  // seed dor rand. numbers
        std::mt19937 rand_generator(rand_dev());  // pseudorandom number generator
        std::uniform_int_distribution<size_t> rand_distribution(min, max);  // setting range 

        return rand_distribution(rand_generator);
    }


    std::unique_ptr<std::vector<double>> getRandomVec1D(size_t count, double min_val, double max_val) {
        checkParaVec(min_val, max_val);
        std::random_device rand_dev;  // seed dor rand. numbers
        std::mt19937 rand_generator(rand_dev());  // pseudorandom number generator
        std::uniform_real_distribution<double> rand_distribution(min_val, max_val);  // setting range 

        auto vec_rand = std::make_unique<std::vector<double>>();
        for (size_t idx = 0; idx < count; idx++) {
            vec_rand->push_back(rand_distribution(rand_generator));
        }

        return vec_rand;
    }

    std::unique_ptr<mat_1d_t> getRandomMat1d(size_t count_h, size_t count_w, double min_val, double max_val) {
        auto mat_rand = std::make_unique<mat_1d_t>();
        for (size_t idx = 0; idx < count_h; idx++) {
            mat_rand->push_back(getRandomVec1D(count_w, min_val, max_val));
        }

        return mat_rand;
    }


    std::unique_ptr<std::vector<Vec2d>> getRandomVec2D(size_t count, double min_val, double max_val) {
        checkParaVec(min_val, max_val);
        std::random_device rand_dev;  // seed dor rand. numbers
        std::mt19937 rand_generator(rand_dev());  // pseudorandom number generator
        std::uniform_real_distribution<double> rand_distribution(min_val, max_val);  // setting range 

        auto vec_rand = std::make_unique<std::vector<Vec2d>>();
        for (size_t idx = 0; idx < count; idx++) {
            vec_rand->push_back({rand_distribution(rand_generator), rand_distribution(rand_generator)});
        }

        return vec_rand;
    }

    std::unique_ptr<mat_2d_t> getRandomMat2d(size_t count_h, size_t count_w, double min_val, double max_val) {
        auto mat_rand = std::make_unique<mat_2d_t>();
        for (size_t idx = 0; idx < count_h; idx++) {
            mat_rand->push_back(getRandomVec2D(count_w, min_val, max_val));
        }

        return mat_rand;
    }

    std::unique_ptr<std::vector<std::unique_ptr<mat_2d_t>>> getXRandomMat2d(size_t count_mat, size_t count_h, size_t count_w, double min_val, double max_val) {
        auto mat_x_rand = std::make_unique<std::vector<std::unique_ptr<mat_2d_t>>>();
        for (size_t idx = 0; idx < count_mat; idx++) {
            mat_x_rand->push_back(getRandomMat2d(count_h, count_w, min_val, max_val));
        }

        return mat_x_rand;
    }


    std::unique_ptr<std::vector<Vec2d>> getVec2D(size_t count, double val_x, double val_y) {
        auto vec = std::make_unique<std::vector<Vec2d>>();
        for (size_t idx = 0; idx < count; idx++) {
            vec->push_back({val_x, val_y});
        }

        return vec;
    }

    std::unique_ptr<mat_2d_t> getMat2d(size_t count_h, size_t count_w, double val_x, double val_y) {
        auto mat = std::make_unique<mat_2d_t>();
        for (size_t idx = 0; idx < count_h; idx++) {
            mat->push_back(getVec2D(count_w, val_x, val_y));
        }

        return mat;
    }

    std::unique_ptr<std::vector<std::unique_ptr<mat_2d_t>>> getPNoiseBackground(size_t count_mat, size_t count_h, size_t count_w, 
            double val_x, double val_y) {
        auto background = std::make_unique<std::vector<std::unique_ptr<mat_2d_t>>>();
        for (size_t idx = 0; idx < count_mat; idx++) {
            background->push_back(getMat2d(count_h, count_w, val_x, val_y));
        }

        return background;
    }


    std::unique_ptr<pn_2d_t> getTemplateBackground(size_t w, size_t h, double val) {
        auto background = std::make_unique<pn_2d_t>();
        for (size_t idx_y = 0; idx_y < h; idx_y++) {
            for (size_t idx_x = 0; idx_x < w; idx_x++) {
                background->push_back({idx_x, idx_y, val});
            }
        }

        return background;
    }


    bool directoryExists(const std::string &dir_path) {
#ifdef _WIN32
        DWORD f_typ = GetFileAttributesA(dir_path.c_str());
        if (f_typ == INVALID_FILE_ATTRIBUTES) {
            return false;  // path is wrong
        } 
        if (f_typ & FILE_ATTRIBUTE_DIRECTORY) {
            return true;   
        }
        return false; // not a directory

#elif __linux__
        struct stat info;
        if (stat(dir_path.c_str(), &info) != 0) {
            return false;
        } else if (info.st_mode & S_IFDIR) {
            return true;
        }
#endif
        return false;
    }

    void createDirectory(const std::string &dir_path) {
        if (directoryExists(dir_path)) return;

        int result = -1;
#ifdef _WIN32
            result = _mkdir(dir_path.c_str());
#elif __linux__
            result = mkdir(dir_path.c_str(), 0777);
#endif

        if (result != 0) {
            printError("Can't create the directory \"" + dir_path + "\" " , __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }
    }

}