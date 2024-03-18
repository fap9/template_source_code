#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <tuple>

#include "template_utils.h"
#include "perlin_noise.h"
#include "template_visualizing.h"


namespace tplgen {

    static void checkPara(size_t grid_size, size_t &output_h, size_t &output_w) {
        if (grid_size > output_h || grid_size > output_w || grid_size == 0) grid_size = output_h;
        size_t rest;
        if (grid_size != output_w && (rest = output_w % grid_size) != 0) {
            output_w -= rest;
            printInfo("output width haven't fit to grid_size, adjusted to " + std::to_string(output_w), __FILE__, __LINE__);
        }
        if (grid_size != output_h && (rest = output_h % grid_size) != 0) {
            output_h -= rest;
            printInfo("output height haven't fit to grid_size, adjusted to " + std::to_string(output_h), __FILE__, __LINE__);
        }
    }

    double dotProduct(const Vec2d &vec_1, const Vec2d &vec_2) {
        return vec_1.x*vec_2.x + vec_1.y*vec_2.y;
    }

    double interpolationCubic(double x, double y, double weigth) {
        auto d = std::pow(weigth, 3) * (weigth * (weigth*6.0 - 15.0) + 10.0);
        return (1-d)*x + d*y;
    }

    double interpolationCos(double a, double b, double weigth) {
        double d = weigth * 3.1415927;
        double f = (1.0 - cos(d)) * 0.5;
        return a * (1.0 - f) + b * f;
    }

    double interpolationLinear(double a, double b, double weight) {
        return a * (1 - weight) + b * weight;
    }


