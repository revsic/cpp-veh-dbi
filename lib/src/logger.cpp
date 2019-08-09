#include <asm_support.hpp>
#include <logger.hpp>
#include <utils.hpp>

// Text section based constructor. 
Logger::Logger(std::string filename, bool only_api) :
    filename(filename), output(filename), only_api(only_api)
{
    auto[text_start, text_end] = Utils::GetTextSectionAddress();
    start = text_start;
    end = text_end;
}

// Constructor.
Logger::Logger(size_t start, size_t end, std::string filename, bool only_api) : 
    start(start), end(end), filename(filename), output(filename), only_api(only_api)
{
    // Do nothing
}

// Callback.
void Logger::run(BTInfo const& info, PCONTEXT context) {
    // if callback is invoked in non-branching context.
    if (!info.e8_branch && !info.ff_branch) {
        return;
    }

    void* src_ptr = reinterpret_cast<void*>(info.source);
    void* called_ptr = reinterpret_cast<void*>(info.called);

    // if api call
    if (!(start <= info.source && info.source <= end)) {
        auto[load_module, module_name] = Utils::GetModuleNameByAddr(info.called);
        // if module is loaded
        if (load_module) {
            std::string symbol_name = Utils::GetSymbolName(info.called);
            output << '+' << src_ptr << ',' << called_ptr << ',' << module_name << ',' << symbol_name << std::endl;
        } else {
            output << '+' << src_ptr << ',' << called_ptr << ",," << std::endl;
        }
    } else if (!only_api) {
        // for inner call
        output << '+' << src_ptr << ',' << called_ptr << ",," << std::endl;
    }
}
