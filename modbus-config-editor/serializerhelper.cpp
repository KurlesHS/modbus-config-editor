#include "serializerhelper.h"

#include "utils.h"

namespace  {
constexpr const char * descriptionKey = "description";
constexpr const char * writeRequestTtlKey = "write_request_ttl";
constexpr const char * upakServerUrlKey = "upak_server_url";
constexpr const char * upakUsernameKey = "upak_username";
constexpr const char * upakPasswordKey = "upak_password";
constexpr const char * addressKey = "address";
constexpr const char * valTypeKey = "val_type";
constexpr const char * valTypeOrderKey = "val_type_order";
constexpr const char * valCountKey = "val_count";
constexpr const char * startRegAddressKey = "start_reg_address";
constexpr const char * regAddressKey = "reg_address";
constexpr const char * regTypeKey = "reg_type";
constexpr const char * correctFuncKey = "correct_func";

constexpr const char * maxValKey = "max_val";
constexpr const char * minValKey = "min_val";
constexpr const char * mapIdKey = "map_id";
constexpr const char * defaultValKey = "default_val";
constexpr const char * mapOffsetKey = "map_offset";
constexpr const char * slaveAddrKey = "slave_addr";
constexpr const char * updateTresholdKey = "update_treshold";
constexpr const char * modeKey = "mode";
}

