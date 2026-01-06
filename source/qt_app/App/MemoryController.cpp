#include "MemoryController.hpp"

MemoryController::MemoryController(Memory &memory, const Memory::Layout &layout, QObject *parent)
    : QObject(parent)
    , m_memory(memory)
    , m_layout(layout) {
}

int MemoryController::dataRowCount() const {
    return int((m_memory.get_data_size() + 15) / 16);
}

QString MemoryController::addressAt(const QString &baseHex, int offset) const {
    uint64_t base = parseHex(baseHex);
    return formatHex(base + offset);
}

int MemoryController::getByteAt(const QString &addrHex) const {
    MemErr err;
    uint64_t address = parseHex(addrHex);
    auto val = m_memory.load<uint8_t>(address, err);
    return err == MemErr::None ? int(val) : -1;
}

bool MemoryController::modifyByte(const QString &addrHex, int value) {
    if (!m_modificationAllowed || value < 0 || value > 255)
        return false;

    uint64_t address = parseHex(addrHex);
    if (m_memory.store(address, uint8_t(value)) != MemErr::None)
        return false;

    notifyContentChanged();
    return true;
}

namespace {
template<std::integral T>
bool parseAndStore(Memory &mem, uint64_t addr, const QString &str, int base = 10) {
    bool ok;
    T val = std::is_signed_v<T>
        ? T(str.toLongLong(&ok, base))
        : T(str.toULongLong(&ok, base));
    return ok && mem.store(addr, val) == MemErr::None;
}
}

bool MemoryController::modifyValue(const QString &addrHex, int typeIndex, const QString &valueStr) {
    if (!m_modificationAllowed)
        return false;

    uint64_t addr = parseHex(addrHex);
    bool ok = [&] {
        switch (typeIndex) {
            case 0: return parseAndStore<uint8_t>(m_memory, addr, valueStr, 2);
            case 1: return parseAndStore<int8_t>(m_memory, addr, valueStr);
            case 2: return parseAndStore<uint8_t>(m_memory, addr, valueStr);
            case 3: return parseAndStore<int16_t>(m_memory, addr, valueStr);
            case 4: return parseAndStore<uint16_t>(m_memory, addr, valueStr);
            case 5: return parseAndStore<int32_t>(m_memory, addr, valueStr);
            case 6: return parseAndStore<uint32_t>(m_memory, addr, valueStr);
            case 7: return parseAndStore<int64_t>(m_memory, addr, valueStr);
            case 8: return parseAndStore<uint64_t>(m_memory, addr, valueStr);
            default: return false;
        }
    }();

    if (!ok)
        return false;

    notifyContentChanged();
    return true;
}

void MemoryController::loadDataTypes(const QString &addrHex) {
    QVariantList values;
    MemErr err;
    uint64_t addr = parseHex(addrHex);

    auto u8 = m_memory.load<uint8_t>(addr, err);
    values.append(err == MemErr::None ? QString("%1").arg(u8, 8, 2, QChar('0')) : "N/A");
    values.append(err == MemErr::None ? QString::number(int8_t(u8)) : "N/A");
    values.append(err == MemErr::None ? QString::number(u8) : "N/A");

    auto i16 = m_memory.load<int16_t>(addr, err);
    values.append(err == MemErr::None ? QString::number(i16) : "N/A");

    auto u16 = m_memory.load<uint16_t>(addr, err);
    values.append(err == MemErr::None ? QString::number(u16) : "N/A");

    auto i32 = m_memory.load<int32_t>(addr, err);
    values.append(err == MemErr::None ? QString::number(i32) : "N/A");

    auto u32 = m_memory.load<uint32_t>(addr, err);
    values.append(err == MemErr::None ? QString::number(u32) : "N/A");

    auto i64 = m_memory.load<int64_t>(addr, err);
    values.append(err == MemErr::None ? QString::number(i64) : "N/A");

    auto u64 = m_memory.load<uint64_t>(addr, err);
    values.append(err == MemErr::None ? QString::number(u64) : "N/A");

    emit dataTypesLoaded(values);
}

void MemoryController::notifyContentChanged() {
    ++m_revision;
    emit contentChanged();
}
