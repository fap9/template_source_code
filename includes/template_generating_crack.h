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


namespace tplgen {
    class TemplateGeneratingCrack: public TemplateGenerating {
    private:
        std::set<uint64_t> _tepl_shape;
        Vec2d _rot_point = {0.0, 0.0};

        void positionInstantiation(size_t len, size_t heigth_max, size_t amp, size_t width, double angle_start, double angle_end);
        void translateRotatePoint(Vec2d &crack_point);

        void createCrack(size_t grid_size);
        void crackIntoSurface(pn_crack &crack);
        void intoSurface(const Vec2d& current_point, const Vec2d& last_point, double value = 1);

        double scaleValue(double value);
        

    public:
        TemplateGeneratingCrack(size_t output_w, size_t output_h, const std::string &file_path);
        void generateTemplates(size_t count_tepls) override;

    };
}
