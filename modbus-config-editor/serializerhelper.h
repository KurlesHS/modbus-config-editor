#pragma once

#include <QJsonObject>
#include <QUuid>

#include "modbusentities.h"

namespace ModbusConfig {

enum class RegisterAddressType {
    Map,
    Sensor
};

class SerializerHelper
{
public:

    SerializerHelper(QJsonObject &obj);
    SerializerHelper(const QJsonObject &obj);

    RegisterAddress registerAddress(RegisterAddressType type, QString *errorString = nullptr) const;
    void setRegisterAddress(RegisterAddressType type, const RegisterAddress &address);

    QString upakServerUrl() const;
    void setUpakServerUrl(const QString &url);

    QString upakServerPassword() const;
    void setUpakServerPassword(const QString &password);

    QString upakServerUsername() const;
    void setUpakServerUsername(const QString &username);

    void setWriteRequestTtl(int ttl);
    int writeRequestTtl() const;

    QString address() const;
    void setAddress(const QString &address);

    QString description() const;
    void setDescription(const QString &description);

    Sensor sensor(QString *errorString = nullptr) const;
    void setSensor(const Sensor &sensor);

    SensorsMap sensorMap(QString *errorString = nullptr) const;
    void setSensorMap(const SensorsMap &sensorMap);


private:
    QJsonValue getHelper(const QString &key) const;
    void setHelper(const QString &key, const QJsonValue &value);

    QString getStringHelper(const QString &key) const;
    void setStringHelper(const QString &key, const QString &value);

    QUuid getUuidHelper(const QString &key) const;
    void setUuidHelper(const QString &key, const QUuid &value);

    Sensor singleSensor(Sensor sensor, QString *errorString = nullptr) const;
    void setSingleSensor(const Sensor &sensor);

private:
    QJsonObject *mObj{};
    const QJsonObject *mCObj{};
};

}