namespace ModbusConfig {

SerializerHelper::SerializerHelper(QJsonObject &obj) :
    mObj(&obj)
{

}

SerializerHelper::SerializerHelper(const QJsonObject &obj) :
    mCObj(&obj)
{

}

void SerializerHelper::setHelper(const QString &key, const QJsonValue &value)
{
    if (mObj) {
        (*mObj)[key] = value;
    }
}

QString SerializerHelper::getStringHelper(const QString &key) const
{
    return getHelper(key).toString();
}

void SerializerHelper::setStringHelper(const QString &key, const QString &value)
{
    if (!value.isEmpty()) {
        setHelper(key, value);
    }
}

QUuid SerializerHelper::getUuidHelper(const QString &key) const
{
    return toUuid(getHelper(key).toString());
}

void SerializerHelper::setUuidHelper(const QString &key, const QUuid &value)
{
    setHelper(key, toString(value));
}

namespace {
QString getRegAddressKey(RegisterAddressType type) {
    QString result;
    switch (type) {
    case RegisterAddressType::Map:
        result = startRegAddressKey;
        break;
    case RegisterAddressType::Sensor:
        result = ::regAddressKey;
        break;
    }
    return result;
}
}

RegisterAddress SerializerHelper::registerAddress(
    RegisterAddressType type, QString *errorString) const
{
    QString fakeErrorString;
    if (!errorString) {
        errorString = &fakeErrorString;
    }
    RegisterAddress address;
    int iValue = getHelper(slaveAddrKey).toInt(-1);
    if (iValue < 0 || iValue > 255) {
        *errorString = QObject::tr("Адрес слейва должен быть в диапазоне от 0 до 255 включительно");
        return address;
    }
    address.regAddress = static_cast<quint8>(iValue);
    address.regType = toRegisterType(getHelper(regTypeKey).toString());
    if (address.regType == RegisterAddress::RegisterType::Unknown) {
        *errorString = QObject::tr("Некорректный тип регистра");
        return address;
    }
    address.regAddress = getHelper(getRegAddressKey(type)).toInt(-1);
    *errorString = checkRegisterAddress(address.regType, address.regAddress);
    if (!errorString->isEmpty()) {
        return address;
    }
    address.slaveAddress = getHelper(slaveAddrKey).toInt(0);
    address.valType = toValueType(getHelper(valTypeKey).toString());
    if (address.valType == RegisterAddress::ValType::Unknown) {
        *errorString = QObject::tr("Некорректный тип значения регистра");
        return address;
    }
    address.typeOrder = getHelper(valTypeOrderKey).toString();
    return address;
}

void SerializerHelper::setRegisterAddress(RegisterAddressType type, const RegisterAddress &address)
{
    setHelper(slaveAddrKey, address.slaveAddress);
    if (!address.typeOrder.isEmpty()) {
        setHelper(valTypeOrderKey, address.typeOrder);
    }
    setHelper(getRegAddressKey(type), address.regAddress);
    setHelper(valTypeKey, toString(address.valType));
    setHelper(regTypeKey, toString(address.regType));
}

QString SerializerHelper::upakServerUrl() const
{
    return getStringHelper(upakServerUrlKey);
}

void SerializerHelper::setUpakServerUrl(const QString &url)
{
    setStringHelper(upakServerUrlKey, url);
}

QString SerializerHelper::upakServerPassword() const
{
    return getStringHelper(upakPasswordKey);
}

void SerializerHelper::setUpakServerPassword(const QString &password)
{
    setStringHelper(upakPasswordKey, password);
}

QString SerializerHelper::upakServerUsername() const
{
    return getStringHelper(upakUsernameKey);
}

void SerializerHelper::setUpakServerUsername(const QString &username)
{
    setStringHelper(upakUsernameKey, username);
}

void SerializerHelper::setWriteRequestTtl(int ttl)
{
    if (ttl > 0) {
        setHelper(writeRequestTtlKey, ttl);
    }
}

int SerializerHelper::writeRequestTtl() const
{
    return getHelper(writeRequestTtlKey).toInt(-1);
}

QString SerializerHelper::address() const
{
    return getStringHelper(addressKey);
}

void SerializerHelper::setAddress(const QString &address)
{
    setStringHelper(addressKey, address);
}

QString SerializerHelper::description() const
{
    return getStringHelper(descriptionKey);
}

void SerializerHelper::setDescription(const QString &description)
{
    setStringHelper(descriptionKey, description);
}

Sensor SerializerHelper::singleSensor(Sensor sensor, QString *errorString) const
{
    sensor.type = Sensor::Type::Separate;
    sensor.registerAddress = registerAddress(RegisterAddressType::Sensor, errorString);
    return sensor;
}

void SerializerHelper::setSingleSensor(const Sensor &sensor)
{
    setRegisterAddress(RegisterAddressType::Sensor, sensor.registerAddress);
}

Sensor SerializerHelper::sensor(QString *errorString) const
{
    Sensor result;
    QString mode = getStringHelper(modeKey).toLower();
    result.correctFunction = getStringHelper(correctFuncKey);
    result.mapId = getStringHelper(mapIdKey);
    result.mode = Sensor::Mode::Read;    
    result.updateThreshold = getHelper(updateTresholdKey).toDouble(0);
    result.description = getStringHelper(descriptionKey);
    if (mode == "w") {
        result.mode = Sensor::Mode::Write;
    } else if (mode == "rw") {
        result.mode = Sensor::Mode::ReadWrite;
    }
    auto minVal = getHelper(minValKey);
    auto maxVal = getHelper(maxValKey);
    if (minVal.isDouble()) {
        result.minValue = minVal;
    }
    if (maxVal.isDouble()) {
        result.maxValue = maxVal;
    }
    if (result.mapId.isEmpty()) {
        return singleSensor(result, errorString);
    }
    result.mapOffset = getHelper(mapOffsetKey).toInt();
    result.type = Sensor::Type::Map;
    return result;
}

void SerializerHelper::setSensor(const Sensor &sensor)
{
    if (!sensor.maxValue.isNull()) {
        setHelper(maxValKey, sensor.maxValue.toDouble());
    }

    if (!sensor.minValue.isNull()) {
        setHelper(minValKey, sensor.minValue.toDouble());
    }

    setStringHelper(descriptionKey, sensor.description);
    if (sensor.updateThreshold > 0) {
        setHelper(updateTresholdKey, sensor.updateThreshold);
    }
    setStringHelper(correctFuncKey, sensor.correctFunction);
    setStringHelper(modeKey, toString(sensor.mode));
    if (sensor.type == Sensor::Type::Separate) {
        setSingleSensor(sensor);
        return;
    }
    setStringHelper(mapIdKey, sensor.mapId);
    setHelper(mapOffsetKey, sensor.mapOffset);
}

SensorsMap SerializerHelper::sensorMap(QString *errorString) const
{
    QString fakeErrorString;
    if (!errorString) {
        errorString = &fakeErrorString;
    }
    SensorsMap result;
    result.registеrAddress = registerAddress(RegisterAddressType::Map, errorString);
    if (!errorString->isEmpty()) {
        return result;
    }
    result.valueCount = getHelper(valCountKey).toInt();
    result.defaultValue = getHelper(defaultValKey);    
    return result;

}

void SerializerHelper::setSensorMap(const SensorsMap &sensorMap)
{
    setRegisterAddress(RegisterAddressType::Map, sensorMap.registеrAddress);
    setHelper(valCountKey, sensorMap.valueCount);
    if (!sensorMap.defaultValue.isNull()) {
        setHelper(defaultValKey, sensorMap.defaultValue.toDouble());
    }
}

QJsonValue SerializerHelper::getHelper(const QString &key) const
{
    if (mObj) {
        return mObj->value(key);
    } else if (mCObj) {
        return mCObj->value(key);
    }
    return QJsonValue();
}

}
