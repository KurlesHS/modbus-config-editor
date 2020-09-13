#pragma once

#include <QObject>

#include "modbusconfigeditormainwindow.h"
#include "modbusconfigmodel.h"


namespace ModbusConfig {

class ModbusConfigEditorController : public QObject
{
    Q_OBJECT
public:
    explicit ModbusConfigEditorController(ModbusConfigEditorMainWindow *modbusConfigEditorMainWindow,
        ModbusConfigModel *modbusConfigModel,
        QObject *parent = nullptr);

signals:

private:
    void onUpakSettingRequest();

    void onAddModbusDeviceRequest();
    void onAddRegisterMapRequest(const QUuid &devId);
    void onAddSensorRequest(const QUuid &devId);
    void onDeleteDeviceRequest(const QUuid &deviceId);
    void onDeleteSensorRequest(const QUuid &deviceId, const QUuid &sensorId);
    void onDeleteSensorMapRequest(const QUuid &deviceId, const QString &sensorMapId);
    void onDeviceSettingsRequest(const QUuid &deviceId);    
    void onSensorsMapSettingsRequest(const QUuid &deviceId, const QString &mapId);
    void onSensorSettingsRequest(const QUuid &deviceId, const QUuid &sensorId);

    void onUpakSettingsChanged(const ModbusConfig::Settings &settings);
    void onModbusDeviceSettingsChanged(const ModbusConfig::DeviceSettings &settings);
    void onSensorMapSettingsChanged(const ModbusConfig::SensorsMap &settings);
    void onSensorSettingsChanged(const ModbusConfig::Sensor &settings);
    void onShowJsonRequest();

private:
    ModbusConfigEditorMainWindow *mModbusConfigEditorMainWindow;
    ModbusConfigModel *mModbusConfigModel;
    QUuid mCurrentDeviceId;
    QUuid mCurrentSensorId;
    QString mCurrentMapId;

};

}
