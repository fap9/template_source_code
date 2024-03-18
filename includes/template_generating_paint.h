#pragma once

#include <memory>
#include <cmath>
#include <set>
#include <optional>
#include <utility>
#include "template_visualizing.h"
#include "template_types.h"
#include "perlin_noise.h"
#include "template_generating.h"
#include "template_utils.h"


namespace tplgen {
    class TemplateGeneratingPaint: public TemplateGenerating {
    private:
        TemplateInstantiation _tepl_inst;

        void positionInstantiation(size_t output_w_max, size_t output_h_max, size_t height);
        
        void createPaintDefect(size_t grid_size);

    public:
        TemplateGeneratingPaint(size_t output_w, size_t output_h, const std::string &file_path);
        void generateTemplates(size_t count_tepls) override;

    };
}
