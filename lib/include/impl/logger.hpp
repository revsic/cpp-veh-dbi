#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>

#include "branch_tracer.hpp"

// Logger for branch tracer.
struct Logger : BTCallback {
    // Text section based constructor. 
    Logger(std::string filename, bool only_api = false);
    // Constructor.
    Logger(size_t start, size_t end, std::string filename, bool only_api = false);

    // Callback.
    void run(BTInfo const& info, PCONTEXT context) override;

private:
    size_t start;
    size_t end;
    
    std::string filename;
    std::ofstream output;

    bool only_api;
};

#endif