#include "settingsmodel.h"

#include "utils.h"

namespace {

QStandardItem *createItem(
    const QString &name, ModbusConfig::SettingsModel::ItemType type, const QString &id=QString()) {
    auto result = new QStandardItem(name);
    result->setEditable(false);
    result->setData(toInt(type), toInt(ModbusConfig::SettingsModel::Roles::ItemTypeRole));
    if (!id.isEmpty()) {
        result->setData(id, toInt(ModbusConfig::SettingsModel::Roles::IdRole));
    }
    return result;
}
}

namespace ModbusConfig {

SettingsModel::SettingsModel(QObject *parent)
    : QStandardItemModel(parent)
    , mUpakSettingsItem(createItem(tr("Общие настройки"), ItemType::CommonType))
    , mModbusSettingsItem(createItem(tr("Настройки modbus"), ItemType::ModbusRootSettings))
{
    appendRow(mUpakSettingsItem);
    appendRow(mModbusSettingsItem);
}

SettingsModel::ItemType SettingsModel::itemType(QStandardItem *item) const
{
    if (!item) {
        return ItemType::NoneType;
    }
    return static_cast<ItemType>(item->data(toInt(Roles::ItemTypeRole)).toInt());
}

QUuid SettingsModel::getDeviceId(QStandardItem *deviceItem) const
{
    return toUuid(getIdHelper(deviceItem, ItemType::ModbusDevice));
}

QString SettingsModel::updateDeviceSettings(
    const QUuid &prevId, const QUuid &newId, const QString &name)
{
    auto item = deviceItem(prevId);
    if (!item) {
        return tr("Не найдено устройство с идентификатором '%0'").arg(toString(prevId));
    }
    item->setText(name);
    item->setData(toString(newId), toInt(Roles::IdRole));
    return {};
}

QString SettingsModel::updateSensorMapSettings(const QUuid &devId, const QString &prevId, const QString &newId)
{
    auto item = deviceItem(devId);
    if (!item) {
        return tr("Не найдено устройство с идентификатором '%0'").arg(toString(prevId));
    }

    auto mapRootItem = item->child(0);
    auto cnt = mapRootItem->rowCount();
    for (int r = 0; r < cnt; ++r) {
        auto item = mapRootItem->child(r);
        if (getIdHelper(item, ItemType::SensorMap) == prevId) {
            item->setText(newId);
            item->setData(newId, toInt(Roles::IdRole));
            return {};
        }
    }

    return tr("Не найдена карта регистров %0 у устройства c Id = %1")
    .arg(toString(devId)).arg(prevId);
}

QString SettingsModel::updateSensorSettings(
    const QUuid &devId, const QUuid &prevId, const QUuid &newId, const QString &name)
{
    auto item = deviceItem(devId);
    if (!item) {
        return tr("Не найдено устройство с идентификатором '%0'").arg(toString(prevId));
    }

    auto sensorRootItem = item->child(1);
    auto cnt = sensorRootItem->rowCount();
    for (int r = 0; r < cnt; ++r) {
        auto item = sensorRootItem->child(r);
        if (toUuid(getIdHelper(item, ItemType::Sensor)) == prevId) {
            item->setText(name);
            item->setData(newId, toInt(Roles::IdRole));
            return {};
        }
    }

    return tr("Не найден датчик %0 у устройства c Id = %1")
        .arg(toString(devId)).arg(toString(prevId));
}

QStandardItem *SettingsModel::addDevice(const QUuid &id, const QString &name)
{
    auto devItem = createItem(name, ItemType::ModbusDevice, toString(id));
    mModbusSettingsItem->appendRow(devItem);
    auto mapsItem = createItem(tr("Карты регистров"), ItemType::SensorMapsRoot);
    devItem->appendRow(mapsItem);
    auto sensorsItem = createItem(tr("Датчики"), ItemType::SensorsRoot);
    devItem->appendRow(sensorsItem);
    return devItem;
}

void SettingsModel::deleteDevice(const QUuid &id)
{
    auto item = deviceItem(id);
    if (item) {
        mModbusSettingsItem->removeRow(item->row());
    }
}

QStandardItem *SettingsModel::addSensorMap(const QUuid &devId, const QString &sensorId)
{
    auto devItem = deviceItem(devId);
    if (!devItem) {
        return {};
    }
    auto item = createItem(sensorId, ItemType::SensorMap, sensorId);
    devItem->child(0)->appendRow(item);
    return item;
}

QStandardItem *SettingsModel::addSensor(
    const QUuid &devId, const QUuid &sensorId, const QString &name)
{
    auto devItem = deviceItem(devId);
    if (!devItem) {
        return {};
    }
    auto item = createItem(name, ItemType::Sensor, toString(sensorId));
    devItem->child(1)->appendRow(item);
    return item;
}

QPair<QUuid, QString> SettingsModel::getDeviceIinfoBySensorsMapRootItem(QStandardItem *item)
{
    if (!item || !item->parent()) {
        return {};
    }
    return {getDeviceId(item->parent()), item->parent()->text()};
}

QPair<QUuid, QString> SettingsModel::getDeviceInfoBySensorRootItem(QStandardItem *item)
{
    return getDeviceIinfoBySensorsMapRootItem(item);
}

QUuid SettingsModel::getSensorIdBySensorItem(QStandardItem *item)
{
    return toUuid(getIdHelper(item, ItemType::Sensor));
}

QString SettingsModel::getSensorsMapIdBySensorsMapItem(QStandardItem *item)
{
    return getIdHelper(item, ItemType::SensorMap);
}

void SettingsModel::deleteSensor(const QUuid &devId, const QUuid &sensorId)
{
    auto devItem = deviceItem(devId);
    if (!devItem) {
        return;
    }
    auto sensorRootItem = devItem->child(1);
    if (!sensorRootItem) {
        return;
    }
    int cnt = sensorRootItem->rowCount();
    for (int r = 0; r < cnt; ++r) {
        auto item = sensorRootItem->child(r);
        if (toUuid(getIdHelper(item, ItemType::Sensor)) == sensorId) {
            sensorRootItem->removeRow(r);
            return;
        }
    }
}

void SettingsModel::deleteSensorMap(const QUuid &devId, const QString &sensorMapId)
{
    auto devItem = deviceItem(devId);
    if (!devItem) {
        return;
    }
    auto sensorMapRootItem = devItem->child(0);
    if (!sensorMapRootItem) {
        return;
    }
    int cnt = sensorMapRootItem->rowCount();
    for (int r = 0; r < cnt; ++r) {
        auto item = sensorMapRootItem->child(r);
        if (getIdHelper(item, ItemType::SensorMap) == sensorMapId) {
            sensorMapRootItem->removeRow(r);
            return;
        }
    }
}

QStringList SettingsModel::sensorMapsForDevice(const QUuid &devId)
{
    QStringList result;
    auto devInfo = deviceItem(devId);
    if (!devInfo) {
        return result;
    }
    auto mapsRoot = devInfo->child(0);
    if (!mapsRoot) {
        return result;
    }
    int cnt = mapsRoot->rowCount();
    for (int r = 0; r < cnt; ++r) {
        result.append(mapsRoot->child(r)->text());
    }
    return result;
}

int SettingsModel::deviceCount() const
{
    return mModbusSettingsItem->rowCount();
}

QStandardItem *SettingsModel::modbusSettingsItem() const
{
    return mModbusSettingsItem;
}

QStandardItem *SettingsModel::deviceItem(const QUuid &id)
{
    int count = mModbusSettingsItem->rowCount();
    for (int r = 0; r < count; ++r) {
        auto item = mModbusSettingsItem->child(r);
        if (getDeviceId(item) == id) {
            return item;
        }
    }
    return nullptr;
}

QString SettingsModel::getIdHelper(
    QStandardItem *deviceItem, SettingsModel::ItemType itemType) const
{
    if (!deviceItem) {
        return {};
    }
    if (deviceItem->data(toInt(Roles::ItemTypeRole)).toInt() != toInt(itemType)) {
        return {};
    }
    return deviceItem->data(toInt(Roles::IdRole)).toString();
}

}
