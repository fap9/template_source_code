#include <iostream>
#include <fstream>
#include "template_visualizing.h"
#include "template_utils.h"


namespace tplgen {

    void PNVisualizing::axisLabeling(sf::RenderWindow &window, size_t x_max, int64_t y_max) {
        // font
        sf::Font font;
        if (!font.loadFromFile(FONT_PATH)) {  // FONT_PATH is defined in the "CMakeLists.txt"
            printError("Could not load font.", __FILE__, __LINE__);
            return;
        }
        // x-axis
        for (size_t i = 1; i <= x_max; ++i) {
            if (i % (10*_scale) == 0) {
                sf::Text text(std::to_string(i/_scale), font, 12);
                text.setPosition(i, 0);
                text.setScale(3 , 1);
                text.setFillColor(sf::Color::Black);
                window.draw(text);    
            }            
        }
        // y-axis
        sf::Text axis_zero("0", font, 12);
        axis_zero.setPosition(-1*_scale, 0);
        axis_zero.setScale(3 , 1);
        axis_zero.setFillColor(sf::Color::Black);
        window.draw(axis_zero);    
        int64_t step = (y_max < 5*_scale) ? y_max : 5*_scale;
        for (int64_t idx = step; idx <= y_max; idx += step) {
            sf::Text text_neg(std::to_string(-idx/_scale), font, 12);
            sf::Text text_pos(std::to_string(idx/_scale), font, 12);
            text_neg.setPosition(-1*_scale, idx);
            text_pos.setPosition(-1*_scale, -idx);
            text_neg.setScale(3 , 1);
            text_pos.setScale(3 , 1);
            text_neg.setFillColor(sf::Color::Black);
            text_pos.setFillColor(sf::Color::Black);
            window.draw(text_neg);    
            window.draw(text_pos);    
        }
    }

    bool PNVisualizing::saveImgPGMBinary(const char* save_path, const pn_2d_t* output_2d, size_t output_w, size_t output_h) {
        std::ofstream out_file(save_path, std::ios::binary);
        if (!out_file.is_open()) {
            printError("Can't open file \"" + std::string(save_path) + "\" ", __FILE__, __LINE__);
            return false;
        }
        // PGM-Image header
        out_file << "P5\n";  // PGM identifier for binary=5
        out_file << output_w << " " << output_h << "\n";  // width and height of the image
        out_file << "255" << std::endl;  // max intensity
        // Write pixel values (grayscale)
        for (size_t h = 0; h < output_h; ++h) {
            for (size_t w = 0; w < output_w; ++w) {
                double pixel_val = (*output_2d)[w + h*output_w].value;
                if (pixel_val > 1.0) pixel_val = 1.0;
                else if (pixel_val < -1.0) pixel_val = -1.0;
                pixel_val = (pixel_val + 1.0)*0.5*255;           

                unsigned char intensity = static_cast<unsigned char>(pixel_val);
                out_file.write(reinterpret_cast<const char*>(&intensity), sizeof(unsigned char));
            }
        }
        out_file.close();
        return true;
    }

    bool PNVisualizing::saveImgPGMAscii(const char* save_path, const pn_2d_t* output_2d, size_t output_w, size_t output_h) {
        std::ofstream out_file(save_path, std::ios::binary);
        if (!out_file.is_open()) {
            printError("Can't open file \"" + std::string(save_path) + "\" ", __FILE__, __LINE__);
            return false;
        }
        // PGM-Image header
        out_file << "P2\n";  // PGM identifier for ascii=2
        out_file << output_w << " " << output_h << "\n";  // width and height of the image
        out_file << "255" << std::endl;  // max intensity
        // Write pixel values (grayscale)
        for (size_t h = 0; h < output_h; ++h) {
            for (size_t w = 0; w < output_w; ++w) {
                double pixel_val = (*output_2d)[w + h*output_w].value;
                if (pixel_val > 1.0) pixel_val = 1.0;
                else if (pixel_val < -1.0) pixel_val = -1.0;

                int intensity = static_cast<int>((pixel_val + 1.0)*0.5*255);           

                out_file << intensity << " ";
                out_file.flush();
            }
            out_file << std::endl;
        }
        out_file.close();
        return true;
    }

