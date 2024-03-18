#include <iostream>
#include "template_generating_crack.h"
#include "template_utils.h"
#include "interpolation.h"


namespace tplgen {

    TemplateGeneratingCrack::TemplateGeneratingCrack(size_t output_w, size_t output_h, const std::string &file_path)
        : TemplateGenerating(output_w, output_h, file_path)
    { 
        _file_path += "/cracks";
        _file_name_prefix += "crack";
        createDirectory(_file_path);
    }


    void TemplateGeneratingCrack::positionInstantiation(size_t len, size_t heigth_max, size_t amp, size_t width, double angle_start, double angle_end) {    
        angle_start -= M_PI/2;
        angle_end -= M_PI/2;
        _tepl_inst.length = getRandomNumber(len/2, len);
        _tepl_inst.x_trsl = getRandomNumber(1, _output_w-1);
        _tepl_inst.y_trsl = getRandomNumber(1, _output_h-1);
        _tepl_inst.amp = getRandomNumber(amp/2, amp);
        _tepl_inst.width = getRandomNumber(2, width);
        _tepl_inst.rotation = getRandomNumber(angle_start, angle_end);
        _tepl_inst.height = getRandomNumber(heigth_max/2, heigth_max);
    }

    double TemplateGeneratingCrack::scaleValue(double value) {
        value = value < -1.0 ? -1.0 : value;
        value = value > 1.0 ? 1.0 : value;
        value = (value + 1.0)/2.0*255.0;

        return value;
    }

    void TemplateGeneratingCrack::translateRotatePoint(Vec2d &crack_point) {
        // rotation around the first point of the curve
        crack_point.x -= _rot_point.x;
        crack_point.y *= _tepl_inst.amp;
        crack_point.y -= _rot_point.y;
        double rot_x = crack_point.x*cos(_tepl_inst.rotation) - crack_point.y*sin(_tepl_inst.rotation);
        double rot_y = crack_point.x*sin(_tepl_inst.rotation) + crack_point.y*cos(_tepl_inst.rotation);
        // translation back to original coordinate and translation
        crack_point.x = rot_x + _rot_point.x + _tepl_inst.x_trsl;
        crack_point.y = std::floor(rot_y + _rot_point.y + _tepl_inst.y_trsl);
    }

    void TemplateGeneratingCrack::intoSurface(const Vec2d& current_point, const Vec2d& last_point, double value) {
        // insert curve into the surface
        std::unique_ptr<std::vector<Vec2d>> interpolation = calcLinearizationPoints(current_point, last_point);
        for (auto &int_point : *interpolation) {
            size_t curve_x = static_cast<size_t>(int_point.x);
            size_t curve_y = static_cast<size_t>(int_point.y);
            // insert point into the surface
            if (curve_x >= _output_w || curve_y < 0.0 || curve_y >= _output_h) continue;  // out of bounds
            (*_tepl)[static_cast<size_t>(curve_y*_output_w + curve_x)].value = value; 
            for (size_t width = 1; width < _tepl_inst.width; width++) {
                if (curve_x + width >= _output_w || curve_y < 0.0 || curve_y >= _output_h) continue;  // out of bounds
                (*_tepl)[static_cast<size_t>(curve_y*_output_w + curve_x + width)].value = value; 
                (*_tepl)[static_cast<size_t>(curve_y*_output_w + curve_x - width)].value = value; 
            }
        }
    }


    void TemplateGeneratingCrack::crackIntoSurface(pn_crack &crack) {
        Vec2d last_point = {static_cast<double>(crack[0].x), crack[0].y};
        translateRotatePoint(last_point);
        for (size_t idx = 1; idx < crack.size(); idx++) {
            Vec2d current_point = {static_cast<double>(crack[idx].x), crack[idx].y};
            translateRotatePoint(current_point);
            intoSurface(current_point, last_point, -crack[idx].height);

            last_point = current_point; 
        }
    }

    void TemplateGeneratingCrack::createCrack(size_t grid_size) {
        constexpr size_t WIDTH = 5;
        constexpr double OFFSET_ROT = M_PI/8;
        size_t amp = grid_size/20;
        size_t height = grid_size/10;
        size_t len = _output_w/10;
        std::unique_ptr<pn_crack> crack;

        // create cracks
        size_t count_main_crack = getRandomNumber(10ul, 15ul);
        double angle_rot = getRandomNumber(0.0, M_PI - OFFSET_ROT);
        for (size_t idx_crack = 0; idx_crack < count_main_crack; idx_crack++) {
            positionInstantiation(len, height, amp, WIDTH, angle_rot, angle_rot + OFFSET_ROT);
            crack = calcPNoiseCrack(_tepl_inst.length, 4, 4);
            _rot_point = {static_cast<double>((*crack)[0].x), (*crack)[0].y};
            crackIntoSurface(*crack.get());
        }

    }


    void TemplateGeneratingCrack::generateTemplates(size_t count_tepls) {
        constexpr size_t OCTAVES = 8;
        size_t len = _output_h < _output_w ? _output_h/2 : _output_w/2;
        const size_t grid_size = len;

        for (size_t idx = 0; idx < count_tepls; idx++) {
            std::unique_ptr<pn_2d_t> pn_2d = calcPNoise2DRandom(grid_size, _output_h, _output_w, OCTAVES, 1.0);
            //_tepl = _pn->getPNoise2D();  
            _tepl = getTemplateBackground(_output_w, _output_h, 1.0);
            createCrack(grid_size);
            saveTemplate(_tepl_inst.height, OFFSize::METER);
        }
    }
}