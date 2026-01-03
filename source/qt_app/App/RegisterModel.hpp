#pragma once

#include <QAbstractListModel>
#include <array>
#include <rv64/Cpu.hpp>

class RegisterModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(DisplayFormat format READ format WRITE setFormat NOTIFY formatChanged)

public:
    enum class DisplayFormat { Dec = 10, Hex = 16, Bin = 2 };
    Q_ENUM(DisplayFormat)

    enum class Role : int {
        Reg = Qt::UserRole,
        Abi,
        Value,
        UserModified,
        CoreModified
    };

    explicit RegisterModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    DisplayFormat format() const { return m_format; }
    void setFormat(DisplayFormat fmt);

    void updateFromCpu(const rv64::Cpu &cpu);
    void clearCoreModifiedFlags();
    void resetAllFlags();

    Q_INVOKABLE bool modify(int index, const QString &value);
    Q_INVOKABLE int maxInputLength() const;
    Q_INVOKABLE QString validatorPattern() const;

signals:
    void formatChanged();
    void registerModified(int index, uint64_t newValue);

private:
    QString formatValue(uint64_t value) const;

    struct RegInfo {
        const char *name;
        const char *abi;
    };
    static constexpr std::array<RegInfo, 32> s_regInfo = {{
        {"x0", "zero"}, {"x1", "ra"}, {"x2", "sp"}, {"x3", "gp"},
        {"x4", "tp"}, {"x5", "t0"}, {"x6", "t1"}, {"x7", "t2"},
        {"x8", "s0/fp"}, {"x9", "s1"}, {"x10", "a0"}, {"x11", "a1"},
        {"x12", "a2"}, {"x13", "a3"}, {"x14", "a4"}, {"x15", "a5"},
        {"x16", "a6"}, {"x17", "a7"}, {"x18", "s2"}, {"x19", "s3"},
        {"x20", "s4"}, {"x21", "s5"}, {"x22", "s6"}, {"x23", "s7"},
        {"x24", "s8"}, {"x25", "s9"}, {"x26", "s10"}, {"x27", "s11"},
        {"x28", "t3"}, {"x29", "t4"}, {"x30", "t5"}, {"x31", "t6"}
    }};

    DisplayFormat m_format = DisplayFormat::Dec;
    std::array<uint64_t, 32> m_values{};
    std::array<uint64_t, 32> m_prevValues{};
    std::array<bool, 32> m_userModified{};
    std::array<bool, 32> m_coreModified{};
};
