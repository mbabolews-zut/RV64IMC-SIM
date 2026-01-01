#pragma once
#include <functional>

#include <source_location>
#include <string_view>


namespace ui {

    void set_output_callback(const std::function<void(std::string_view)> &clbk);

    void set_warning_msg_callback(const std::function<void(std::string_view)> &clbk);

    void set_error_msg_callback(const std::function<void(std::string_view)> &clbk);

    void set_hint_msg_callback(const std::function<void(std::string_view)> &clbk);

    void set_info_msg_callback(const std::function<void(std::string_view)> &clbk);



    void print_error(std::string_view msg,
                     const std::source_location &loc = std::source_location::current());

    void print_output(std::string_view msg);

    void print_warning(std::string_view msg);

    void print_hint(std::string_view msg);

    void print_info(std::string_view msg);
}