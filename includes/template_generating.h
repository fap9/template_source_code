#pragma once

#include <memory>
#include <cmath>
#include <set>
#include <optional>
#include <utility>
#include "template_visualizing.h"
#include "template_types.h"
#include "perlin_noise.h"


namespace tplgen {
    struct TemplateInstantiation {
        size_t x_trsl, y_trsl;
        size_t length;  
        size_t amp;
        size_t width;
        size_t height;
        double rotation;
    };

    class TemplateGenerating {
    protected:
        size_t _number = 1;
        size_t _output_w;
        size_t _output_h;
        std::string _file_path;
        std::string _file_name_prefix = "tepl_";
        PNVisualizing _vis;
        TemplateInstantiation _tepl_inst;
        std::unique_ptr<pn_2d_t> _tepl;

        void saveTemplate(double amp = 1.0, OFFSize size = OFFSize::METER);

    public:
        TemplateGenerating(size_t output_w, size_t output_h, const std::string &file_path);
        virtual void generateTemplates(size_t count_tepls) = 0;
        virtual ~TemplateGenerating() = default;

    };
}
