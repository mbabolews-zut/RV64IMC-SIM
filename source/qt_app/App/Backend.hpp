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
    Prepared,
    Running,
    Stopped,
};

class Backend : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool editorLocked READ isEditorLocked NOTIFY editorLockChanged)
    Q_PROPERTY(bool buildingEnabled READ isBuildingEnabled NOTIFY buildingEnabledChanged)
    Q_PROPERTY(QString output READ getOutput NOTIFY outputUpdated)

public:
    explicit Backend(QObject *parent = nullptr);
    ~Backend() override = default;

    bool isEditorLocked() const { return m_lock_editor; }
    bool isBuildingEnabled() const { return m_building_enabled; }
    QString getOutput() const { return m_output; }

    // Output methods with color formatting
    void appendOutput(const QString &text);
    void printSuccess(const QString &text);
    void printWarning(const QString &text);
    void printError(const QString &text);
    void printInfo(const QString &text);
    void clearOutput();

public slots:
    void build(const QString &source_code);
    // void step();
    // void run();
    // void stop();
    void reset();

signals:
    void editorLockChanged(bool locked);
    void buildingEnabledChanged(bool built);
    void errorOccurred(const QString &message);
    void outputUpdated(QStringView output);

private:
    void setAppState(AppState state);

    static QString colorizeText(const QString &text, const QString &color_code);

private:
    rv64::VM m_vm;

    bool m_lock_editor = false;
    bool m_building_enabled = true;
    QString m_output;
    AppState m_app_state = AppState::Idle;
};