    std::unique_ptr<pn_2d_t> calcPNoise2DEmphasis(size_t grid_size, size_t octaves_count, double contrast) {
        if (grid_size == 0) grid_size = 128;
        constexpr double MAX_VAL = -0.75;
        constexpr size_t COUNT_H = 3;
        constexpr size_t COUNT_W = 3;
        std::unique_ptr<std::vector<std::unique_ptr<mat_2d_t>>> mat = getPNoiseBackground(octaves_count, COUNT_H, COUNT_W, 0.0, 0.0);

        double emphasis_x = getRandomNumber(0.75, 1.5);
        double emphasis_y = getRandomNumber(0.75, 1.5);
        for (size_t idx_octave = 0; idx_octave < octaves_count; idx_octave++) {
            for (int64_t idx_h = 0; idx_h < static_cast<int64_t>(COUNT_H); idx_h++) {
                for (int64_t idx_w = 0; idx_w < static_cast<int64_t>(COUNT_W); idx_w++) {
                    Vec2d gradient;
                    if (idx_w == emphasis_x && idx_h == emphasis_y) {
                        gradient = {0, 0};
                    } else {
                        Vec2d direction = {emphasis_x - static_cast<double>(idx_w), emphasis_y - static_cast<double>(idx_h)};
                        double length = std::sqrt(direction.x*direction.x + direction.y*direction.y);
                        double scale_x = std::abs(direction.x);
                        double scale_y = std::abs(direction.y);
                        double scale = scale_x > scale_y ? scale_x : scale_y;
                        if (length == 0.0 || scale == 0.0) gradient = {0.0, 0.0};
                        else gradient = {(direction.x/length)*(MAX_VAL/scale), (direction.y/length)*(MAX_VAL/scale)};
                    }

                    mat->at(idx_octave)->at(idx_h)->at(idx_w) = gradient;
                }
            }
        }
        auto output_2d = std::make_unique<pn_2d_t>(grid_size*grid_size);        
        size_t grid_idx_w = 0;
        size_t grid_idx_h = 0;

        for (size_t idx_h = 0; idx_h < grid_size; idx_h++) {
            double idx_h_d = static_cast<double>(idx_h);  // for floating point calculation
            grid_idx_w = 0;
            for (size_t idx_w = 0; idx_w < grid_size; idx_w++) {                           
                double idx_w_d = static_cast<double>(idx_w);  // for floating point calculation                                             
                double weigth_w = idx_w_d/grid_size - grid_idx_w;
                double weigth_h = idx_h_d/grid_size - grid_idx_h;

                PN2d pixel = {idx_w, idx_h, 0.0};
                double frequency = 1.0;
                double amplitude = 1.0;   
                for (size_t octave = 0; octave < octaves_count; octave++) {
                    double idx_w_d_2 = idx_w_d*frequency/grid_size;
                    double idx_h_d_2 = idx_h_d*frequency/grid_size;
                    // determine location vectors                  
                    Vec2d vec_top_left{idx_w_d_2 - grid_idx_w, idx_h_d_2 - grid_idx_h};
                    Vec2d vec_top_right{idx_w_d_2 - (grid_idx_w + 2), idx_h_d_2 - grid_idx_h};
                    Vec2d vec_bottom_left{idx_w_d_2 - grid_idx_w, idx_h_d_2 - (grid_idx_h + 2)};
                    Vec2d vec_bottom_right{idx_w_d_2 - (grid_idx_w + 2), idx_h_d_2 - (grid_idx_h + 2)};
                    Vec2d vec_left{idx_w_d_2 - grid_idx_w, idx_h_d_2 - (grid_idx_h + 1)};
                    Vec2d vec_right{idx_w_d_2 - (grid_idx_w + 2), idx_h_d_2 - (grid_idx_h + 1)};
                    Vec2d vec_top{idx_w_d_2 - (grid_idx_w + 1), idx_h_d_2 - (grid_idx_h)};
                    Vec2d vec_bottom{idx_w_d_2 - (grid_idx_w + 1), idx_h_d_2 - (grid_idx_h + 2)};
                    // determine dot products
                    double dot_left_top = dotProduct(vec_top_left, (*(*(*mat)[octave])[grid_idx_h])[grid_idx_w]);
                    double dot_right_top = dotProduct(vec_top_right, (*(*(*mat)[octave])[grid_idx_h])[grid_idx_w + 2]);
                    double dot_left_bottom = dotProduct(vec_bottom_left, (*(*(*mat)[octave])[grid_idx_h + 2])[grid_idx_w]);
                    double dot_right_bottom = dotProduct(vec_bottom_right, (*(*(*mat)[octave])[grid_idx_h + 2])[grid_idx_w + 2]);
                    double dot_top = dotProduct(vec_top, (*(*(*mat)[octave])[grid_idx_h])[grid_idx_w + 1]);
                    double dot_bottom = dotProduct(vec_bottom, (*(*(*mat)[octave])[grid_idx_h + 2])[grid_idx_w + 1]);
                    double dot_center_left = dotProduct(vec_left, (*(*(*mat)[octave])[grid_idx_h + 1])[grid_idx_w]);
                    double dot_center_right = dotProduct(vec_right, (*(*(*mat)[octave])[grid_idx_h + 1])[grid_idx_w + 2]);                                        
                    // calculate interpolation                    
                    double inter_w_1 = interpolationCubic(dot_left_top, dot_right_top, weigth_w);
                    double inter_w_2 = interpolationCubic(dot_left_bottom, dot_right_bottom, weigth_w);
                    double inter_w_3 = interpolationCubic(dot_center_left, dot_center_right, weigth_w);
                    double inter_w_4 = interpolationCubic(dot_bottom, dot_top, weigth_w);
                    double inter_h_1 = interpolationCubic(inter_w_1, inter_w_2, weigth_h);
                    double inter_h_2 = interpolationCubic(inter_w_3, inter_w_4, weigth_h);
                    double inter_h = interpolationCubic(inter_h_1, inter_h_2, weigth_h);
                    // octave result                             
                    inter_h *= amplitude;
                    pixel.value += inter_h;

                    frequency *= 2;
                    amplitude /= 8;
                }                          
                pixel.value *= contrast;
                (*output_2d)[idx_h*grid_size + idx_w] = pixel;                
                if ((idx_w + 1) % grid_size == 0) grid_idx_w += 1; 
            }
            if ((idx_h + 1) % grid_size == 0) grid_idx_h += 1;
        }

        return output_2d;
    }

