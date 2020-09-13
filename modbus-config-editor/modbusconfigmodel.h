#pragma once

#include "modbusentities.h"

namespace ModbusConfig {

class ModbusConfigModel
{
public:
    ModbusConfigModel();
    QString setCommonSettings(const Settings &settings);

    void clear();

    QString upsertDevice(
        const QUuid &devId, const QUuid &prevDevId, const ConnectionParams &connectionParams,
        const QString &name);

    QString upsertSensor(const QUuid &devId, const QUuid &sensorId, const Sensor &sensor);
    QString upsertSensorMap(const QUuid &devId, const QString mapId, const SensorsMap &map);

    QString deleteDevice(const QUuid &devId);
    QString deleteSensor(const QUuid &devId, const QUuid &sensorId);
    QString deleteSensorMap(const QUuid &devId, const QString &mapId);

    const Settings &commonSettings() const;
    QList<QUuid> devicesIds() const;

    const Device &device(const QUuid &devId) const;

private:
    QString checkSingleSensor(const Device &dev, const Sensor &sensor);
    QString checkMapSensor(const Device &dev, const Sensor &sensor);
    QString checkSensor(const Device &dev, const Sensor &sensor);


private:
    Settings mSettings;
    QHash<QUuid, Device> mDevices;
};

}
