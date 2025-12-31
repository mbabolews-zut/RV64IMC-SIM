#include "Backend.hpp"
#include <QtConcurrent/QtConcurrentRun>
#include <bit>

Backend::Backend(QObject *parent) : QObject(parent) {
}

bool Backend::isEditorLocked() const { return m_lock_editor; }

bool Backend::isBuildingEnabled() const {
    return m_app_state == AppState::Idle;
}

int Backend::getRegDisplayFormat() const { return int(m_disp_format); }

bool Backend::isResetLocked() const {
    return m_app_state == AppState::Idle;
}

bool Backend::isRunLocked() const {
    return m_app_state != AppState::Ready &&
           m_app_state != AppState::Stopped;
}

bool Backend::isModificationAllowed() const {
    return m_app_state == AppState::Stopped || m_app_state == AppState::Ready;
}

QVariantList Backend::getRegisters() const {
    QVariantList result;
    result.reserve(32);

    bool is_signed = m_disp_format == NumDispFormat::Dec;
    int fieldw = m_disp_format == NumDispFormat::Bin
                     ? 64
                     : (m_disp_format == NumDispFormat::Hex ? 16 : 0);

    for (int i = 0; i < 32; ++i) {
        if (is_signed) {
            result.append(
                QString("%1")
                .arg(m_vm.m_cpu.reg(i).sval(), fieldw, int(m_disp_format), QChar('0')));
            continue;
        }
        result.append(
            QString("%1%2")
            .arg(m_disp_format == NumDispFormat::Hex ? "0x" : "")
            .arg(m_vm.m_cpu.reg(i).val(), fieldw, int(m_disp_format), QChar('0')));
    }

    return result;
}

void Backend::appendOutput(const QString &text) {
    m_output += text.toHtmlEscaped().replace("\n", "<br>");
    emit outputUpdated(m_output);
}

void Backend::printSuccess(const QString &text) {
    m_output += colorizeText(text, "#006600");
    emit outputUpdated(m_output);
}

void Backend::printWarning(const QString &text) {
    m_output += colorizeText(text, "#ffc107");
    emit outputUpdated(m_output);
}

void Backend::printError(const QString &text) {
    m_output += colorizeText(text, "#b30000");
    emit outputUpdated(m_output);
}

void Backend::printInfo(const QString &text) {
    m_output += colorizeText(text, "#0a00bf");
    emit outputUpdated(m_output);
}

void Backend::clearOutput() {
    m_output.clear();
    emit outputUpdated(m_output);
}

void Backend::build(const QString &source_code) {
    if (m_app_state != AppState::Idle) return;
    setAppState(AppState::Building);

    clearOutput();
    resetVm();

    QtConcurrent::run([this, str = source_code.toStdString()] {
                asm_parsing::ParsedInstVec parsed_insts{};
                auto result = asm_parsing::parse_and_resolve(str, parsed_insts, m_vm.get_memory_layout().data_base);
                return std::make_pair(result, std::move(parsed_insts));
            })
            .then(this, [this](std::pair<int, asm_parsing::ParsedInstVec> result_pair) {
                auto &[result, insts] = result_pair;
                if (result != 0) {
                    printError("Parse error\n");
                    setAppState(AppState::Idle);
                    return;
                }
                m_vm.load_program(insts);
                printSuccess("Build successful\n");
                m_line = 0;
                setAppState(AppState::Ready);
                ++m_memory_revision;
                emit memoryChanged();
            });
}

void Backend::step() {
    if (isRunLocked()) return;
    setAppState(AppState::Running);
    stepImpl();

    emit registersChanged();
}

void Backend::run() {
    if (isRunLocked()) return;
    m_stop_requested.store(false);
    setAppState(AppState::Running);

    QtConcurrent::run([this] {
        while (!m_stop_requested.load()) {
            m_vm.run_step();

            auto state = m_vm.get_state();
            if (state == rv64::VMState::Error ||
                state == rv64::VMState::Finished ||
                state == rv64::VMState::Breakpoint) {
                break;
            }
        }
    }).then(this, [this] {
        m_line = static_cast<int64_t>(m_vm.get_current_line()) - 1;

        switch (m_vm.get_state()) {
            case rv64::VMState::Error:
                setAppState(AppState::Error);
                printError("VM encountered an error\n");
                break;
            case rv64::VMState::Finished:
                setAppState(AppState::Finished);
                printInfo("Program finished execution\n");
                break;
            case rv64::VMState::Breakpoint:
                setAppState(AppState::Stopped);
                printInfo("Breakpoint hit at line " + QString::number(m_line + 1) + "\n");
                emit breakpointHit(static_cast<int>(m_line));
                break;
            default:
                setAppState(AppState::Stopped);
                break;
        }
        emit registersChanged();
    });
}

