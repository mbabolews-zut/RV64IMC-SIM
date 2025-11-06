#pragma once
#include <iostream>
#include <string_view>

namespace ui {
    static inline std::ostream *_ostrm = &std::cout;
    static inline std::ostream *_estrm = &std::cerr;

    inline void set_out_stream(std::ostream &os) {
        _ostrm = &os;
    }

    inline void set_err_stream(std::ostream &os) {
        _estrm = &os;
    }

    inline void display_error(std::string_view msg) {
        *_estrm << "[ERROR] " << msg << std::endl;
    }

    inline void print_output(std::string_view msg) {
        *_ostrm << "" << msg << std::endl;
    }

    inline void print_warning(std::string_view msg) {
        *_ostrm << "[WARNING] " << msg << std::endl;
    }

    inline void print_info(std::string_view msg) {
        *_ostrm << "[INFO] " << msg << std::endl;
    }
}
