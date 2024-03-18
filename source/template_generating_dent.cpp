#include <iostream>
#include "template_generating_dent.h"
#include "template_utils.h"
#include "interpolation.h"


namespace tplgen {

    TemplateGeneratingDent::TemplateGeneratingDent(size_t output_w, size_t output_h, const std::string &file_path)
        : TemplateGenerating(output_w, output_h, file_path)
    { 
        _file_path += "/dents";
        _file_name_prefix += "dent";
        createDirectory(_file_path);
    }

    void TemplateGeneratingDent::positionInstantiation(size_t output_w_max, size_t output_h_max, size_t len_max, size_t heigth_max, double angle_max) {
        _tepl_inst.width = getRandomNumber(len_max/10, len_max/5);
        _tepl_inst.length = getRandomNumber(len_max/2, len_max - _tepl_inst.width);
        if (_tepl_inst.length % 2 == 0) ++_tepl_inst.length;
        _tepl_inst.x_trsl = getRandomNumber(_tepl_inst.length + _tepl_inst.width, output_w_max - _tepl_inst.length - _tepl_inst.width);
        _tepl_inst.y_trsl = getRandomNumber(_tepl_inst.length + _tepl_inst.width, output_h_max - _tepl_inst.length - _tepl_inst.width);
        _tepl_inst.amp = 0;
        _tepl_inst.rotation = getRandomNumber(0.0, angle_max);
        _tepl_inst.height = getRandomNumber(heigth_max/2, heigth_max);
    }


    std::pair<int64_t, int64_t> TemplateGeneratingDent::coordFillingToShapeCenter(size_t x, size_t y) {
        int64_t y_shape_center, x_shape_center;
        x_shape_center = static_cast<int64_t>(x) - _tepl_inst.length;
        if (y < _tepl_inst.length) y_shape_center = _tepl_inst.length - static_cast<int64_t>(y);
        else y_shape_center = -static_cast<int64_t>(y - _tepl_inst.length);

        return std::make_pair(x_shape_center, y_shape_center);
    }

    std::pair<size_t, size_t> TemplateGeneratingDent::coordOriginToShapeCenter(const Vec2d &shape_coord) {
        size_t y_shape_center, x_shape_center;
        x_shape_center = static_cast<size_t>(std::round(shape_coord.x - (_tepl_inst.x_trsl - _tepl_inst.length)));
        y_shape_center = static_cast<size_t>(std::round(shape_coord.y - (_tepl_inst.y_trsl - _tepl_inst.length)));

        return std::make_pair(x_shape_center, y_shape_center);
    }

    void TemplateGeneratingDent::setGeometryDentShape(Vec2d &circle_point) {    
        // set random diameter
        circle_point.x *= _tepl_inst.length;
        circle_point.y *= _tepl_inst.length;
        // set random translation
        circle_point.x += _tepl_inst.x_trsl;
        circle_point.y = -circle_point.y + _tepl_inst.y_trsl;
    }

    double TemplateGeneratingDent::indicesInterpolation(double idx_rad, size_t circle_filling_dim, pn_2d_t &circle_filling, pn_vec_t &circle_radius) {
        size_t idx_rad_next = static_cast<size_t>(std::ceil(idx_rad));
        if (idx_rad_next <= 0 || idx_rad_next > circle_radius.size()) idx_rad_next = circle_radius.size();
        size_t idx_rad_before = static_cast<size_t>(std::floor(idx_rad));
        if (idx_rad_before <= 0 || idx_rad_before > circle_radius.size()) idx_rad_before = circle_radius.size();
        // Determining height by interpolating between indexes
        const Vec2d &shape_coord_1 = circle_radius.find(idx_rad_next)->second;
        size_t shape_x_1 = static_cast<size_t>(std::round(shape_coord_1.x - (_tepl_inst.x_trsl - _tepl_inst.length)));
        size_t shape_y_1 = static_cast<size_t>(std::round(shape_coord_1.y - (_tepl_inst.y_trsl - _tepl_inst.length)));
        double shape_height_1 = circle_filling[shape_y_1*circle_filling_dim + shape_x_1].value;
        const Vec2d &shape_coord_2 = circle_radius.find(idx_rad_before)->second;
        size_t shape_x_2 = static_cast<size_t>(std::round(shape_coord_2.x - (_tepl_inst.x_trsl - _tepl_inst.length)));
        size_t shape_y_2 = static_cast<size_t>(std::round(shape_coord_2.y - (_tepl_inst.y_trsl - _tepl_inst.length)));
        double shape_height_2 = circle_filling[shape_y_2*circle_filling_dim + shape_x_2].value;

        return quadraticInterpolation(shape_height_1, shape_height_2, std::abs(idx_rad - std::floor(idx_rad)));
    }

