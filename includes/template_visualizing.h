#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include <string>
#include "template_types.h"


namespace tplgen {

    constexpr int64_t SCALE = 50;

    enum OFFSize {
        METER = 1,
        DECIMETER = 10,
        CENTIMETER = 100,
        MILLIMETER = 1000
    };

    class PNVisualizing {
    private:
        int64_t _scale = 1;

        void axisLabeling(sf::RenderWindow &window, size_t x_max, int64_t y_max);

    public:
        bool saveImgPGMAscii(const char* save_path, const pn_2d_t* output_2d, size_t output_w, size_t output_h);
        bool saveImgPGMBinary(const char* save_path, const pn_2d_t* output_2d, size_t output_w, size_t output_h);
        bool saveTeplOff(const char* save_path, const pn_2d_t* output_2d, size_t output_w, size_t output_h, double amp = 1.0, OFFSize size = OFFSize::METER);

        bool showImg(const pn_2d_t* output_2d, size_t output_w, size_t output_h, size_t scale = 1);

        void plotOutput(const pn_1d_t* output_1d, const char* label, int64_t scale = SCALE);
    
    };
}