    bool PNVisualizing::saveTeplOff(const char* save_path, const pn_2d_t* output_2d, size_t output_w, size_t output_h, double amp, OFFSize size) {
        std::string mesh;
        std::ofstream out_file(save_path);
        if (!out_file.is_open()) {
            printError("Can't open file \"" + std::string(save_path) + "\" ", __FILE__, __LINE__);
            return false;
        }
        if (size == 0) {
            printError("Wrong size ", __FILE__, __LINE__);
            return false;
        }

        // OFF-File header
        out_file << "OFF\n";
        out_file << output_w*output_h << " " << (output_w - 1)*(output_h - 1)*2 << " 0\n";
        for (double h = 0; h < output_h; ++h) {
            for (double w = 0; w < output_w; ++w) {
                size_t pos = static_cast<size_t>(h*output_w + w);
                // point coordinates
                out_file << w/size << " " << h/size << " " << (*output_2d)[pos].value*amp/size << std::endl;  

                // triangle meshes
                if ((h < output_h - 1) && (w < output_w - 1)) {
                    mesh += "3 " + std::to_string(pos) + " " + std::to_string(pos + 1) +  
                        " " + std::to_string(pos + output_w) + "\n";
                    mesh += "3 " + std::to_string(pos + 1) + " " + std::to_string(pos + output_w) +  
                        " " + std::to_string(pos + output_w + 1) + "\n";
                }
            }
        }

        out_file << mesh << std::flush;
        out_file.close();
        return true;
    }

    void PNVisualizing::plotOutput(const pn_1d_t* output_1d, const char* label, int64_t scale) {
        _scale = scale;
        const float boundary = 5;
        const float x_start = (output_1d->front().x - boundary)*_scale;
        const float x_end = (output_1d->back().x + 2*boundary)*_scale;
        const float y_max = 2*_scale;
        // setup window
        sf::RenderWindow window(sf::VideoMode(800, 600), label);  
        window.setView(sf::View(sf::FloatRect(x_start, -1.5*y_max, x_end, 3*y_max)));
        // plot loop
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }
            window.clear(sf::Color::White);
            // draw x-axis
            sf::VertexArray x_axis(sf::Lines, 2);
            x_axis[0].position = sf::Vector2f(x_start + _scale*boundary, 0);
            x_axis[0].color = sf::Color::Black;
            x_axis[1].position = sf::Vector2f(x_end - _scale*2*boundary, 0);
            x_axis[1].color = sf::Color::Black;
            window.draw(x_axis);
            // draw y-axis
            sf::VertexArray y_axis(sf::Lines, 2);
            y_axis[0].position = sf::Vector2f(0, -y_max);
            y_axis[0].color = sf::Color::Black;
            y_axis[1].position = sf::Vector2f(0, y_max);
            y_axis[1].color = sf::Color::Black;
            window.draw(y_axis);
            axisLabeling(window, x_end, y_max);
            // drawing the function
            sf::VertexArray plot(sf::LineStrip, output_1d->size());
            for (size_t x = 0; x < output_1d->size(); x++) {
                plot[x].position = sf::Vector2f(static_cast<double>((*output_1d)[x].x)*_scale, -(*output_1d)[x].y*_scale);
                plot[x].color = sf::Color::Blue;
            }            
            window.draw(plot);
            window.display();
        }
    }       


    bool PNVisualizing::showImg(const pn_2d_t* output_2d, size_t output_w, size_t output_h, size_t scale) {
        sf::RenderWindow window(sf::VideoMode(output_w*scale, output_h*scale), "Perlin Noise 2D");
#ifdef __linux__
        constexpr const char* PATH_IMG_SHOW = "/tmp/pn_2d_show.pgm";
#elif _WIN32
        constexpr const char* PATH_IMG_SHOW = "C:\\misc\\pn_2d_show.pgm";
#else
        std::cout << printError("Enter a file path for the Templates", __FILE__, __LINE__);
        exit(-1);
#endif

        // create Image on disk
        if (!saveImgPGMBinary(PATH_IMG_SHOW, output_2d, output_w, output_h)) {
            return false;
        }
        // load image from file
        sf::Image image;
        if (!image.loadFromFile(PATH_IMG_SHOW)) {
            printError("Failed to load image", __FILE__, __LINE__);           
            return false;
        }
        // load texture from file and create sprite
        sf::Texture texture;
        texture.loadFromImage(image);
        sf::Sprite sprite(texture);
        sprite.setScale(scale, scale);
        // show window
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear(); 
            window.draw(sprite); 
            window.display();  // show result
        }

        return true;
    }

}