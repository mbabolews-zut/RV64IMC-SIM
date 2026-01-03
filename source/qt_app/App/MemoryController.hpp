#pragma once

#include <QObject>
#include <QVariantList>
#include "Memory.hpp"

class MemoryController : public QObject {
    Q_OBJECT

    Q_PROPERTY(uint64_t dataBaseAddress READ dataBaseAddress NOTIFY layoutChanged)
    Q_PROPERTY(uint64_t stackBaseAddress READ stackBaseAddress NOTIFY layoutChanged)
    Q_PROPERTY(int dataRowCount READ dataRowCount NOTIFY contentChanged)
    Q_PROPERTY(int stackRowCount READ stackRowCount NOTIFY contentChanged)
    Q_PROPERTY(int revision READ revision NOTIFY contentChanged)

public:
    explicit MemoryController(Memory &memory, const Memory::Layout &layout, QObject *parent = nullptr);

    uint64_t dataBaseAddress() const { return m_layout.data_base; }
    uint64_t stackBaseAddress() const { return m_layout.stack_base; }
    int dataRowCount() const;
    int stackRowCount() const { return int(m_layout.stack_size / 16); }
    int revision() const { return m_revision; }

    Q_INVOKABLE int getByte(uint64_t address) const;
    Q_INVOKABLE bool modifyByte(uint64_t address, int value);
    Q_INVOKABLE bool modifyValue(uint64_t address, int typeIndex, const QString &valueStr);
    Q_INVOKABLE void loadDataTypes(uint64_t address);

    void notifyContentChanged();
    void setModificationAllowed(bool allowed) { m_modificationAllowed = allowed; }

signals:
    void layoutChanged();
    void contentChanged();
    void dataTypesLoaded(QVariantList values);

private:
    Memory &m_memory;
    Memory::Layout m_layout;
    int m_revision = 0;
    bool m_modificationAllowed = false;
};
