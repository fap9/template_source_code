
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
    struct LocalExtremumChecking {
        bool above = false;
        bool below = false;
    };


    class TemplateGeneratingDent: public TemplateGenerating {
    private:
        std::set<uint64_t> _tepl_shape;

        void positionInstantiation(size_t output_w_max, size_t output_h_max, size_t len_max, size_t heigth_max, double angle_max = 2*M_PI);

        std::pair<int64_t, int64_t> coordFillingToShapeCenter(size_t x, size_t y);
        std::pair<size_t, size_t> coordOriginToShapeCenter(const Vec2d &shape_coord);
        void createDent(pn_vec_t &shape_samples);
        std::unique_ptr<pn_vec_t> createDentShape();
        std::optional<double> getSlopeHeight(size_t idx_x, size_t idx_y, size_t circle_filling_dim, pn_2d_t &circle_filling, pn_vec_t &circle_radius);
        void setGeometryDentShape(Vec2d &circle_point);
        double indicesInterpolation(double idx_rad, size_t circle_filling_dim, pn_2d_t &circle_filling, pn_vec_t &circle_radius);
        LocalExtremumChecking checkPixelAboveBelow(std::set<uint64_t> &hole_shape, size_t idx_y, size_t idx_x, size_t x_pos);
        double getDentValue(double value);

    public:
        TemplateGeneratingDent(size_t output_w, size_t output_h, const std::string &file_path);
        void generateTemplates(size_t count_tepls) override;

    };
}