void Backend::stop() {
    if (m_app_state != AppState::Running) return;
    m_stop_requested.store(true);
}

void Backend::reset() {
    if (m_app_state == AppState::Building) return;
    resetVm();
    setAppState(AppState::Idle);
    emit registersChanged();
}

bool Backend::modifyRegister(int reg_index, QString value_str) {
    assert(reg_index >= 0 && reg_index < 32);
    if (!isModificationAllowed()) return false;
    if (reg_index == 0 || value_str.isEmpty()) return false;

    if (value_str.startsWith("0x", Qt::CaseInsensitive))
        value_str = value_str.mid(2);

    bool ok = false;
    uint64_t parsed = m_disp_format == NumDispFormat::Dec
                          ? std::bit_cast<uint64_t>(value_str.toLongLong(&ok))
                          : value_str.toULongLong(&ok, int(m_disp_format));

    if (!ok || parsed == m_vm.m_cpu.reg(reg_index).val()) return false;

    m_vm.m_cpu.reg(reg_index) = parsed;
    emit registersChanged();
    return true;
}

void Backend::setRegDisplayFormat(int base) {
    m_disp_format = NumDispFormat(base);
    emit registersChanged();
}

bool Backend::toggleBreakpoint(int line) {
    bool enabled;
    if (m_breakpoints.contains(line)) {
        m_breakpoints.remove(line);
        enabled = false;
    } else {
        m_breakpoints.insert(line);
        enabled = true;
    }
    // Sync to VM (VM uses 1-based lines)
    m_vm.toggle_breakpoint(static_cast<size_t>(line + 1));
    emit breakpointToggled(line, enabled);
    return enabled;
}

bool Backend::hasBreakpoint(int line) const {
    return m_breakpoints.contains(line);
}

void Backend::clearBreakpoints() {
    m_breakpoints.clear();
    m_vm.clear_breakpoints();
}

void Backend::resetVm() {
    m_vm.reset();
    // Re-apply breakpoints to VM (VM uses 1-based lines)
    for (int line : m_breakpoints) {
        m_vm.toggle_breakpoint(static_cast<size_t>(line + 1));
    }
}

void Backend::stepImpl() {
    m_vm.run_step();
    m_line = static_cast<int64_t>(m_vm.get_current_line()) - 1;

    switch (m_vm.get_state()) {
        case rv64::VMState::Error:
            setAppState(AppState::Error);
            printError("VM encountered an error\n");
            break;
        case rv64::VMState::Finished:
            setAppState(AppState::Finished);
            printInfo("Program finished execution\n");
            break;
        default:
            setAppState(AppState::Stopped);
            break;
    }
}

void Backend::setAppState(AppState state) {
    switch (state) {
        case AppState::Idle: {
            m_line = -1;
            emit editorLockChanged(m_lock_editor = false);
            break;
        }
        default: {
            emit editorLockChanged(m_lock_editor = true);
            break;
        }
    }
    m_app_state = state;
    emit appStateChanged(state);
}

QString Backend::colorizeText(const QString &text, const QString &color_code) {
    return QString("<span style='color: %1;'>%2</span>")
            .arg(color_code, text.toHtmlEscaped()
                 .replace("\n", "<br>"));
}

int Backend::memoryRowCount() const {
    return 256; // 256 rows * 16 bytes = 4KB visible memory
}

uint64_t Backend::getDataBaseAddress() const {
    return m_vm.get_memory_layout().data_base;
}

uint64_t Backend::getStackBaseAddress() const {
    return m_vm.get_memory_layout().stack_base;
}

int Backend::dataRowCount() const {
    auto dataSize = m_vm.m_memory.get_data_size();
    return static_cast<int>((dataSize + 15) / 16);
}