    std::unique_ptr<pn_2d_t> calcPNoise2DRandom(size_t grid_size, size_t output_h, size_t output_w, size_t octaves_count, double contrast) {
        checkPara(grid_size, output_h, output_w);
        size_t count_h = output_h/grid_size + 1;
        size_t count_w = output_w/grid_size + 1;
        std::unique_ptr<std::vector<std::unique_ptr<mat_2d_t>>> mat = getXRandomMat2d(octaves_count, count_h, count_w, RAND_MIN_PN, RAND_MAX_PN);

         auto output_2d = std::make_unique<pn_2d_t>(output_h*output_w);        
        size_t grid_idx_w = 0;
        size_t grid_idx_h = 0;
        // loop through output raster
        for (size_t idx_h = 0; idx_h < output_h; idx_h++) {
            double idx_h_d = static_cast<double>(idx_h);  // for floating point calculation
            grid_idx_w = 0;
            for (size_t idx_w = 0; idx_w < output_w; idx_w++) {                           
                double idx_w_d = static_cast<double>(idx_w);  // for floating point calculation                                             
                double weigth_w = idx_w_d/grid_size - grid_idx_w;
                double weigth_h = idx_h_d/grid_size - grid_idx_h;

                PN2d pixel = {idx_w, idx_h, 0.0};
                double frequency = 1.0;
                double amplitude = 1.0;   
                for (size_t octave = 0; octave < octaves_count; octave++) {
                    double idx_w_d_2 = idx_w_d*frequency/grid_size;
                    double idx_h_d_2 = idx_h_d*frequency/grid_size;
                    // determine location vectors                  
                    Vec2d vec_top_left{idx_w_d_2 - grid_idx_w, idx_h_d_2 - grid_idx_h};
                    Vec2d vec_top_right{idx_w_d_2 - (grid_idx_w + 1), idx_h_d_2 - grid_idx_h};
                    Vec2d vec_bottom_left{idx_w_d_2 - grid_idx_w, idx_h_d_2 - (grid_idx_h + 1)};
                    Vec2d vec_bottom_right{idx_w_d_2 - (grid_idx_w + 1), idx_h_d_2 - (grid_idx_h + 1)};
                    // determine dot products
                    double dot_left_top = dotProduct(vec_top_left, (*(*(*mat)[octave])[grid_idx_h])[grid_idx_w]);
                    double dot_right_top = dotProduct(vec_top_right, (*(*(*mat)[octave])[grid_idx_h])[grid_idx_w + 1]);
                    double dot_left_bottom = dotProduct(vec_bottom_left, (*(*(*mat)[octave])[grid_idx_h + 1])[grid_idx_w]);
                    double dot_right_bottom = dotProduct(vec_bottom_right, (*(*(*mat)[octave])[grid_idx_h + 1])[grid_idx_w + 1]);                    
                    // calculate interpolation                    
                    double inter_w_1 = interpolationCubic(dot_left_top, dot_right_top, weigth_w);
                    double inter_w_2 = interpolationCubic(dot_left_bottom, dot_right_bottom, weigth_w);
                    double inter_h = interpolationCubic(inter_w_1, inter_w_2, weigth_h);
                    // octave result                             
                    inter_h *= amplitude;
                    pixel.value += inter_h;

                    frequency *= 2;
                    amplitude /= 8;
                }                          
                pixel.value *= contrast;
                (*output_2d)[idx_h*output_w + idx_w] = pixel;                
                if ((idx_w + 1) % grid_size == 0) grid_idx_w += 1; 
            }
            if ((idx_h + 1) % grid_size == 0) grid_idx_h += 1;
        }

        return output_2d;
    }
  

