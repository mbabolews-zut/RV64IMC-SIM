#include "ui.hpp"
#include <iostream>
#include <format>
#include <optional>

#ifndef DEBUG
#   define DEBUG 0
#endif

static std::function<void(std::string_view)> output_callback
            = [](auto str) {
                std::cout << str << std::endl; // default output callback
};
static std::function<void(std::string_view)> error_callback
        = [](auto str) {
            std::cerr << str << std::endl; // default error callback
};
static std::optional<std::function<void(std::string_view)>> warning_callback = std::nullopt;
static std::optional<std::function<void(std::string_view)>> info_callback = std::nullopt;
static std::optional<std::function<void(std::string_view)>> hint_callback = std::nullopt;

void ui::set_output_callback(const std::function<void(std::string_view)> &clbk) {
    output_callback = clbk;
}

void ui::set_error_msg_callback(const std::function<void(std::string_view)> &clbk) {
    error_callback = clbk;
}

void ui::set_warning_msg_callback(const std::function<void(std::string_view)> &clbk) {
    warning_callback = clbk;
}

void ui::set_hint_msg_callback(const std::function<void(std::string_view)> &clbk) {
    hint_callback = clbk;
}

void ui::set_info_msg_callback(const std::function<void(std::string_view)> &clbk) {
    info_callback = clbk;
}


void ui::print_error(std::string_view msg, const std::source_location &loc) {
    std::string full_msg;
    if constexpr (DEBUG) {
        full_msg = std::format("[ERROR] [{}:{} ({})] {}\n",
                               loc.file_name(), loc.line(), loc.function_name(), msg);
    } else {
        full_msg = std::format("[ERROR] {}\n", msg);
    }
    error_callback(full_msg);
}

void ui::print_output(std::string_view msg) {
    output_callback(msg);
}

void ui::print_warning(std::string_view msg) {
    warning_callback.value_or(output_callback)(std::format("[WARNING] {}\n", msg));
}

void ui::print_hint(std::string_view msg) {
    if (!hint_callback.has_value()) return;
    (*hint_callback)(std::format("[HINT] {}\n", msg));
}

void ui::print_info(std::string_view msg) {
    info_callback.value_or(output_callback)(std::format("[INFO] {}\n", msg));
}
