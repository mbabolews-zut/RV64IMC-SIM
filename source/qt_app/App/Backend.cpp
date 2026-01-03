#include "Backend.hpp"
#include <QtConcurrent/QtConcurrentRun>

Backend::Backend(QObject *parent)
    : QObject(parent)
    , m_registerModel(this)
    , m_memoryController(m_vm.m_memory, m_vm.get_memory_layout(), this) {

    connect(&m_registerModel, &RegisterModel::registerModified,
            this, [this](int index, uint64_t value) {
        m_vm.m_cpu.reg(index) = value;
    });
}

bool Backend::isRunLocked() const {
    return m_appState != AppState::Ready && m_appState != AppState::Stopped;
}

void Backend::build(const QString &sourceCode) {
    if (m_appState != AppState::Idle)
        return;

    setAppState(AppState::Building);
    clearOutput();
    m_vm.reset();

    QtConcurrent::run([this, str = sourceCode.toStdString()] {
        asm_parsing::ParsedInstVec parsed{};
        auto result = asm_parsing::parse_and_resolve(str, parsed, m_vm.m_cpu.get_pc());
        return std::make_pair(result, std::move(parsed));
    }).then(this, [this](std::pair<int, asm_parsing::ParsedInstVec> result) {
        auto &[code, insts] = result;
        if (code != 0) {
            print("Parse error\n", MsgType::Error);
            setAppState(AppState::Idle);
            return;
        }
        m_vm.load_program(insts);
        print("Build successful\n", MsgType::Success);
        m_currentLine = 0;
        setAppState(AppState::Ready);
        m_registerModel.updateFromCpu(m_vm.m_cpu);
        m_memoryController.notifyContentChanged();
    });
}

void Backend::step() {
    if (isRunLocked())
        return;

    m_registerModel.clearCoreModifiedFlags();
    setAppState(AppState::Running);
    m_vm.run_step();
    m_currentLine = int64_t(m_vm.get_current_line()) - 1;
    handleVmState();
    m_registerModel.updateFromCpu(m_vm.m_cpu);
}

void Backend::run() {
    if (isRunLocked())
        return;

    m_registerModel.clearCoreModifiedFlags();
    m_stopRequested.store(false);
    setAppState(AppState::Running);

    QtConcurrent::run([this] {
        while (!m_stopRequested.load()) {
            m_vm.run_step();
            auto state = m_vm.get_state();
            if (state == rv64::VMState::Error ||
                state == rv64::VMState::Finished ||
                state == rv64::VMState::Breakpoint) {
                break;
            }
        }
    }).then(this, [this] {
        m_currentLine = int64_t(m_vm.get_current_line()) - 1;
        handleVmState();
        m_registerModel.updateFromCpu(m_vm.m_cpu);
    });
}

void Backend::stop() {
    if (m_appState == AppState::Running)
        m_stopRequested.store(true);
}

void Backend::reset() {
    if (m_appState == AppState::Building)
        return;

    m_vm.reset();
    setAppState(AppState::Idle);
    m_registerModel.updateFromCpu(m_vm.m_cpu);
    m_registerModel.resetAllFlags();
}

void Backend::handleVmState() {
    switch (m_vm.get_state()) {
        case rv64::VMState::Error:
            setAppState(AppState::Error);
            print("VM encountered an error\n", MsgType::Error);
            break;
        case rv64::VMState::Finished:
            setAppState(AppState::Finished);
            print("Program finished execution\n", MsgType::Info);
            break;
        case rv64::VMState::Breakpoint:
            setAppState(AppState::Stopped);
            print("Breakpoint hit at line " + QString::number(m_currentLine + 1) + "\n", MsgType::Info);
            emit breakpointHit(int(m_currentLine));
            break;
        default:
            setAppState(AppState::Stopped);
            break;
    }
}

void Backend::setAppState(AppState state) {
    bool shouldLock = (state != AppState::Idle);
    if (m_editorLocked != shouldLock) {
        m_editorLocked = shouldLock;
        emit editorLockChanged(m_editorLocked);
    }

    if (state == AppState::Idle) {
        m_currentLine = -1;
        m_registerModel.resetAllFlags();
    }

    bool canModify = (state == AppState::Ready || state == AppState::Stopped);
    m_memoryController.setModificationAllowed(canModify);

    m_appState = state;
    emit appStateChanged(state);
}

bool Backend::toggleBreakpoint(int line) {
    bool enabled = m_vm.toggle_breakpoint(size_t(line + 1));
    emit breakpointToggled(line, enabled);
    return enabled;
}

bool Backend::hasBreakpoint(int line) const {
    return m_vm.has_breakpoint(size_t(line + 1));
}

// Output

void Backend::print(const QString &text, MsgType type) {
    static const QHash<MsgType, QString> colors = {
        {MsgType::Plain, QString()},
        {MsgType::Success, "#006600"},
        {MsgType::Warning, "#ffc107"},
        {MsgType::Error, "#b30000"},
        {MsgType::Info, "#0a00bf"}
    };

    QString escaped = text.toHtmlEscaped().replace("\n", "<br>");
    if (type == MsgType::Plain) {
        m_output += escaped;
    } else {
        m_output += QStringLiteral("<span style='color: %1;'>%2</span>")
                        .arg(colors[type], escaped);
    }
    emit outputUpdated();
}

void Backend::clearOutput() {
    m_output.clear();
    emit outputUpdated();
}

void Backend::appendOutput(const QString &text) {
    print(text, MsgType::Plain);
}

void Backend::appendError(const QString &text) {
    print(text, MsgType::Error);
}
