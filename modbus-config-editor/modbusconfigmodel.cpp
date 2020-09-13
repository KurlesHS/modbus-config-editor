#include "modbusconfigmodel.h"

#include "utils.h"

#include <QList>
#include <QUrl>

namespace ModbusConfig {

ModbusConfigModel::ModbusConfigModel()
{
    clear();
}

QString ModbusConfigModel::upsertDevice(
    const QUuid &devId, const QUuid &prevDevId,
    const ConnectionParams &connectionParams, const QString &name)
{
    if (devId.isNull()) {
        return QObject::tr("Идентификатор устройства должен быть валидный UUID");
    }
    if (prevDevId.isNull() || prevDevId != devId) {
        if (mDevices.count(devId) != 0) {
            return QObject::tr(
                "Устройство с идентификатором %0 уже присутствует").arg(toString(devId));
        }
    }

    auto it = mDevices.find(prevDevId);
    if (it != mDevices.end()) {
        if (devId == prevDevId) {
            it.value().settings.description = name;
            it.value().settings.connectionParams = connectionParams;
            it.value().settings.id = devId;
        } else {
            auto dev = it.value();
            mDevices.erase(it);
            dev.settings.description = name;
            dev.settings.id = devId;
            dev.settings.connectionParams = connectionParams;
            mDevices[devId] = dev;
        }
    } else {
        Device dev;
        dev.settings.description = name;
        dev.settings.connectionParams = connectionParams;
        dev.settings.id = devId;
        mDevices[devId] = dev;
    }
    return {};
}

QString ModbusConfigModel::setCommonSettings(const Settings &settings)
{
    QUrl url = QUrl::fromUserInput(settings.upakServeUrl);
    if (!url.isValid()) {
        return QObject::tr("Невалидный адрес сервера УПАК");
    }
    mSettings = settings;
    return {};
}

void ModbusConfigModel::clear()
{
    mSettings.upakPassword = "default password";
    mSettings.upakUserName = "default username";
    mSettings.upakServeUrl = "http://localhost:8888";
    mSettings.writeRequestTtl = 180;
    mDevices.clear();
}

QString ModbusConfigModel::upsertSensor(
    const QUuid &devId, const QUuid &sensorId, const Sensor &sensor)
{
    if (sensor.id.isNull()) {
        return QObject::tr("Идентификатор датчика должен быть валидный UUID");
    }
    auto it = mDevices.find(devId);
    if (it == mDevices.end()) {
        return QObject::tr("Ошибка вставки / обновления датчика: "
                           "устройство с идентификатором %0 не найдено")
            .arg(toString(devId));
    }

    Device &dev = it.value();

    QString errorString = checkSensor(dev, sensor);
    if (!errorString.isEmpty()) {
        return errorString;
    }

    if (sensorId.isNull() || sensorId != sensor.id) {
        for (const auto &dev :qAsConst(mDevices)) {
            if (dev.sensors.count(sensor.id) != 0) {
                return QObject::tr(
                    "Датчик с идентификатором %0 уже присутствует").arg(toString(sensor.id));
            }
        }
    }

    auto sensorIt = dev.sensors.find(sensorId);
    if (sensorIt == dev.sensors.end()) {
        dev.sensors[sensor.id] = sensor;
        // insert
    } else {
        if (sensorId == sensor.id) {
            sensorIt.value() = sensor;
        } else {
            // update
            dev.sensors.erase(sensorIt);
            dev.sensors[sensor.id] = sensor;
        }
    }
    return {};
}

QString ModbusConfigModel::upsertSensorMap(
    const QUuid &devId, const QString mapId, const SensorsMap &map)
{
    auto it = mDevices.find(devId);
    if (it == mDevices.end()) {
        return QObject::tr("Ошибка вставки / обновления датчика: "
                           "устройство с идентификатором %0 не найдено")
            .arg(toString(devId));
    }
    Device &dev = it.value();
    if (map.id.isEmpty()) {
        return QObject::tr("Идентификатор карты датчиков не может быть пустым");
    }


    if (mapId.isEmpty() || mapId != map.id) {
        if (dev.maps.count(map.id) != 0) {
            return QObject::tr(
                "Карта регистров с идентификатором %0 уже присутствует").arg(map.id);
        }
    } 

    QList<decltype (dev.sensors.begin())> toRename;
    if (!mapId.isEmpty() && map.id != mapId) {
        // при переименовании идентификаторов карты регистров так же переименовывать эти
        // идентфиикаторы в привязанных датчиках
        for (auto it = dev.sensors.begin(); it != dev.sensors.end(); ++it) {
            if (it.value().type == Sensor::Type::Map && it.value().mapId == mapId) {
                toRename.append(it);
                if (it.value().mapOffset >= map.valueCount) {
                    return QObject::tr(
                        "Нельзя обновить карту регистров, так как количество занчений в ней меньше "
                        "используемого смещения привязанного датчика '%0'")
                        .arg(it.value().description);
                }
            }
        }
    }

    for (auto it : toRename) {
        it.value().mapId = map.id;
    }

    QString error = checkRegisterAddress(map.registеrAddress);
    if (!error.isEmpty()) {
        return error;
    }

    // TODO: проверить, что значение по умолчанию не выходит за диапазно типа значений карты регистров
    if (mapId != map.id) {
        dev.maps.remove(mapId);
    }
    dev.maps[map.id] = map;

    return {};
}

QString ModbusConfigModel::deleteSensor(const QUuid &devId, const QUuid &sensorId)
{
    auto it = mDevices.find(devId);
    if (it == mDevices.end()) {
        return QObject::tr("Ошибка удаления датчика: "
                           "устройство с идентификатором %0 не найдено").arg(toString(devId));
    }
    Device &dev = it.value();
    auto sensorIt = dev.sensors.find(sensorId);
    if (sensorIt == dev.sensors.end()) {
        return QObject::tr("Ошибка удаления датчика: "
                           "датчик с идентификатором %0 не найдено").arg(toString(sensorId));
    }
    dev.sensors.erase(sensorIt);
    return {};
}

QString ModbusConfigModel::deleteSensorMap(const QUuid &devId, const QString &mapId)
{
    auto it = mDevices.find(devId);
    if (it == mDevices.end()) {
        return QObject::tr("Ошибка удаления карты регистров: "
                           "устройство с идентификатором %0 не найдено").arg(toString(devId));
    }
    Device &dev = it.value();

    auto mapIt = dev.maps.find(mapId);
    if (mapIt == dev.maps.end()) {
        return QObject::tr("Ошибка удаления карты регистров: "
                           "карта регистров с идентификатором %0 не найдена").arg(mapId);
    }

    for (const auto &sensor : qAsConst(dev.sensors)) {
        if (sensor.type == Sensor::Type::Map && sensor.mapId == mapId) {
            return QObject::tr("Ошибка удаления карты регистров: "
                               "карта регистров привязана к датчику '%0'").arg(sensor.description);
        }
    }

    dev.maps.erase(mapIt);
    return {};
}

QString ModbusConfigModel::deleteDevice(const QUuid &devId)
{
    auto it = mDevices.find(devId);
    if (it == mDevices.end()) {
        return QObject::tr("Ошибка удаления устройства: "
                           "устройство с идентификатором %0 не найдено").arg(toString(devId));
    }
    mDevices.erase(it);
    return {};
}

QString ModbusConfigModel::checkSingleSensor(const Device &dev, const Sensor &sensor)
{
    Q_UNUSED(dev);
    QString error = checkRegisterAddress(sensor.registerAddress);
    if (!error.isEmpty()) {
        return error;
    }
    error = checkSensorMode(sensor.registerAddress.regType, sensor.mode);
    if (!error.isEmpty()) {
        return error;
    }
    return {};
}

QString ModbusConfigModel::checkMapSensor(const Device &dev, const Sensor &sensor)
{
    if (sensor.id.isNull()) {
        return QObject::tr("Идентификатор датчика должен быть валидным UUID");
    }
    if (sensor.mapId.isEmpty()) {
        return QObject::tr("Идентификатор карты датчиков не может быть пустым");
    }
    auto mapIt = dev.maps.find(sensor.mapId);

    if (mapIt == dev.maps.end()) {
        return QObject::tr("Карта регистров с идентфикатором '%0' отсутствует").arg(sensor.mapId);
    }
    if (sensor.mapOffset < 0) {
        return QObject::tr("Смещение в карте регистров не может быть отрицательным");
    }

    if (sensor.mapOffset >= mapIt.value().valueCount) {
        return QObject::tr(
            "Смещение в карте регистров должно быть меньше количества значений в карте (%0)")
            .arg(mapIt.value().valueCount);
    }

    return {};
}

QString ModbusConfigModel::checkSensor(const Device &dev, const Sensor &sensor)
{
    switch (sensor.type) {
    case Sensor::Type::Map:
        return checkMapSensor(dev, sensor);
    case Sensor::Type::Separate:
        return checkSingleSensor(dev, sensor);
    }
    return QObject::tr("Внутрення ошибка - непредвиденный тип датчика");
}

const Settings &ModbusConfigModel::commonSettings() const
{
    return mSettings;
}

QList<QUuid> ModbusConfigModel::devicesIds() const
{
    return mDevices.keys();
}

const Device &ModbusConfigModel::device(const QUuid &devId) const
{
    static Device fakeDevice;
    auto it = mDevices.find(devId);
    if (it == mDevices.end()) {
        return fakeDevice;
    }
    return it.value();
}

}
