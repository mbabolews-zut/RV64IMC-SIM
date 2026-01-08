#pragma once
#include "rv64/VM.hpp"
#include <QObject>
#include <bit>

class Backend;

class SettingsManager : public QObject {
    Q_OBJECT

    // Memory settings
    Q_PROPERTY(int stackSizeKiB READ stackSizeKiB WRITE setStackSizeKiB NOTIFY stackSizeKiBChanged)
    Q_PROPERTY(QString dataAddress READ dataAddress WRITE setDataAddress NOTIFY dataAddressChanged)
    Q_PROPERTY(QString stackAddress READ stackAddress WRITE setStackAddress NOTIFY stackAddressChanged)

    // System settings
    Q_PROPERTY(int spPosIndex READ spPosIndex WRITE setSpPosIndex NOTIFY spPosIndexChanged)
    Q_PROPERTY(int endiannessIndex READ endiannessIndex WRITE setEndiannessIndex NOTIFY endiannessIndexChanged)

public:
    explicit SettingsManager(Backend *parent);

    [[nodiscard]] rv64::VMConfig createVMConfig() const;

    // Memory settings getters/setters
    [[nodiscard]] int stackSizeKiB() const;
    void setStackSizeKiB(int sizeKiB);

    [[nodiscard]] QString dataAddress() const;
    void setDataAddress(const QString &addr);

    [[nodiscard]] QString stackAddress() const;
    void setStackAddress(const QString &addr);

    // System settings getters/setters
    [[nodiscard]] int spPosIndex() const;
    void setSpPosIndex(int index);

    [[nodiscard]] int endiannessIndex() const;
    void setEndiannessIndex(int index);

public slots:
    void loadFromVM();
    QString apply();
    void cancel();

signals:
    void stackSizeKiBChanged();
    void dataAddressChanged();
    void stackAddressChanged();
    void spPosIndexChanged();
    void endiannessIndexChanged();
    void settingsApplied();

private:
    struct SystemConfig {
        rv64::SpPos spPos = rv64::SpPos::StackTop;
        std::endian endianness = std::endian::little;
    };

    struct MemoryConfig {
        uint64_t dataAddr = 0;
        uint64_t stackAddr = 0;
        size_t stackSize = 0;
    };

    static QString toHexString(uint64_t value);
    static uint64_t fromHexString(const QString &str, bool *ok = nullptr);

    [[nodiscard]] Memory::Layout buildMemLayout() const;
    [[nodiscard]] const rv64::VM& vm() const;

    Backend *m_backend;

    // Applied settings
    MemoryConfig m_memoryConfig{};
    SystemConfig m_systemConfig{};

    // Temporary settings (edited in dialog)
    MemoryConfig m_tmpMemoryConfig{};
    SystemConfig m_tmpSystemConfig{};
};