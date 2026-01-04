#include <rv64/VM.hpp>
#include <common.hpp>
#include <ui.hpp>
#include <chrono>
#include <functional>
#include <random>
#include <iomanip>
#include <format>

using namespace rv64;

// VT100 colors
namespace color {
    constexpr auto RESET = "\033[0m", BOLD = "\033[1m", DIM = "\033[2m";
    constexpr auto RED = "\033[31m", GREEN = "\033[32m", YELLOW = "\033[33m";
    constexpr auto BLUE = "\033[34m", MAGENTA = "\033[35m", CYAN = "\033[36m", WHITE = "\033[37m";
    constexpr auto CLEAR_LINE = "\033[2K\r";
}

// Forward declarations
void print_section_header(std::string_view title, const char *border_color);
void print_progress_bar(std::string_view name, int current, int total, std::string_view status);
void print_results_table(std::string_view name, const std::vector<int> &n_vals,
                         const std::vector<int64_t> &parse, const std::vector<int64_t> &exec,
                         const std::vector<int64_t> &total);
std::string format_time(int64_t us);

static std::mt19937 g_rng;

int random_int(int min, int max) {
    return std::uniform_int_distribution(min, max)(g_rng);
}

// Stopwatch
struct Stopwatch {
    std::chrono::steady_clock::time_point t0;
    int64_t elapsed_us = 0;

    void start() { t0 = std::chrono::steady_clock::now(); }

    void stop() {
        elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - t0).count();
    }
};

// Test case
struct TestCase {
    std::string_view name;
    std::string source;
    std::function<std::string(std::string_view, int)> prep;
    std::vector<int64_t> parse_times, exec_times, total_times;

    void run(const std::vector<int> &n_values) {
        parse_times.clear();
        exec_times.clear();
        total_times.clear();

        auto null_cb = [](std::string_view) {
        };
        ui::set_output_callback(null_cb);
        ui::set_error_msg_callback(null_cb);

        int total = static_cast<int>(n_values.size());
        for (int i = 0; i < total; ++i) {
            int n = n_values[i];
            print_progress_bar(name, i, total, std::format("N={}", n));

            VM vm;
            Stopwatch sw_total, sw_parse, sw_exec;
            asm_parsing::ParsedInstVec instructions;

            sw_total.start();
            sw_parse.start();
            int res = asm_parsing::parse_and_resolve(prep(source, n), instructions, vm.m_cpu.get_pc());
            sw_parse.stop();

            if (res != 0) {
                std::cout << color::CLEAR_LINE;
                std::cerr << color::RED << "✗ Parse failed: " << color::RESET << name << " n=" << n << "\n";
                parse_times.push_back(-1);
                exec_times.push_back(-1);
                total_times.push_back(-1);
                continue;
            }

            vm.load_program(instructions);
            sw_exec.start();
            vm.run_until_stop();
            sw_exec.stop();
            sw_total.stop();

            parse_times.push_back(sw_parse.elapsed_us);
            exec_times.push_back(sw_exec.elapsed_us);
            total_times.push_back(sw_total.elapsed_us);
        }
        std::cout << color::CLEAR_LINE << std::flush;
    }
};

// Source prep helpers
std::string load_imm_asm(int val) {
    if (val >= -2048 && val <= 2047)
        return std::format("addi a0, zero, {}\n", val);
    int hi = (val + 0x800) >> 12, lo = val - (hi << 12);
    return std::format("lui a0, {}\naddi a0, a0, {}\n", hi, lo);
}

auto prep_repeat = [](std::string_view src, int n) {
    std::string r;
    r.reserve(src.size() * n + 10);
    for (int i = 0; i < n; ++i) r += src;
    return r + "\necall";
};

auto prep_load_n = [](std::string_view src, int n) {
    std::string r(src);
    if (auto p = r.find("{}"); p != std::string::npos) r.replace(p, 2, load_imm_asm(n));
    return r;
};

