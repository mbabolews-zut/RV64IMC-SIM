#include "SettingsManager.hpp"
#include "Backend.hpp"

SettingsManager::SettingsManager(Backend *parent)
    : QObject(parent), m_backend(parent) {
    Q_ASSERT(parent != nullptr);
    loadFromVM();
}

rv64::VMConfig SettingsManager::createVMConfig() const {
    rv64::VMConfig conf;
    conf.m_mem_layout = buildMemLayout();
    conf.m_sp_pos = m_systemConfig.spPos;
    return conf;
}

int SettingsManager::stackSizeKiB() const {
    return static_cast<int>(m_tmpMemoryConfig.stackSize / 1024);
}

void SettingsManager::setStackSizeKiB(int sizeKiB) {
    size_t newSize = static_cast<size_t>(sizeKiB) * 1024;
    if (m_tmpMemoryConfig.stackSize != newSize) {
        m_tmpMemoryConfig.stackSize = newSize;
        emit stackSizeKiBChanged();
    }
}

QString SettingsManager::dataAddress() const {
    return toHexString(m_tmpMemoryConfig.dataAddr);
}

void SettingsManager::setDataAddress(const QString &addr) {
    bool ok;
    uint64_t newAddr = fromHexString(addr, &ok);
    if (ok && m_tmpMemoryConfig.dataAddr != newAddr) {
        m_tmpMemoryConfig.dataAddr = newAddr;
        emit dataAddressChanged();
    }
}

QString SettingsManager::stackAddress() const {
    return toHexString(m_tmpMemoryConfig.stackAddr);
}

void SettingsManager::setStackAddress(const QString &addr) {
    bool ok;
    uint64_t newAddr = fromHexString(addr, &ok);
    if (ok && m_tmpMemoryConfig.stackAddr != newAddr) {
        m_tmpMemoryConfig.stackAddr = newAddr;
        emit stackAddressChanged();
    }
}

int SettingsManager::spPosIndex() const {
    return static_cast<int>(m_tmpSystemConfig.spPos);
}

void SettingsManager::setSpPosIndex(int index) {
    auto newPos = static_cast<rv64::SpPos>(index);
    if (m_tmpSystemConfig.spPos != newPos) {
        m_tmpSystemConfig.spPos = newPos;
        emit spPosIndexChanged();
    }
}

int SettingsManager::endiannessIndex() const {
    return m_tmpSystemConfig.endianness == std::endian::little ? 0 : 1;
}

void SettingsManager::setEndiannessIndex(int index) {
    auto newEndian = index == 0 ? std::endian::little : std::endian::big;
    if (m_tmpSystemConfig.endianness != newEndian) {
        m_tmpSystemConfig.endianness = newEndian;
        emit endiannessIndexChanged();
    }
}

void SettingsManager::loadFromVM() {
    const auto &vmConfig = vm().get_config();
    const auto &memLayout = vm().get_memory_layout();

    m_memoryConfig.dataAddr = memLayout.data_base;
    m_memoryConfig.stackAddr = memLayout.stack_base;
    m_memoryConfig.stackSize = memLayout.stack_size;

    m_systemConfig.spPos = vmConfig.m_sp_pos;
    m_systemConfig.endianness = memLayout.endianness;

    // Copy to temp config
    m_tmpMemoryConfig = m_memoryConfig;
    m_tmpSystemConfig = m_systemConfig;

    emit stackSizeKiBChanged();
    emit dataAddressChanged();
    emit stackAddressChanged();
    emit spPosIndexChanged();
    emit endiannessIndexChanged();
}

Memory::Layout SettingsManager::buildMemLayout() const {
    Memory::Layout layout;
    layout.data_base = m_tmpMemoryConfig.dataAddr;
    layout.stack_base = m_tmpMemoryConfig.stackAddr;
    layout.stack_size = m_tmpMemoryConfig.stackSize;
    layout.endianness = m_tmpSystemConfig.endianness;
    return layout;
}

QString SettingsManager::apply() {
    auto validationError = Memory::validate_layout(buildMemLayout());
    if (validationError.has_value()) {
        m_backend->appendError(QString::fromStdString(*validationError));
        return QString::fromStdString(*validationError);
    }

    m_memoryConfig = m_tmpMemoryConfig;
    m_systemConfig = m_tmpSystemConfig;
    emit settingsApplied();
    return {};
}

void SettingsManager::cancel() {
    m_tmpMemoryConfig = m_memoryConfig;
    m_tmpSystemConfig = m_systemConfig;

    emit stackSizeKiBChanged();
    emit dataAddressChanged();
    emit stackAddressChanged();
    emit spPosIndexChanged();
    emit endiannessIndexChanged();
}

QString SettingsManager::toHexString(uint64_t value) {
    return QString("0x%1").arg(value, 0, 16);
}

uint64_t SettingsManager::fromHexString(const QString &str, bool *ok) {
    QString s = str.trimmed();
    if (s.startsWith("0x") || s.startsWith("0X")) {
        s = s.mid(2);
    }
    return s.toULongLong(ok, 16);
}

const rv64::VM& SettingsManager::vm() const {
    return m_backend->vm();
}
