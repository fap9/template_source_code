#include <iostream>
#include "template_generating_paint.h"
#include "template_utils.h"


namespace tplgen {
    
    TemplateGeneratingPaint::TemplateGeneratingPaint(size_t output_w, size_t output_h, const std::string &file_path)
        : TemplateGenerating(output_w, output_h, file_path)
    { 
        _file_path += "/paints";
        _file_name_prefix += "paint";
        createDirectory(_file_path);
    }

    void TemplateGeneratingPaint::positionInstantiation(size_t output_w_max, size_t output_h_max, size_t height) {
        _tepl_inst.x_trsl = getRandomNumber(0, output_w_max);
        _tepl_inst.y_trsl = getRandomNumber(0, output_h_max);
        _tepl_inst.height = getRandomNumber(height/3, height/3*2);

    }

    void TemplateGeneratingPaint::createPaintDefect(size_t grid_size) {
        constexpr size_t OCTAVES = 1;
        std::unique_ptr<pn_2d_t> paint = calcPNoise2DEmphasis(grid_size, OCTAVES, 1.0);

        for (size_t idx_y = _tepl_inst.y_trsl; idx_y < _tepl_inst.y_trsl + grid_size; idx_y++) {
            for (size_t idx_x = _tepl_inst.x_trsl; idx_x < _tepl_inst.x_trsl + grid_size; idx_x++) {
                if (idx_x < _output_w && idx_y < _output_h) {
                    size_t pos_paint = idx_x - _tepl_inst.x_trsl + (idx_y - _tepl_inst.y_trsl)*grid_size;
                    _tepl->at(idx_x + idx_y*_output_w).value = paint->at(pos_paint).value;
                }
            }
        }

    }


    void TemplateGeneratingPaint::generateTemplates(size_t count_tepls) {
        //const size_t grid_size = _output_h < _output_w ? _output_h/2 : _output_w/2;
        const size_t grid_size = _output_h < _output_w ? _output_h : _output_w;
        const size_t height = grid_size/2;

        for (size_t idx = 0; idx < count_tepls; idx++) {
            positionInstantiation(0, 0, height);
            _tepl = getTemplateBackground(_output_w, _output_h, 0.0);
            createPaintDefect(grid_size);
            saveTemplate(_tepl_inst.height, OFFSize::DECIMETER);
        }
    }

}