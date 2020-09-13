#pragma once

#include <QStandardItemModel>

namespace ModbusConfig {

class SettingsModel : public QStandardItemModel
{
    Q_OBJECT

public:
    enum  class Roles {
        ItemTypeRole = Qt::UserRole + 1,
        IdRole
    };

    enum class ItemType {
        NoneType,
        CommonType,
        ModbusRootSettings,
        ModbusDevice,
        SensorMap,
        Sensor,
        SensorMapsRoot,
        SensorsRoot
    };

public:
    SettingsModel(QObject *parent=nullptr);

    ItemType itemType(QStandardItem *item) const;
    QUuid getDeviceId(QStandardItem *deviceItem) const;
    QString updateDeviceSettings(const QUuid &prevId, const QUuid &newId, const QString &name);
    QString updateSensorMapSettings(const QUuid &devId, const QString &prevId, const QString &newId);
    QString updateSensorSettings(
        const QUuid &devId, const QUuid &prevId, const QUuid &newId, const QString &name);

    QStandardItem *addDevice(const QUuid &id, const QString &name);
    void deleteDevice(const QUuid &id);

    QStandardItem *addSensorMap(const QUuid &devId, const QString &sensorId);
    QStandardItem *addSensor(const QUuid &devId, const QUuid &sensorId, const QString &name);

    QPair<QUuid, QString> getDeviceIinfoBySensorsMapRootItem(QStandardItem *item);
    QPair<QUuid, QString> getDeviceInfoBySensorRootItem(QStandardItem *item);

    QUuid getSensorIdBySensorItem(QStandardItem *item);
    QString getSensorsMapIdBySensorsMapItem(QStandardItem *item);

    void deleteSensor(const QUuid &devId, const QUuid &sensorId);
    void deleteSensorMap(const QUuid &devId, const QString &sensorMapId);

    QStringList sensorMapsForDevice(const QUuid &devId);

    int deviceCount() const;

    QStandardItem *modbusSettingsItem() const;

private:
    QStandardItem *deviceItem(const QUuid &id);
    QString getIdHelper(QStandardItem *deviceItem, ItemType itemType) const;


private:
    QStandardItem *mUpakSettingsItem;
    QStandardItem *mModbusSettingsItem;
};

}
