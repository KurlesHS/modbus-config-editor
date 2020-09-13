#pragma once

#include <QMainWindow>

#include "widgets/modbusdevicesettingswidget.h"
#include "widgets/sensorsettingswidget.h"
#include "widgets/upaksettingswidget.h"
#include "widgets/sensormapwidget.h"

#include "settingsmodel.h"

#include <QItemSelectionModel>
#include <QTextEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class ModbusConfigEditorMainWindow; }
QT_END_NAMESPACE

class ModbusConfigEditorMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    ModbusConfigEditorMainWindow(QWidget *parent = nullptr);
    ~ModbusConfigEditorMainWindow();

    void setJson(const QString &text);
    void setError(const QString &error);
    void setCommonSetting(const ModbusConfig::Settings &settings);
    void setDeviceSettings(const QUuid &prevId, const ModbusConfig::DeviceSettings &settings);
    void setSensorMapSettings(const ModbusConfig::SensorsMap &settings);
    void setSensorSettings(const QUuid &devId, const ModbusConfig::Sensor &settings);
    void updateDeviceInModel(const QUuid &prevId, const ModbusConfig::DeviceSettings &settings);
    void updateSensorMapInModel(const QUuid &devId, const QString &prevId,
        const ModbusConfig::SensorsMap &settings);
    void updateSensorInModel(const QUuid &devId, const QUuid &prevId,
        const ModbusConfig::Sensor &settings);

    void setSettings(const ModbusConfig::DeviceSettings &settings);

    void addDevice(const QUuid &id, const QString &name);
    void deleteDevice(const QUuid &id);
    void deleteSensor(const QUuid &devId, const QUuid &sensorId);
    void deleteSensorMap(const QUuid &devId, const QString &sensorMapId);

    void addSensorMap(const QUuid &devId, const QString &sensorId);
    void addSensor(const QUuid &devId, const QUuid &sensorId, const QString &name);


private:
    void requestDataByItem(QStandardItem *item);
    void requestUpakSettings(QStandardItem *item);
    void requestDeviceSettings(QStandardItem *item);
    void requestSensorMapSettings(QStandardItem *item);
    void requestSensorSettings(QStandardItem *item);

    void requestDeleteDevice(const QUuid &id);
    void requestAddRegisterMap(const QUuid &devId);
    void requestAddSensor(const QUuid &devId);

    void addAndExpandItem(QStandardItem *item);

private: //slots
    void onSelectionChanged(const QItemSelection &current, const QItemSelection &previous);
    void onCustomContextMenuRequested(const QPoint &pos);

    QMenu *commonSettingsContextMenu(QStandardItem *item);
    QMenu *modbusRootMenuContextMenu(QStandardItem *item);
    QMenu *modbusDeviceMenuContextMenu(QStandardItem *item);

    QMenu *sensorsMapRootMenuContextMenu(QStandardItem *item);
    QMenu *sensorRootMenuContextMenu(QStandardItem *item);

    QMenu *sensorMenuContextMenu(QStandardItem *item);
    QMenu *sensorMapMenuContextMenu(QStandardItem *item);


    void addAddSensorMenuAction(QMenu *menu, const QUuid &devId, const QString &deviceName);
    void addAddSensorsMapMenuAction(QMenu *menu, const QUuid &devId, const QString &deviceName);

signals:
    void upakSettingRequest();
    void addModbusDeviceRequest();
    void deleteDeviceRequest(const QUuid &deviceId);
    void deleteSensorRequest(const QUuid &deviceId, const QUuid &sensorId);
    void deleteSensorMapRequest(const QUuid &deviceId, const QString &sensorMapId);

    void deviceSettingsRequest(const QUuid &deviceId);
    void sensorsMapSettingsRequest(const QUuid &deviceId, const QString &mapId);
    void sensorSettingsRequest(const QUuid &deviceId, const QUuid &sensorId);

    void addRegisterMapRequest(const QUuid &deviceId);
    void addSensorRequest(const QUuid &deviceId);

    void upakSettingsChanged(const ModbusConfig::Settings &settings);
    void modbusDeviceSettingsChanged(const ModbusConfig::DeviceSettings &settings);
    void sensorMapSettingsChanged(const ModbusConfig::SensorsMap &settings);
    void sensorSettingsChanged(const ModbusConfig::Sensor &settings);

    void showJsonRequest();

private:
    Ui::ModbusConfigEditorMainWindow *ui;

    ModbusConfig::SettingsModel mSettingsModel;

    QString mError;

    ModbusDeviceSettingsWidget *mModbusDeviceSettingsWidget;
    SensorSettingsWidget *mSensorSettingsWidget;
    UpakSettingsWidget *mUpakSettingsWidget;
    SensorMapWidget *mSensorMapWidget;
    QTextEdit *mTextEdit;
    QWidget *mFakeWidget;


};
