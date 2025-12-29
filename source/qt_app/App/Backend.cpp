#include "Backend.hpp"
#include <QtConcurrent/QtConcurrent>

Backend::Backend(QObject *parent) : QObject(parent) {
}

bool Backend::isBuildingEnabled() const {
    return m_app_state == AppState::Idle;
}

bool Backend::isResetLocked() const {
    return m_app_state == AppState::Idle;
}

bool Backend::isRunLocked() const {
    return m_app_state != AppState::Ready &&
           m_app_state != AppState::Stopped;
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
                .arg(static_cast<int64_t>(m_vm.m_cpu.reg(i).val()), fieldw, int(m_disp_format), QChar('0')));
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
    m_output += colorizeText(text, "#28a745");
    emit outputUpdated(m_output);
}

void Backend::printWarning(const QString &text) {
    m_output += colorizeText(text, "#ffc107");
    emit outputUpdated(m_output);
}

void Backend::printError(const QString &text) {
    m_output += colorizeText(text, "#dc3545");
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
    m_vm.reset();

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
            });
}

void Backend::step() {
    if (isRunLocked()) return;
    setAppState(AppState::Running);

    if (m_vm.get_state() == rv64::VMState::Error) {
        printError("VM encountered an error\n");
        setAppState(AppState::Error);
        return;
    }

    m_vm.run_step();
    m_line = m_vm.get_current_line() - 1;

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

    emit registersChanged();
}

void Backend::reset() {
    if (m_app_state == AppState::Building) return;
    m_vm.reset();
    setAppState(AppState::Idle);
    emit registersChanged();
}

void Backend::setRegDisplayFormat(int base) {
    m_disp_format = NumDispFormat(base);
    emit registersChanged();
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
