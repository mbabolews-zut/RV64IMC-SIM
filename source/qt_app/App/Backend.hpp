#pragma once

#include <QString>
#include <QUrl>
#include <atomic>
#include "rv64/VM.hpp"
#include "RegisterModel.hpp"
#include "MemoryController.hpp"
#include "SettingsManager.hpp"

enum class AppState {
    Idle = 0, Building, Ready, Running, Stopped, Finished, Error
};

class Backend : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool editorLocked READ isEditorLocked NOTIFY editorLockChanged)
    Q_PROPERTY(bool buildingEnabled READ isBuildingEnabled NOTIFY appStateChanged)
    Q_PROPERTY(bool runLocked READ isRunLocked NOTIFY appStateChanged)
    Q_PROPERTY(bool resetLocked READ isResetLocked NOTIFY appStateChanged)
    Q_PROPERTY(int64_t currentLine READ currentLine NOTIFY appStateChanged)
    Q_PROPERTY(QString output READ output NOTIFY outputUpdated)
    Q_PROPERTY(QString currentFile READ currentFile NOTIFY currentFileChanged)
    Q_PROPERTY(bool fileModified READ isFileModified NOTIFY fileModifiedChanged)
    Q_PROPERTY(QString windowTitle READ windowTitle NOTIFY windowTitleChanged)

public:
    explicit Backend(QObject *parent = nullptr);

    RegisterModel *registerModel() { return &m_registerModel; }
    MemoryController *memoryController() { return &m_memoryController; }
    SettingsManager *settingsManager() { return &m_settingsManager; }

    [[nodiscard]] const rv64::VM& vm() const { return m_vm; }

    bool isEditorLocked() const { return m_editorLocked; }
    bool isBuildingEnabled() const { return m_appState == AppState::Idle; }
    bool isResetLocked() const { return m_appState == AppState::Idle; }
    bool isRunLocked() const;
    int64_t currentLine() const { return m_currentLine; }
    QString output() const { return m_output; }
    QString currentFile() const { return m_currentFile; }
    bool isFileModified() const { return m_fileModified; }
    QString windowTitle() const;

    Q_INVOKABLE bool toggleBreakpoint(int line);
    Q_INVOKABLE bool hasBreakpoint(int line) const;
    Q_INVOKABLE void newFile();
    Q_INVOKABLE QString openFile(const QUrl &fileUrl);
    Q_INVOKABLE QString saveFile(const QString &content);
    Q_INVOKABLE QString saveFileAs(const QUrl &fileUrl, const QString &content);

    void appendOutput(const QString &text);
    void appendError(const QString &text);

public slots:
    void build(const QString &sourceCode);
    void step();
    void run();
    void stop();
    void reset();

public slots:
    void setFileModified(bool modified);

signals:
    void editorLockChanged(bool locked);
    void appStateChanged(AppState newState);
    void outputUpdated();
    void breakpointToggled(int line, bool enabled);
    void breakpointHit(int line);
    void currentFileChanged();
    void fileModifiedChanged();
    void windowTitleChanged();
    void fileLoaded(const QString &content);
    void fileCleared();

private:
    enum class MsgType { Plain, Success, Warning, Error, Info };

    void setAppState(AppState state);
    void handleVmState();
    void print(const QString &text, MsgType type = MsgType::Plain);
    void clearOutput();

    rv64::VM m_vm;
    RegisterModel m_registerModel;
    MemoryController m_memoryController;
    SettingsManager m_settingsManager{this};

    std::atomic_bool m_stopRequested{false};
    bool m_editorLocked = false;
    QString m_output;
    AppState m_appState = AppState::Idle;
    int64_t m_currentLine = -1;

    QString m_currentFile;
    bool m_fileModified = false;
};
