#include <iostream>
#include <unistd.h>
#include <sys/wait.h> 
#include "template_utils.h"
#include "template_generating_dent.h"
#include "template_generating_crack.h"
#include "template_generating_paint.h"



int main () {
    std::cout << "Programm started\n" << std::endl;
    constexpr size_t NUMBER_OF_TEMPLATES = 5;

#ifdef __linux__
    std::string root_path = "/tmp/templates";

    pid_t pid_dents = tplgen::createFork();
    if (pid_dents == 0) {
        tplgen::TemplateGeneratingDent tg_dent = tplgen::TemplateGeneratingDent(256, 256, root_path);
        tg_dent.generateTemplates(NUMBER_OF_TEMPLATES);    
        std::cout << "[INFO] Dent templates (" << NUMBER_OF_TEMPLATES << ") generated." << std::endl;
        exit(EXIT_SUCCESS);
    }

    pid_t pid_cracks = tplgen::createFork();
    if (pid_cracks == 0) {
        tplgen::TemplateGeneratingCrack tg_crack = tplgen::TemplateGeneratingCrack(256, 256, root_path);
        tg_crack.generateTemplates(NUMBER_OF_TEMPLATES);
        std::cout << "[INFO] Crack templates (" << NUMBER_OF_TEMPLATES << ") generated." << std::endl;
        exit(EXIT_SUCCESS);
    }

    pid_t pid_paints = tplgen::createFork();
    if (pid_paints == 0) {
        tplgen::TemplateGeneratingPaint tg_paint = tplgen::TemplateGeneratingPaint(256, 256, root_path);
        tg_paint.generateTemplates(NUMBER_OF_TEMPLATES);  
        std::cout << "[INFO] Paint templates (" << NUMBER_OF_TEMPLATES << ") generated." << std::endl;
        exit(EXIT_SUCCESS);
    }
    // Wait for all three forks to finish
    int status;
    waitpid(pid_dents, &status, 0);
    waitpid(pid_cracks, &status, 0);
    waitpid(pid_paints, &status, 0);


#elif _WIN32
    std::string root_path = "C:\\misc\\templates";


    tplgen::TemplateGeneratingDent tg_dent = tplgen::TemplateGeneratingDent(256, 256, root_path);
    tg_dent.generateTemplates(NUMBER_OF_TEMPLATES);  
    std::cout << "[INFO] Dent templates (" << NUMBER_OF_TEMPLATES << ") generated." << std::endl;

    tplgen::TemplateGeneratingCrack tg_crack = tplgen::TemplateGeneratingCrack(256, 256, root_path);
    tg_crack.generateTemplates(NUMBER_OF_TEMPLATES);
    std::cout << "[INFO] Crack templates (" << NUMBER_OF_TEMPLATES << ") generated." << std::endl;

    tplgen::TemplateGeneratingPaint tg_paint = tplgen::TemplateGeneratingPaint(256, 256, root_path);
    tg_paint.generateTemplates(NUMBER_OF_TEMPLATES);  
    std::cout << "[INFO] Paint templates (" << NUMBER_OF_TEMPLATES << ") generated." << std::endl;


#else
    tplgen::printError("Unsupported platform", __FILE__, __LINE__); 
    return EXIT_FAILURE;

#endif

    std::cout << "\nProgramm finished" << std::endl;
    return EXIT_SUCCESS;
}

