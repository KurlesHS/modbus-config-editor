#include "modbusconfigeditorcontroller.h"

#include "utils.h"

#include <QDebug>
#include <QJsonDocument>

#include "serializer.h"

namespace ModbusConfig {

ModbusConfigEditorController::ModbusConfigEditorController(
    ModbusConfigEditorMainWindow *modbusConfigEditorMainWindow,
    ModbusConfigModel *modbusConfigModel,
    QObject *parent)
    : QObject(parent)
    , mModbusConfigEditorMainWindow(modbusConfigEditorMainWindow)
    , mModbusConfigModel(modbusConfigModel)
{
    connect(mModbusConfigEditorMainWindow, &ModbusConfigEditorMainWindow::upakSettingRequest,
        this, &ModbusConfigEditorController::onUpakSettingRequest);
    connect(mModbusConfigEditorMainWindow, &ModbusConfigEditorMainWindow::addModbusDeviceRequest,
        this, &ModbusConfigEditorController::onAddModbusDeviceRequest);
    connect(mModbusConfigEditorMainWindow, &ModbusConfigEditorMainWindow::deleteDeviceRequest,
        this, &ModbusConfigEditorController::onDeleteDeviceRequest);
    connect(mModbusConfigEditorMainWindow, &ModbusConfigEditorMainWindow::deviceSettingsRequest,
        this, &ModbusConfigEditorController::onDeviceSettingsRequest);
    connect(mModbusConfigEditorMainWindow, &ModbusConfigEditorMainWindow::addRegisterMapRequest,
        this, &ModbusConfigEditorController::onAddRegisterMapRequest);
    connect(mModbusConfigEditorMainWindow, &ModbusConfigEditorMainWindow::addSensorRequest,
        this, &ModbusConfigEditorController::onAddSensorRequest);
    connect(mModbusConfigEditorMainWindow, &ModbusConfigEditorMainWindow::sensorSettingsRequest,
        this, &ModbusConfigEditorController::onSensorSettingsRequest);
    connect(mModbusConfigEditorMainWindow, &ModbusConfigEditorMainWindow::sensorsMapSettingsRequest,
        this, &ModbusConfigEditorController::onSensorsMapSettingsRequest);
    connect(mModbusConfigEditorMainWindow, &ModbusConfigEditorMainWindow::upakSettingsChanged,
        this, &ModbusConfigEditorController::onUpakSettingsChanged);
    connect(
        mModbusConfigEditorMainWindow, &ModbusConfigEditorMainWindow::modbusDeviceSettingsChanged,
        this, &ModbusConfigEditorController::onModbusDeviceSettingsChanged);
    connect(mModbusConfigEditorMainWindow, &ModbusConfigEditorMainWindow::sensorMapSettingsChanged,
        this, &ModbusConfigEditorController::onSensorMapSettingsChanged);
    connect(mModbusConfigEditorMainWindow, &ModbusConfigEditorMainWindow::sensorSettingsChanged,
        this, &ModbusConfigEditorController::onSensorSettingsChanged);
    connect(mModbusConfigEditorMainWindow, &ModbusConfigEditorMainWindow::showJsonRequest,
        this, &ModbusConfigEditorController::onShowJsonRequest);
    connect(mModbusConfigEditorMainWindow, &ModbusConfigEditorMainWindow::deleteSensorMapRequest,
        this, &ModbusConfigEditorController::onDeleteSensorMapRequest);
    connect(mModbusConfigEditorMainWindow, &ModbusConfigEditorMainWindow::deleteSensorRequest,
        this, &ModbusConfigEditorController::onDeleteSensorRequest);
}

void ModbusConfigEditorController::onUpakSettingRequest()
{
    mModbusConfigEditorMainWindow->setCommonSetting(mModbusConfigModel->commonSettings());
}

void ModbusConfigEditorController::onUpakSettingsChanged(const Settings &settings)
{
    mModbusConfigEditorMainWindow->setError(mModbusConfigModel->setCommonSettings(settings));
}

void ModbusConfigEditorController::onAddModbusDeviceRequest()
{
    QUuid devId = QUuid::createUuid();
    QString name = tr("Устрйстово modbus %0").arg(toString(devId).right(0x08));
    ConnectionParams params;
    params.type = ConnectionParams::Type::RtuSerial;
    params.address = "modbus.address";
    params.deviceName = "/dev/ttyUSB0";
    params.port = 54363;
    params.baudrate = 19200;
    params.stopBits = ConnectionParams::StopBits::OneStop;
    params.parity = ConnectionParams::Parity::NoParity;
    params.flowControl = ConnectionParams::FlowControl::NoFlowControl;
    params.databits = 8;

    QString error = mModbusConfigModel->upsertDevice(devId, {}, params, name);
    mModbusConfigEditorMainWindow->setError(error);
    if (error.isEmpty()) {
        mModbusConfigEditorMainWindow->addDevice(devId, name);
        onShowJsonRequest();
    }
}

void ModbusConfigEditorController::onAddRegisterMapRequest(const QUuid &devId)
{
    SensorsMap map;
    map.id = QString("map_%0").arg(toString(QUuid::createUuid()).right(0x08));
    map.defaultValue = 0;
    map.valueCount = 0x08;
    map.registеrAddress.regAddress = 40001;
    map.registеrAddress.regType = RegisterAddress::RegisterType::AnalogOutputHoldingRegisters;
    map.registеrAddress.slaveAddress = 1;
    map.registеrAddress.valType = RegisterAddress::ValType::Int32;
    map.registеrAddress.typeOrder = "2143";
    auto error = mModbusConfigModel->upsertSensorMap(devId, {}, map);
    mModbusConfigEditorMainWindow->setError(error);
    if (error.isEmpty()) {
        mModbusConfigEditorMainWindow->addSensorMap(devId, map.id);
    }
}

void ModbusConfigEditorController::onAddSensorRequest(const QUuid &devId)
{
    Sensor sensor;
    sensor.id = QUuid::createUuid();
    sensor.description = tr("Датчик %0").arg(toString(sensor.id).right(0x08));
    sensor.type = Sensor::Type::Separate;
    sensor.maxValue = 10000000;
    sensor.minValue = -10000000;
    sensor.registerAddress.regAddress = 30001;
    sensor.registerAddress.regType = RegisterAddress::RegisterType::AnalogInputRegisters;
    sensor.registerAddress.slaveAddress = 1;
    sensor.registerAddress.typeOrder = "21";
    sensor.registerAddress.valType = RegisterAddress::ValType::UInt16;
    sensor.mode = Sensor::Mode::Read;
    sensor.updateThreshold = 0.001;
    sensor.correctFunction = "val * 2";
    auto error = mModbusConfigModel->upsertSensor(devId, {}, sensor);
    mModbusConfigEditorMainWindow->setError(error);
    if (error.isEmpty()) {
        mModbusConfigEditorMainWindow->addSensor(devId, sensor.id, sensor.description);
    }
}

void ModbusConfigEditorController::onDeleteDeviceRequest(const QUuid &deviceId)
{
    QString error = mModbusConfigModel->deleteDevice(deviceId);
    mModbusConfigEditorMainWindow->setError(error);
    if (error.isEmpty()) {
        mModbusConfigEditorMainWindow->deleteDevice(deviceId);
    }
}

void ModbusConfigEditorController::onDeleteSensorRequest(
    const QUuid &deviceId, const QUuid &sensorId)
{
    auto error = mModbusConfigModel->deleteSensor(deviceId, sensorId);
    mModbusConfigEditorMainWindow->setError(error);
    if (error.isEmpty()) {
        mModbusConfigEditorMainWindow->deleteSensor(deviceId, sensorId);
    }
}

void ModbusConfigEditorController::onDeleteSensorMapRequest(
    const QUuid &deviceId, const QString &sensorMapId)
{
    auto error = mModbusConfigModel->deleteSensorMap(deviceId, sensorMapId);
    mModbusConfigEditorMainWindow->setError(error);
    if (error.isEmpty()) {
        mModbusConfigEditorMainWindow->deleteSensorMap(deviceId, sensorMapId);
    }
}

void ModbusConfigEditorController::onDeviceSettingsRequest(const QUuid &deviceId)
{
    auto dev = mModbusConfigModel->device(deviceId);
    if (dev.settings.id != deviceId) {
        return;
    }
    mCurrentDeviceId = deviceId;
    mModbusConfigEditorMainWindow->setDeviceSettings(mCurrentDeviceId, dev.settings);
}

void ModbusConfigEditorController::onSensorsMapSettingsRequest(
    const QUuid &deviceId, const QString &mapId)
{
    auto dev = mModbusConfigModel->device(deviceId);
    if (dev.settings.id != deviceId) {
        return;
    }
    auto map = dev.maps.value(mapId);
    if (map.id != mapId)  {
        return;
    }
    mCurrentDeviceId = deviceId;
    mCurrentMapId = mapId;
    mModbusConfigEditorMainWindow->setSensorMapSettings(map);
}

void ModbusConfigEditorController::onSensorSettingsRequest(
    const QUuid &deviceId, const QUuid &sensorId)
{
    auto dev = mModbusConfigModel->device(deviceId);
    if (dev.settings.id != deviceId) {
        return;
    }
    auto sensor = dev.sensors.value(sensorId);
    if (sensor.id != sensorId)  {
        return;
    }
    mCurrentDeviceId = deviceId;
    mCurrentSensorId = sensorId;
    mModbusConfigEditorMainWindow->setSensorSettings(deviceId, sensor);
}

void ModbusConfigEditorController::onModbusDeviceSettingsChanged(const DeviceSettings &settings)
{
    auto error = mModbusConfigModel->upsertDevice(
        settings.id, mCurrentDeviceId, settings.connectionParams, settings.description);
    mModbusConfigEditorMainWindow->setError(error);
    if (error.isEmpty()) {
        mModbusConfigEditorMainWindow->updateDeviceInModel(mCurrentDeviceId, settings);
        mCurrentDeviceId = settings.id;
    }
}

void ModbusConfigEditorController::onSensorMapSettingsChanged(const SensorsMap &settings)
{
    auto error = mModbusConfigModel->upsertSensorMap(mCurrentDeviceId, mCurrentMapId, settings);
    mModbusConfigEditorMainWindow->setError(error);
    if (error.isEmpty()) {
        mModbusConfigEditorMainWindow->updateSensorMapInModel(
            mCurrentDeviceId, mCurrentMapId, settings);
        mCurrentMapId = settings.id;
    }
}

void ModbusConfigEditorController::onSensorSettingsChanged(const Sensor &settings)
{
    auto error = mModbusConfigModel->upsertSensor(mCurrentDeviceId, mCurrentSensorId, settings);
    mModbusConfigEditorMainWindow->setError(error);
    if (error.isEmpty()) {
        mModbusConfigEditorMainWindow->updateSensorInModel(
            mCurrentDeviceId, mCurrentSensorId, settings);
        mCurrentSensorId = settings.id;
    }
}

void ModbusConfigEditorController::onShowJsonRequest()
{
    Serializer serializer;
    mModbusConfigEditorMainWindow->setJson(
        QString::fromUtf8(QJsonDocument(serializer.serialize(*mModbusConfigModel)).toJson()));
}

}