    std::unique_ptr<pn_1d_t> calcPNoise1D(size_t number_of_elem, size_t octaves_count, double increase_amp) {
        auto output_1d = std::make_unique<pn_1d_t>(number_of_elem);        
        std::unique_ptr<std::vector<double>> seed = getRandomVec1D(number_of_elem, 0.0, RAND_MAX_PN);

        for (size_t idx_val = 0, idx_w = 0; idx_val < number_of_elem; idx_val++, idx_w += 10) {            
            double amp_norm = 1.0;
            double amplitude = 1.0;
            size_t frequency = 1;            
            double val = 0.0;
            for (size_t octave = 0; octave < octaves_count; octave++) {
                size_t octave_step_1 = (idx_w*frequency) % number_of_elem;
                size_t octave_step_2 = (idx_w*3*frequency) % number_of_elem;
                val += interpolationCubic((*seed)[octave_step_1], (*seed)[octave_step_2], 0.5) * amplitude;
                amplitude /= 2 * increase_amp;
                amp_norm += amplitude;
                frequency *= 2.5;
            }
            val /= amp_norm;
            (*output_1d)[idx_val] = {idx_w, val}; 
        }

        return output_1d;
    }

    std::unique_ptr<pn_crack> calcPNoiseCrack(size_t number_of_elem, size_t octaves_count, double increase_amp) {
        auto output_crack = std::make_unique<pn_crack>();        
        std::unique_ptr<std::vector<double>> seed_y = getRandomVec1D(number_of_elem, 0.0, RAND_MAX_PN);
        //std::unique_ptr<std::vector<double>> seed_height = getRandomVec1D(number_of_elem, 0.0, RAND_MAX_PN);
        double val_height = getRandomNumber(0.0, 1.0);        

        for (size_t idx_val = 0, idx_w = 0; idx_val < number_of_elem; idx_val++, idx_w += 10) {            
            double amp_norm = 1.0;
            double amplitude = 1.0;
            size_t frequency = 1;            
            double val_y = 0.0;
            //double val_height = 0.0;
            for (size_t octave = 0; octave < octaves_count; octave++) {
                size_t octave_step_1 = (idx_w*frequency) % number_of_elem;
                size_t octave_step_2 = (idx_w*3*frequency) % number_of_elem;
                val_y += interpolationCubic((*seed_y)[octave_step_1], (*seed_y)[octave_step_2], 0.5) * amplitude;
                //val_height += interpolationCubic((*seed_height)[octave_step_1], (*seed_height)[octave_step_2], 0.5) * amplitude;
                amplitude /= (2 * increase_amp);
                amp_norm += amplitude;
                frequency *= 2.5;
            }
            //val_height /= amp_norm;
            val_y /= amp_norm;
            output_crack->push_back({idx_w, val_y, val_height});
        }

        return output_crack;
    }

    std::unique_ptr<pn_vec_t> calcPNoiseCircle(size_t grid_size, size_t output_h, size_t output_w, size_t circle_samples, size_t octaves_count, double contrast) {
        constexpr double FULL_CIRCLE_RAD = 2*M_PI;
        const size_t x_center = static_cast<size_t>(output_w/2);
        const size_t y_center = static_cast<size_t>(output_h/2);
        std::unique_ptr<pn_2d_t> pn_2d = calcPNoise2DRandom(grid_size, output_h, output_w, octaves_count, contrast);
        size_t radius_max = (output_w < output_h ? output_w : output_h);
        size_t radius_pn = getRandomNumber(radius_max/2, radius_max) / 2;
        
        std::unique_ptr<pn_vec_t> output_circle = std::make_unique<pn_vec_t>();
        for(size_t idx = 1; idx <= circle_samples; idx++) {
            // determine coordinates by radiant
            double rad = FULL_CIRCLE_RAD/circle_samples*idx;
            double x_rad = cos(rad);
            double y_rad = sin(rad);
            // determine circle radius from 2d perlin noise
            size_t x_pn = static_cast<size_t>(std::round( x_rad*radius_pn + x_center ));
            size_t y_pn = static_cast<size_t>(std::round( y_rad*radius_pn + y_center ));
            auto radius_circle = (*pn_2d)[y_pn*output_w + x_pn].value;
            // set the x and y values for the noise circle
            if (radius_circle > 1.0) radius_circle = 1.0;
            else if (radius_circle < -1.0) radius_circle = -1.0;
            radius_circle = (radius_circle + 1)/2;  // adjust to positive value
            output_circle->insert({idx, {x_rad*radius_circle, y_rad*radius_circle}});
        }   

        return output_circle;
    }

} 

