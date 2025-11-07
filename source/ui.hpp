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

#ifdef DEBUG
#define PRINT_ERROR(msg)\
    *ui::_estrm << "[ERROR in " << __FILE__ << ":" << __LINE__ \
    << " (" << __func__ << ")] " << msg << std::endl
#else
#define PRINT_ERROR(msg) \
    *ui::_estrm << "[ERROR]" \
    << " (" << __func__ << ")] " << msg << std::endl
#endif

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