    std::optional<double> TemplateGeneratingDent::getSlopeHeight(size_t idx_x, size_t idx_y, size_t circle_filling_dim, pn_2d_t &circle_filling, pn_vec_t &circle_radius) {
        constexpr uint64_t MSB_MASK = 1ull << 63;  // to avoid errors when converting numerical space
        // obtaining the radian with respect to the template center
        if ((idx_x & MSB_MASK) != 0 || (idx_y & MSB_MASK) != 0) return std::nullopt;
        std::pair<int64_t, int64_t> rad_coord = coordFillingToShapeCenter(idx_x, idx_y);  // coordinate with respect to the shape center
        double outside_rad = std::atan2(rad_coord.second, rad_coord.first);
        if (outside_rad < 0) outside_rad += 2*M_PI;
        // calculate the shape radius index to get height at that position
        double idx_rad = outside_rad/(2*M_PI)*circle_radius.size();
        if (idx_rad <= 0 || idx_rad > circle_radius.size()) idx_rad = circle_radius.size();
        double shape_height = indicesInterpolation(idx_rad, circle_filling_dim, circle_filling, circle_radius);
        shape_height = getDentValue(shape_height);
        // obtaining coordinates with respect to shape center
        size_t idx_sample = static_cast<size_t>(std::round(idx_rad));
        if (idx_sample <= 0 || idx_sample > circle_radius.size()) idx_sample = circle_radius.size();
        const Vec2d &shape_coor = circle_radius.find(idx_sample)->second;
        std::pair<size_t, size_t> shape_center = coordOriginToShapeCenter(shape_coor);
        // determing distance from the current point to shape border
        if ((_tepl_inst.length & MSB_MASK) != 0 || (shape_center.first & MSB_MASK) != 0 || (shape_center.second & MSB_MASK) != 0) return std::nullopt;
        int64_t shape_x_center = shape_center.first - _tepl_inst.length;
        int64_t shape_y_center;
        if (shape_center.second < _tepl_inst.length) shape_y_center = _tepl_inst.length - shape_center.second;
        else shape_y_center = -static_cast<int64_t>(shape_center.second - _tepl_inst.length);
        double distance_slope = std::sqrt(std::pow(rad_coord.first - shape_x_center, 2) + std::pow(rad_coord.second - shape_y_center, 2));
        if (distance_slope < _tepl_inst.width) {
            size_t x_tepl = static_cast<size_t>(shape_coor.x + std::round(std::cos(outside_rad)*_tepl_inst.width));
            size_t y_tepl = static_cast<size_t>(shape_coor.y - std::round(std::sin(outside_rad)*_tepl_inst.width));
            try {
                double height_tepl = _tepl->at(y_tepl*_output_w + x_tepl).value;
                Vec2d start = {0.0, shape_height};
                Vec2d end = {static_cast<double>(_tepl_inst.width), height_tepl};
                return calcLinearizationPoint(start, end, distance_slope);
            } catch (const std::out_of_range& e) {
                return std::nullopt;
            }
            
        }
        
        return std::nullopt;
    }

    LocalExtremumChecking TemplateGeneratingDent::checkPixelAboveBelow(std::set<uint64_t> &hole_shape, size_t idx_y, size_t idx_x, size_t x_pos) {
        size_t pixel_below_coor = (idx_y + _tepl_inst.y_trsl - _tepl_inst.length + 1)*_output_w + 
            idx_x + _tepl_inst.x_trsl - _tepl_inst.length - x_pos;
        size_t pixel_above_coor = (idx_y + _tepl_inst.y_trsl - _tepl_inst.length - 1)*_output_w + 
            idx_x + _tepl_inst.x_trsl - _tepl_inst.length - x_pos;

        LocalExtremumChecking checking;
        checking.below = (hole_shape.find(pixel_below_coor) != hole_shape.end());
        checking.above = (hole_shape.find(pixel_above_coor) != hole_shape.end());

        return checking;
    }

    std::unique_ptr<pn_vec_t> TemplateGeneratingDent::createDentShape() {
        constexpr size_t CIRCLE_SAMPLES = 64;
        constexpr size_t OCTAVES = 4;
        constexpr size_t PN_SIZE = 64;
        constexpr double CIRCLE_LIKE_SHAPE_PROB = 1.0;  // probability for circle like shape (1.0 -> more like, 2 -> less like)
        std::unique_ptr<pn_vec_t> circle_radius = calcPNoiseCircle(PN_SIZE, PN_SIZE, PN_SIZE, CIRCLE_SAMPLES, OCTAVES, CIRCLE_LIKE_SHAPE_PROB);

        // calculate hole border
        Vec2d last_point = circle_radius->find(CIRCLE_SAMPLES)->second;
        setGeometryDentShape(last_point);
        for (size_t idx = 1; idx <= CIRCLE_SAMPLES; idx++) {
            auto &current_point = circle_radius->find(idx)->second;
            setGeometryDentShape(current_point);
            auto interpolation = calcLinearizationPoints(current_point, last_point);
            for (auto &int_point : *interpolation) {
                auto circle_x = static_cast<size_t>(int_point.x);
                auto circle_y = static_cast<size_t>(int_point.y);
                if (circle_x >= _output_w || circle_y < 0.0 || circle_y >= _output_h) continue; 
                _tepl_shape.insert(circle_y*_output_w + circle_x);
            }
            last_point = current_point;
        }
        return circle_radius;
    }

