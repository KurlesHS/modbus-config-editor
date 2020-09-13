#include "serializer.h"

#include "utils.h"
#include "serializerhelper.h"

namespace  {
constexpr const char * sensorsMapKey = "sensors_map";
constexpr const char * sensorsKey = "sensors";
constexpr const char * settingsKey = "settings";
}

namespace ModbusConfig {

QJsonObject Serializer::serialize(const ModbusConfigModel &model)
{
    QJsonObject root;

    SerializerHelper helper(root);
    helper.setUpakServerUrl(model.commonSettings().upakServeUrl);
    helper.setUpakServerUsername(model.commonSettings().upakUserName);
    helper.setUpakServerPassword(model.commonSettings().upakPassword);
    helper.setWriteRequestTtl(model.commonSettings().writeRequestTtl);

    QJsonObject settingsObj;
    auto devicesIds = model.devicesIds();
    for (const auto &devId : qAsConst(devicesIds)) {
        const auto device = model.device(devId);
        QJsonObject deviceObj;
        SerializerHelper helper(deviceObj);
        helper.setAddress(toString(device.settings.connectionParams));
        helper.setDescription(device.settings.description);

        QJsonObject sensorsObj;
        for (auto it = device.sensors.begin(); it != device.sensors.end(); ++it) {
            QJsonObject sensorObj;
            SerializerHelper helper(sensorObj);
            helper.setSensor(it.value());
            sensorsObj[toString(it.key())] = sensorObj;
        }

        if (!sensorsObj.isEmpty()) {
            deviceObj[sensorsKey] = sensorsObj;
        }

        QJsonObject sensorMapsObj;
        for (auto it = device.maps.begin(); it != device.maps.end(); ++it) {
            QJsonObject mapObj;
            SerializerHelper helper(mapObj);
            helper.setSensorMap(it.value());
            sensorMapsObj[it.key()] = mapObj;
        }

        if (!sensorMapsObj.isEmpty()) {
            deviceObj[sensorsMapKey] = sensorMapsObj;
        }
        if (!deviceObj.isEmpty()) {
            settingsObj[toString(devId)] = deviceObj;
        }
    }

    if (!settingsObj.isEmpty()) {
        root[settingsKey] = settingsObj;
    }
    return root;
}

ModbusConfigModel Serializer::deserialize(const QJsonObject &root, QString *error)
{
    QString fakeError;
    if (!error) {
        error = &fakeError;
    }
    ModbusConfigModel result;

    Settings settings;
    SerializerHelper helper(root);
    settings.upakPassword = helper.upakServerPassword();
    settings.upakUserName = helper.upakServerUsername();
    settings.upakServeUrl = helper.upakServerUrl();
    settings.writeRequestTtl = helper.writeRequestTtl();
    result.setCommonSettings(settings);

    const QJsonObject settingsObj = root.value(settingsKey).toObject();

    for (auto it = settingsObj.begin(); it != settingsObj.end(); it++) {
        auto devId = toUuid(it.key());
        if (devId.isNull()) {
            *error = QObject::tr("Идентификатор устройсва должен быть валидным UUID");
            return {};
        }
        auto deviceObj = it.value().toObject();
        SerializerHelper helper(deviceObj);

        auto connectionParams = toConnectionParams(helper.address(), toString(devId), error);
        auto description = helper.description();
        if (!error->isEmpty()) {
            return {};
        }

        result.upsertDevice(devId, {}, connectionParams, description);

        auto sensorsMapsObj = deviceObj.value(sensorsMapKey).toObject();
        auto sensorsObj = deviceObj.value(sensorsKey).toObject();

        for (auto it = sensorsMapsObj.begin(); it != sensorsMapsObj.end(); ++it) {
            auto mapObj = it.value().toObject();
            SerializerHelper helper(mapObj);
            auto map = helper.sensorMap(error);
            if (!error->isEmpty()) {
                return {};
            }
            map.id = it.key();
            *error = result.upsertSensorMap(devId, {}, map);
            if (!error->isEmpty()) {
                return {};
            }
        }

        for (auto it = sensorsObj.begin(); it != sensorsObj.end(); ++it) {
            auto sensorObj = it.value().toObject();
            SerializerHelper helper(sensorObj);
            auto sensor = helper.sensor(error);
            if (!error->isEmpty()) {
                return {};
            }
            sensor.id = it.key();
            *error = result.upsertSensor(devId, {}, sensor);
            if (!error->isEmpty()) {
                return {};
            }
        }
    }

    return result;
}

}