auto prep_muldiv = [](std::string_view src, int n) {
    int r1 = random_int(1, int12::MAX);
    int r2 = random_int(1, int12::MAX);
    int iters = std::clamp(n, 1, static_cast<int>(int12::MAX));
    return std::vformat(src, std::make_format_args(r1, r2, iters));
};

// Test cases
std::vector<TestCase> create_tests() {
    return {
        {
            "Simple Arithmetic (repeated N times)",
            R"(
        lui x3, 0x3
        addi x1, x3, 9
        addi x1, x1, 12
        srai x5, x1, 1
        sub x5, x3, x5
        xor x1, x1, x2
        sll x1, x1, x3
        or x1, x3, ra
        addi a0, zero, 10
        and x1, x1, sp
        )",
            prep_repeat, {}, {}, {}
        },

        {
            "Loop (N iterations)",
            R"(
        {}
        addi x1, zero, 0
        loop0:
            addi x1, x1, 1
            blt x1, a0, loop0
        addi a0, zero, 10
        ecall
        )",
            prep_load_n, {}, {}, {}
        },

        {
            "Multiply/Divide intensive (N iterations)",
            R"(
        addi x10, zero, {}
        addi x11, zero, {}
        addi x1, zero, 0
        addi x2, zero, {}
        loop0:
            mul x12, x10, x11
            mulh x13, x10, x11
            mulhu x14, x10, x11
            addi x15, x10, 1
            addi x16, x11, 1
            beq x12, zero, skip_div
            div x17, x15, x12
            divu x18, x16, x12
            rem x19, x15, x12
            remu x20, x16, x12
        skip_div:
            addi x1, x1, 1
            blt x1, x2, loop0
        addi a0, zero, 10
        ecall
        )",
            prep_muldiv, {}, {}, {}
        },

        {
            "Memory operations (N iterations)",
            R"(
        lui x2, 0x400
        {}
        addi x1, zero, 0
        mem_loop:
            sw x1, 0(x2)
            lw x4, 0(x2)
            sh x1, 4(x2)
            lh x5, 4(x2)
            sb x1, 6(x2)
            lb x6, 6(x2)
            sd x1, 8(x2)
            ld x7, 8(x2)
            addi x1, x1, 1
            blt x1, a0, mem_loop
        addi a0, zero, 10
        ecall
        )",
            prep_load_n, {}, {}, {}
        },

        {
            "Branch-heavy (N iterations)",
            R"(
        addi x1, zero, 0
        {}
        addi x3, zero, 5
        branch_loop:
            beq x1, x3, skip1
            addi x4, x4, 1
        skip1:
            bne x1, x3, skip2
            addi x5, x5, 1
        skip2:
            blt x1, x3, skip3
            addi x6, x6, 1
        skip3:
            bge x1, x3, skip4
            addi x7, x7, 1
        skip4:
            addi x1, x1, 1
            blt x1, a0, branch_loop
        addi a0, zero, 10
        ecall
        )",
            prep_load_n, {}, {}, {}
        }
    };
}

int main() {
    unsigned seed = std::random_device()();
    g_rng.seed(seed);

    print_section_header("RV64 Simulator Performance Test", color::BLUE);
    std::cout << color::DIM << "Random seed: " << seed << color::RESET << "\n";

    auto tests = create_tests();
    std::vector n_values = {10, 100, 1000, 10000, 100000};

    for (auto &t: tests) {
        t.run(n_values);
        print_results_table(t.name, n_values, t.parse_times, t.exec_times, t.total_times);
    }

    int64_t total_parse = 0, total_exec = 0;
    for (const auto &t: tests)
        for (size_t i = 0; i < t.parse_times.size(); ++i) {
            if (t.parse_times[i] > 0) total_parse += t.parse_times[i];
            if (t.exec_times[i] > 0) total_exec += t.exec_times[i];
        }

    print_section_header("Summary", color::GREEN);
    std::cout << std::format("{}Total parsing time:   {}{}{}\n", color::YELLOW, color::BOLD, format_time(total_parse),
                             color::RESET)
            << std::format("{}Total execution time: {}{}{}\n", color::GREEN, color::BOLD, format_time(total_exec),
                           color::RESET)
            << std::format("{}Grand total:          {}{}{}\n", color::MAGENTA, color::BOLD,
                           format_time(total_parse + total_exec), color::RESET);
}