    double TemplateGeneratingDent::getDentValue(double value) {
        /* 
        if (value > 1.0) value = 1.0;
        else if (value < -1.0) value = -1.0;
        */
        //return value - 1.0;
        return value; 
    }

    void TemplateGeneratingDent::createDent(pn_vec_t &circle_radius) {
        constexpr size_t OCTAVES = 8;
        size_t shape_dim = (_tepl_inst.length + 1)*2;
        std::unique_ptr<pn_2d_t> circle_filling = calcPNoise2DRandom(shape_dim/4, shape_dim, shape_dim, OCTAVES, 1.5);
        
        // fill inner circle
        for (size_t idx_y = 0; idx_y < shape_dim; idx_y++) {
            size_t boundaries_crossed = 0;
            LocalExtremumChecking local_extreme_left;
            LocalExtremumChecking local_extreme_right;
            bool local_max_min = false;
            bool before_inside_template = false;
            bool last_pixel_was_border = false;

            for (size_t idx_x = 0; idx_x < shape_dim + 1; idx_x++) {
                uint64_t tepl_coor = (idx_y + _tepl_inst.y_trsl - _tepl_inst.length)*_output_w + 
                    idx_x + _tepl_inst.x_trsl - _tepl_inst.length;

                if (_tepl_shape.find(tepl_coor) != _tepl_shape.end()) {
                    if (!last_pixel_was_border) {
                        boundaries_crossed++;
                        local_extreme_left = checkPixelAboveBelow(_tepl_shape, idx_y, idx_x, 1);
                    }
                    local_max_min = false;
                    last_pixel_was_border = true;
                    double value = (*circle_filling)[idx_y*shape_dim + idx_x].value;
                    (*_tepl)[tepl_coor].value = getDentValue(value);
                } else { 
                    if (boundaries_crossed == 0) {  // before the first boundary (assuming starting outside the boundary)
                        std::optional<double> slope_height = getSlopeHeight(idx_x, idx_y, shape_dim, *circle_filling.get(), circle_radius); 
                        if (slope_height) (*_tepl)[tepl_coor].value = *slope_height;
                        continue;  
                    }
                    if (last_pixel_was_border) {  // check for local extremum
                        local_extreme_right = checkPixelAboveBelow(_tepl_shape, idx_y, idx_x, 0);
                        // local maximum
                        if (local_extreme_left.below && local_extreme_right.below && !local_extreme_right.above && !local_extreme_left.above) {
                            local_max_min = true;
                            boundaries_crossed--;
                        }
                        // local minimum
                        else if (local_extreme_left.above && local_extreme_right.above && !local_extreme_right.below && !local_extreme_left.below) {
                            local_max_min = true;
                            boundaries_crossed--;
                        }
                        last_pixel_was_border = false;

                        std::optional<double> slope_height = getSlopeHeight(idx_x, idx_y, shape_dim, *circle_filling.get(), circle_radius); 
                        if (slope_height) (*_tepl)[tepl_coor].value = *slope_height;
                    }

                    if ((!local_max_min) && (boundaries_crossed % 2 == 1)) {  // inside template
                        //(*_tepl)[tepl_coor].value = -1;  // testing
                        double value = (*circle_filling)[idx_y*shape_dim + idx_x].value;
                        (*_tepl)[tepl_coor].value = getDentValue(value);
                        before_inside_template = true;
                    }
                    else if ((!local_max_min) && (boundaries_crossed % 2 == 0)) {  // outside template
                        std::optional<double> slope_height = getSlopeHeight(idx_x, idx_y, shape_dim, *circle_filling.get(), circle_radius);
                        if (slope_height) (*_tepl)[tepl_coor].value = *slope_height;
                        //(*_tepl)[tepl_coor].value = 0;
                        before_inside_template = false;
                    }
                    else if (local_max_min && before_inside_template) {  // inside template if before lastborder inside       
                        //(*_tepl)[tepl_coor].value = -1;  // testing
                        double value = (*circle_filling)[idx_y*shape_dim + idx_x].value;
                        (*_tepl)[tepl_coor].value = getDentValue(value);
                        before_inside_template = true;     
                    }
                }
            }
        }   
    }


    void TemplateGeneratingDent::generateTemplates(size_t count_tepls) {
        const size_t grid_size = _output_h < _output_w ? _output_h/2 : _output_w/2;
        const size_t height = grid_size/10;
        const size_t len = grid_size;

        for (size_t idx = 0; idx < count_tepls; idx++) {
            positionInstantiation(_output_w, _output_h, len, height);
            //_tepl = calcPNoise2DRandom(grid_size, _output_h, _output_w, 8);
            _tepl = getTemplateBackground(_output_w, _output_h, 1.0);
            auto circle_radius = createDentShape();
            createDent(*(circle_radius.get()));
            saveTemplate(_tepl_inst.height, OFFSize::DECIMETER);
            _tepl_shape.clear();
        }
    }

}

