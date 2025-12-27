#pragma once
#include <iostream>
#include <source_location>
#include <string_view>

namespace ui {
    static inline std::ostream *_ostrm = &std::cout;
    static inline std::ostream *_estrm = &std::cerr;
    static inline bool g_hints_enabled = true;

    inline void set_out_stream(std::ostream &os) {
        _ostrm = &os;
    }

    inline void set_err_stream(std::ostream &os) {
        _estrm = &os;
    }

#ifdef DEBUG
    inline void print_error(std::string_view msg, const std::source_location& loc = std::source_location::current()) {
        *_estrm << "[ERROR] [" << loc.file_name() << ":" << loc.line()
                << " (" << loc.function_name() << ")] " << std::endl;
        *_estrm << "[ERROR] " << msg << std::endl;
        _estrm->flush();
    }
#else
    inline void print_error(std::string_view msg) {
        *_estrm << "[ERROR] " << msg << std::endl;
        _estrm->flush();
    }
#endif

    inline void print_output(std::string_view msg) {
        *_ostrm << "" << msg << std::endl;
    }

    inline void print_warning(std::string_view msg) {
        *_ostrm << "[WARNING] " << msg << std::endl;
    }

    inline void print_hint(std::string_view msg) {
        if (!g_hints_enabled) return;
        *_ostrm << "[HINT] " << msg << std::endl;
    }

    inline void print_info(std::string_view msg) {
        *_ostrm << "[INFO] " << msg << std::endl;
    }
}
