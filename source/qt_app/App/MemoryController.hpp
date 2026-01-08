#pragma once

#include <QObject>
#include <QVariantList>
#include "Memory.hpp"

class MemoryController : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString dataBaseAddress READ dataBaseAddress NOTIFY layoutChanged)
    Q_PROPERTY(QString stackBaseAddress READ stackBaseAddress NOTIFY layoutChanged)
    Q_PROPERTY(int dataRowCount READ dataRowCount NOTIFY contentChanged)
    Q_PROPERTY(int stackRowCount READ stackRowCount NOTIFY contentChanged)
    Q_PROPERTY(int revision READ revision NOTIFY contentChanged)
    Q_PROPERTY(bool isLittleEndian READ isLittleEndian NOTIFY layoutChanged)

public:
    explicit MemoryController(Memory &memory, QObject *parent = nullptr);

    QString dataBaseAddress() const { return formatHex(layout().data_base); }
    QString stackBaseAddress() const { return formatHex(layout().stack_base); }
    int dataRowCount() const;
    int stackRowCount() const { return int(layout().stack_size / 16); }
    int revision() const { return m_revision; }
    bool isLittleEndian() const { return layout().endianness == std::endian::little; }


    // Address helper - returns hex string for (base + offset)
    Q_INVOKABLE QString addressAt(const QString &baseHex, int offset) const;

    Q_INVOKABLE int getByteAt(const QString &addrHex) const;
    Q_INVOKABLE bool modifyByte(const QString &addrHex, int value);
    Q_INVOKABLE bool modifyValue(const QString &addrHex, int typeIndex, const QString &valueStr);
    Q_INVOKABLE void loadDataTypes(const QString &addrHex);

    static QString formatHex(uint64_t addr) {
        return QString::number(addr, 16).toUpper().rightJustified(16, '0');
    }
    static uint64_t parseHex(const QString &hex) {
        return hex.toULongLong(nullptr, 16);
    }

    void notifyContentChanged();
    void notifyLayoutChanged();
    void setModificationAllowed(bool allowed) { m_modificationAllowed = allowed; }

signals:
    void layoutChanged();
    void contentChanged();
    void dataTypesLoaded(QVariantList values);

private:
    const Memory::Layout& layout() const { return m_memory.get_layout(); }

private:
    Memory &m_memory;
    int m_revision = 0;
    bool m_modificationAllowed = false;
};