// ============================================================================
// UI helpers
// ============================================================================

void print_section_header(std::string_view title, const char *border_color) {
    constexpr int W = 67;
    std::string border(W, '=');
    int pad = (W - static_cast<int>(title.size())) / 2;
    std::string centered = std::string(pad, ' ') + std::string(title);
    centered.resize(W, ' ');

    std::cout << border_color << border << color::RESET << "\n"
            << color::BOLD << color::WHITE << centered << color::RESET << "\n"
            << border_color << border << color::RESET << "\n";
}

void print_progress_bar(std::string_view name, int current, int total, std::string_view status) {
    constexpr int W = 30;
    int filled = total > 0 ? (current * W) / total : 0;

    std::cout << color::CLEAR_LINE << color::CYAN << "▶ " << color::RESET
            << color::BOLD << name << color::RESET << " [" << color::GREEN;
    for (int i = 0; i < filled; ++i) std::cout << "█";
    std::cout << color::DIM;
    for (int i = filled; i < W; ++i) std::cout << "░";
    std::cout << color::RESET << "] " << color::YELLOW << current << "/" << total
            << color::RESET << " " << color::DIM << status << color::RESET << std::flush;
}

std::string format_time(int64_t us) {
    if (us < 0) return "ERROR";
    if (us < 1000) return std::to_string(us) + " us";
    if (us < 1000000) return std::format("{:.2f} ms", us / 1000.0);
    return std::format("{:.2f} s", us / 1000000.0);
}

void print_results_table(std::string_view name, const std::vector<int> &n_vals,
                         const std::vector<int64_t> &parse, const std::vector<int64_t> &exec,
                         const std::vector<int64_t> &total) {
    std::cout << "\n" << color::CYAN
            << "╔══════════════════════════════════════════════════════════════════╗\n"
            << "║ " << color::BOLD << color::WHITE << std::left << std::setw(64) << name
            << color::RESET << color::CYAN << " ║\n"
            << "╚══════════════════════════════════════════════════════════════════╝" << color::RESET << "\n";

    constexpr std::array W = {10, 12, 12, 12};
    constexpr std::array HDR = {"N", "Parse", "Execute", "Total"};
    constexpr std::array COL = {color::CYAN, color::YELLOW, color::GREEN, color::MAGENTA};

    auto sep = [&] {
        std::cout << color::DIM << "+";
        for (int w: W) std::cout << std::string(w + 2, '-') << "+";
        std::cout << color::RESET << "\n";
    };

    sep();
    std::cout << color::DIM << "|" << color::RESET;
    for (size_t i = 0; i < 4; ++i)
        std::cout << color::BOLD << " " << std::setw(W[i]) << HDR[i] << color::RESET << color::DIM << " |" <<
                color::RESET;
    std::cout << "\n";
    sep();

    for (size_t i = 0; i < n_vals.size(); ++i) {
        std::array cells = {
            std::to_string(n_vals[i]), format_time(parse[i]),
            format_time(exec[i]), format_time(total[i])
        };
        bool err = parse[i] < 0 || exec[i] < 0;

        std::cout << color::DIM << "|" << color::RESET;
        for (size_t j = 0; j < 4; ++j) {
            auto c = (err && cells[j] == "ERROR") ? color::RED : COL[j];
            std::cout << c << " " << std::setw(W[j]) << cells[j] << color::RESET << color::DIM << " |" << color::RESET;
        }
        std::cout << "\n";
    }
    sep();
}
