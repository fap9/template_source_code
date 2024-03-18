#include <iostream>
#include "template_generating.h"
#include "template_utils.h"
#include "interpolation.h"


namespace tplgen {
    void TemplateGenerating::saveTemplate(double amp, OFFSize size) {
        std::string file_path_pgm; 
        std::string file_path_off;
        
#ifdef __linux__
        file_path_pgm = _file_path + "/" + _file_name_prefix + "_" + std::to_string(_number) + ".pgm";
        file_path_off = _file_path + "/" + _file_name_prefix + "_" + std::to_string(_number) + ".off";
        _vis.saveImgPGMAscii(file_path_pgm.c_str(), _tepl.get(), _output_w, _output_h);
        _vis.saveTeplOff(file_path_off.c_str(), _tepl.get(), _output_w, _output_h, amp, size);
#elif _WIN32
        file_path_pgm = _file_path + "\\" + _file_name_prefix + "_" + std::to_string(_number) + ".pgm";
        file_path_off = _file_path + "\\" + _file_name_prefix + "_" + std::to_string(_number) + ".off";
        _vis.saveImgPGMAscii(file_path_pgm.c_str(), _tepl.get(), _output_w, _output_h);
        _vis.saveTeplOff(file_path_off.c_str(), _tepl.get(), _output_w, _output_h, amp, size);
#else
        std::cout << printError("Enter a file path for the Templates", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
#endif
        _number++;
    }

     TemplateGenerating::TemplateGenerating(size_t output_w, size_t output_h, const std::string &file_path)
        : _output_w(output_w), 
        _output_h(output_h),
        _file_path(file_path)
     { 
        createDirectory(_file_path);
     }

}

