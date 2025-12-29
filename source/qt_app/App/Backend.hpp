#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <memory>
#include "rv64/VM.hpp"

enum class AppState {
    Idle,
    Building,
    Ready,
    Running,
    Stopped,
    Finished,
    Error
};


class Backend : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString output READ getOutput NOTIFY outputUpdated)
    Q_PROPERTY(QVariantList registers READ getRegisters NOTIFY registersChanged)
    // Buttons and editor locking
    Q_PROPERTY(bool editorLocked READ isEditorLocked NOTIFY editorLockChanged)
    Q_PROPERTY(bool buildingEnabled READ isBuildingEnabled NOTIFY appStateChanged)
    Q_PROPERTY(bool runLocked READ isRunLocked NOTIFY appStateChanged) // also for step button
    Q_PROPERTY(bool resetLocked READ isResetLocked NOTIFY appStateChanged)
    Q_PROPERTY(int64_t currentLine READ getCurrentLine NOTIFY appStateChanged)
public:
    explicit Backend(QObject *parent = nullptr);
    ~Backend() override = default;

    bool isEditorLocked() const { return m_lock_editor; }
    bool isBuildingEnabled() const;
    bool isResetLocked() const;
    bool isRunLocked() const;

    QString getOutput() const { return m_output; }
    QVariantList getRegisters() const;
    int64_t getCurrentLine() const { return m_line; }

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
    // void run();
    // void stop();
    void reset();

    void setRegDisplayFormat(int base); // 10: Dec, 16: Hex, 2: Bin

signals:
    void editorLockChanged(bool locked);
    void buildingEnabledChanged(bool built);
    void errorOccurred(const QString &message);
    void outputUpdated(QStringView output);
    void appStateChanged(AppState new_state);
    void registersChanged();

private:
    void setAppState(AppState state);

    static QString colorizeText(const QString &text, const QString &color_code);

private:
    enum class NumDispFormat { Dec = 10, Hex = 16, Bin = 2 };

    rv64::VM m_vm;

    bool m_lock_editor = false;
    QString m_output;
    AppState m_app_state = AppState::Idle;
    NumDispFormat m_disp_format = NumDispFormat::Dec;
    int64_t m_line = -1;
};
