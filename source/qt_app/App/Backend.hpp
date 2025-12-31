#pragma once

#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QSet>
#include <atomic>
#include "rv64/VM.hpp"

enum class AppState {
    Idle = 0, Building = 1, Ready = 2, Running = 3, Stopped = 4, Finished = 5, Error = 6
};

class Backend : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString output READ getOutput NOTIFY outputUpdated)
    Q_PROPERTY(QVariantList registers READ getRegisters NOTIFY registersChanged)
    Q_PROPERTY(int regDisplayFormat READ getRegDisplayFormat NOTIFY registersChanged)
    // Buttons and editor locking
    Q_PROPERTY(bool editorLocked READ isEditorLocked NOTIFY editorLockChanged)
    Q_PROPERTY(bool buildingEnabled READ isBuildingEnabled NOTIFY appStateChanged)
    Q_PROPERTY(bool runLocked READ isRunLocked NOTIFY appStateChanged) // also for step button
    Q_PROPERTY(bool resetLocked READ isResetLocked NOTIFY appStateChanged)
    Q_PROPERTY(int64_t currentLine READ getCurrentLine NOTIFY appStateChanged)
    Q_PROPERTY(int memoryRowCount READ memoryRowCount NOTIFY memoryChanged)
    Q_PROPERTY(uint64_t dataBaseAddress READ getDataBaseAddress NOTIFY memoryLayoutChanged)
    Q_PROPERTY(uint64_t stackBaseAddress READ getStackBaseAddress NOTIFY memoryLayoutChanged)
    Q_PROPERTY(int dataRowCount READ dataRowCount NOTIFY memoryChanged)
    Q_PROPERTY(int stackRowCount READ stackRowCount NOTIFY memoryChanged)
    Q_PROPERTY(int memoryRevision READ memoryRevision NOTIFY memoryChanged)

public:
    explicit Backend(QObject *parent = nullptr);
    ~Backend() override = default;

    Q_INVOKABLE int getDataByte(uint64_t offset) const;
    Q_INVOKABLE int getStackByte(uint64_t offset) const;
    Q_INVOKABLE bool modifyMemoryByte(uint64_t address, int value);
    Q_INVOKABLE bool modifyMemoryValue(uint64_t address, int typeIndex, const QString &valueStr);
    Q_INVOKABLE void loadDataTypesForAddress(uint64_t address);

    bool isEditorLocked() const;
    bool isBuildingEnabled() const;
    int getRegDisplayFormat() const;
    bool isResetLocked() const;
    bool isRunLocked() const;
    bool isModificationAllowed() const;

    QString getOutput() const { return m_output; }
    QVariantList getRegisters() const;
    int64_t getCurrentLine() const { return m_line; }
    int memoryRowCount() const;
    uint64_t getDataBaseAddress() const;
    uint64_t getStackBaseAddress() const;
    int dataRowCount() const;
    int stackRowCount() const;
    int memoryRevision() const { return m_memory_revision; }

    // Output methods with color formatting
    void appendOutput(const QString &text);
    void printSuccess(const QString &text);
    void printWarning(const QString &text);
    void printError(const QString &text);
    void printInfo(const QString &text);
    void clearOutput();

public slots:
    void build(const QString &source_code);
    void step();
    void run();
    void stop();
    void reset();

    bool modifyRegister(int reg_index, QString value_str);
    void setRegDisplayFormat(int base); // 10: Dec, 16: Hex, 2: Bin

    // Breakpoint management
    bool toggleBreakpoint(int line);
    bool hasBreakpoint(int line) const;
    void clearBreakpoints();

signals:
    void editorLockChanged(bool locked);
    void buildingEnabledChanged(bool built);
    void errorOccurred(const QString &message);
    void outputUpdated(QStringView output);
    void appStateChanged(AppState new_state);
    void registersChanged();
    void breakpointToggled(int line, bool enabled);
    void breakpointHit(int line);

    void dataTypesLoaded(QVariantList values);
    void memoryChanged();
    void memoryLayoutChanged();

private:
    void resetVm();

    void stepImpl();

    void setAppState(AppState state);

    static QString colorizeText(const QString &text, const QString &color_code);

private:
    enum class NumDispFormat { Dec = 10, Hex = 16, Bin = 2 };

    rv64::VM m_vm;
    QSet<int> m_breakpoints;

    std::atomic_bool m_stop_requested{false};

    bool m_lock_editor = false;
    QString m_output;
    AppState m_app_state = AppState::Idle;
    NumDispFormat m_disp_format = NumDispFormat::Dec;
    int64_t m_line = -1;
    int m_memory_revision = 0;
};