int Backend::stackRowCount() const {
    return static_cast<int>(m_vm.get_memory_layout().stack_size / 16);
}

int Backend::getDataByte(uint64_t offset) const {
    MemErr err;
    auto val = m_vm.m_memory.load<uint8_t>(m_vm.get_memory_layout().data_base + offset, err);
    return err == MemErr::None ? static_cast<int>(val) : -1;
}

int Backend::getStackByte(uint64_t offset) const {
    MemErr err;
    auto val = m_vm.m_memory.load<uint8_t>(m_vm.get_memory_layout().stack_base + offset, err);
    return err == MemErr::None ? static_cast<int>(val) : -1;
}

bool Backend::modifyMemoryByte(uint64_t address, int value) {
    if (!isModificationAllowed() || value < 0 || value > 255) return false;
    if (m_vm.m_memory.store(address, static_cast<uint8_t>(value)) != MemErr::None) return false;
    ++m_memory_revision;
    emit memoryChanged();
    return true;
}

namespace {
template<std::integral T>
bool parseAndStore(Memory &mem, uint64_t addr, const QString &str, int base = 10) {
    bool ok;
    T val = std::is_signed_v<T>
        ? static_cast<T>(str.toLongLong(&ok, base))
        : static_cast<T>(str.toULongLong(&ok, base));
    return ok && mem.store(addr, val) == MemErr::None;
}
}

bool Backend::modifyMemoryValue(uint64_t address, int typeIndex, const QString &valueStr) {
    if (!isModificationAllowed()) return false;

    bool ok = [&] {
        switch (typeIndex) {
            case 0: return parseAndStore<uint8_t>(m_vm.m_memory, address, valueStr, 2);
            case 1: return parseAndStore<int8_t>(m_vm.m_memory, address, valueStr);
            case 2: return parseAndStore<uint8_t>(m_vm.m_memory, address, valueStr);
            case 3: return parseAndStore<int16_t>(m_vm.m_memory, address, valueStr);
            case 4: return parseAndStore<uint16_t>(m_vm.m_memory, address, valueStr);
            case 5: return parseAndStore<int32_t>(m_vm.m_memory, address, valueStr);
            case 6: return parseAndStore<uint32_t>(m_vm.m_memory, address, valueStr);
            case 7: return parseAndStore<int64_t>(m_vm.m_memory, address, valueStr);
            case 8: return parseAndStore<uint64_t>(m_vm.m_memory, address, valueStr);
            default: return false;
        }
    }();

    if (!ok) return false;
    ++m_memory_revision;
    emit memoryChanged();
    return true;
}

void Backend::loadDataTypesForAddress(uint64_t address) {
    QVariantList values;
    MemErr err;

    // Binary (8-bit)
    auto u8 = m_vm.m_memory.load<uint8_t>(address, err);
    values.append(err == MemErr::None ? QString("%1").arg(u8, 8, 2, QChar('0')) : "N/A");


    // Signed (8-bit)
    values.append(err == MemErr::None ? QString::number(static_cast<int8_t>(u8)) : "N/A");

    // Unsigned (8-bit)
    values.append(err == MemErr::None ? QString::number(u8) : "N/A");

    // Signed (16-bit)
    auto i16 = m_vm.m_memory.load<int16_t>(address, err);
    values.append(err == MemErr::None ? QString::number(i16) : "N/A");

    // Unsigned (16-bit)
    auto u16 = m_vm.m_memory.load<uint16_t>(address, err);
    values.append(err == MemErr::None ? QString::number(u16) : "N/A");

    // Signed (32-bit)
    auto i32 = m_vm.m_memory.load<int32_t>(address, err);
    values.append(err == MemErr::None ? QString::number(i32) : "N/A");

    // Unsigned (32-bit)
    auto u32 = m_vm.m_memory.load<uint32_t>(address, err);
    values.append(err == MemErr::None ? QString::number(u32) : "N/A");

    // Signed (64-bit)
    auto i64 = m_vm.m_memory.load<int64_t>(address, err);
    values.append(err == MemErr::None ? QString::number(i64) : "N/A");

    // Unsigned (64-bit)
    auto u64 = m_vm.m_memory.load<uint64_t>(address, err);
    values.append(err == MemErr::None ? QString::number(u64) : "N/A");

    emit dataTypesLoaded(values);
}

