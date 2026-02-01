#include "MemoryController.hpp"
#include "endianness.hpp"

MemoryController::MemoryController(Memory &memory, QObject *parent)
    : QObject(parent)
    , m_memory(memory)
     {
}

int MemoryController::dataRowCount() const {
    return int((m_memory.get_data_size() + 15) / 16);
}

void MemoryController::notifyLayoutChanged() {
    notifyContentChanged();
    emit layoutChanged();
}

QString MemoryController::addressAt(const QString &baseHex, quint64 offset) const {
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

    // Read raw bytes in visual order (left-to-right = low to high address)
    uint8_t bytes[8] = {};
    bool ok[8] = {};
    for (int i = 0; i < 8; ++i) {
        bytes[i] = m_memory.load<uint8_t>(addr + i, err);
        ok[i] = (err == MemErr::None);
    }

    // Interpret bytes according to endianness setting
    // Little-endian: bytes[0] is LSB (no swap on LE platform)
    // Big-endian: bytes[0] is MSB (swap on LE platform)
    auto u16 = *reinterpret_cast<uint16_t*>(bytes);
    auto u32 = *reinterpret_cast<uint32_t*>(bytes);
    auto u64 = *reinterpret_cast<uint64_t*>(bytes);
    if (!isLittleEndian()) {
        u16 = endianness::swap_on_le_platform(u16);
        u32 = endianness::swap_on_le_platform(u32);
        u64 = endianness::swap_on_le_platform(u64);
    }

    // bin8, i8, u8
    values.append(ok[0] ? QString("%1").arg(bytes[0], 8, 2, QChar('0')) : "N/A");
    values.append(ok[0] ? QString::number(int8_t(bytes[0])) : "N/A");
    values.append(ok[0] ? QString::number(bytes[0]) : "N/A");

    // i16, u16
    bool ok16 = ok[0] && ok[1];
    values.append(ok16 ? QString::number(int16_t(u16)) : "N/A");
    values.append(ok16 ? QString::number(u16) : "N/A");

    // i32, u32
    bool ok32 = ok[0] && ok[1] && ok[2] && ok[3];
    values.append(ok32 ? QString::number(int32_t(u32)) : "N/A");
    values.append(ok32 ? QString::number(u32) : "N/A");

    // i64, u64
    bool ok64 = ok[0] && ok[1] && ok[2] && ok[3] && ok[4] && ok[5] && ok[6] && ok[7];
    values.append(ok64 ? QString::number(int64_t(u64)) : "N/A");
    values.append(ok64 ? QString::number(u64) : "N/A");

    emit dataTypesLoaded(values);
}

void MemoryController::notifyContentChanged() {
    ++m_revision;
    emit contentChanged();
}
