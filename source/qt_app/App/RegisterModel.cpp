#include "RegisterModel.hpp"
#include <bit>

RegisterModel::RegisterModel(QObject *parent)
    : QAbstractListModel(parent) {
    m_values.fill(0);
    m_originalValues.fill(0);
    m_modified.fill(false);
}

int RegisterModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : 32;
}

QVariant RegisterModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= 32)
        return {};

    const auto row = static_cast<size_t>(index.row());
    switch (static_cast<Role>(role)) {
        case Role::Reg:
            return QString::fromLatin1(s_regInfo[row].name);
        case Role::Abi:
            return QString::fromLatin1(s_regInfo[row].abi);
        case Role::Value:
            return formatValue(m_values[row]);
        case Role::Modified:
            return m_modified[row];
    }
    return {};
}

QHash<int, QByteArray> RegisterModel::roleNames() const {
    return {
        {int(Role::Reg), "reg"},
        {int(Role::Abi), "abi"},
        {int(Role::Value), "value"},
        {int(Role::Modified), "modified"}
    };
}

void RegisterModel::setFormat(DisplayFormat fmt) {
    if (m_format == fmt)
        return;
    m_format = fmt;
    emit formatChanged();
    emit dataChanged(index(0), index(31), {int(Role::Value)});
}

void RegisterModel::updateFromCpu(const rv64::Cpu &cpu) {
    for (int i = 0; i < 32; ++i) {
        uint64_t newVal = cpu.reg(i).val();
        if (m_values[i] != newVal) {
            m_values[i] = newVal;
            m_modified[i] = (newVal != m_originalValues[i]);
        }
    }
    emit dataChanged(index(0), index(31), {int(Role::Value), int(Role::Modified)});
}

void RegisterModel::resetModifiedFlags() {
    for (int i = 0; i < 32; ++i) {
        m_originalValues[i] = m_values[i];
        m_modified[i] = false;
    }
    emit dataChanged(index(0), index(31), {int(Role::Modified)});
}

bool RegisterModel::modify(int idx, const QString &valueStr) {
    if (idx <= 0 || idx >= 32 || valueStr.isEmpty())
        return false;

    QString str = valueStr;
    if (str.startsWith("0x", Qt::CaseInsensitive))
        str = str.mid(2);

    bool ok = false;
    uint64_t parsed;

    if (m_format == DisplayFormat::Dec) {
        parsed = std::bit_cast<uint64_t>(str.toLongLong(&ok));
    } else {
        parsed = str.toULongLong(&ok, static_cast<int>(m_format));
    }

    if (!ok || parsed == m_values[idx])
        return false;

    m_values[idx] = parsed;
    m_modified[idx] = (parsed != m_originalValues[idx]);
    emit dataChanged(index(idx), index(idx), {int(Role::Value), int(Role::Modified)});
    emit registerModified(idx, parsed);
    return true;
}

int RegisterModel::maxInputLength() const {
    switch (m_format) {
        case DisplayFormat::Bin: return 64;
        case DisplayFormat::Hex: return 18; // 0x + 16 digits
        case DisplayFormat::Dec: return 20;
    }
    return 20;
}

QString RegisterModel::validatorPattern() const {
    switch (m_format) {
        case DisplayFormat::Bin: return QStringLiteral("[01]+");
        case DisplayFormat::Hex: return QStringLiteral("0?[xX]?[0-9a-fA-F]*");
        case DisplayFormat::Dec: return QStringLiteral("-?[0-9]*");
    }
    return QStringLiteral(".*");
}

QString RegisterModel::formatValue(uint64_t value) const {
    switch (m_format) {
        case DisplayFormat::Bin:
            return QString("%1").arg(value, 64, 2, QChar('0'));
        case DisplayFormat::Hex:
            return QString("0x%1").arg(value, 16, 16, QChar('0'));
        case DisplayFormat::Dec:
            return QString::number(static_cast<int64_t>(value));
    }
    return QString::number(value);
}
