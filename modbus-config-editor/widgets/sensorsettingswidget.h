#pragma once

#include <QWidget>

#include "modbusentities.h"

#include "registeraddresseditwidget.h"

namespace Ui {
class SensorSettingsWidget;
}

class SensorSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SensorSettingsWidget(QWidget *parent = nullptr);
    ~SensorSettingsWidget();

    void setSettings(const QStringList &sensorsMaps, const ModbusConfig::Sensor &settings);

signals:
    void settingChanged(const ModbusConfig::Sensor &settings);

private:
    ModbusConfig::Sensor settings() const;

    void fillSensorTypeCombobox();
    void fillSensorModeCombobox();

    void fillSensorMapCombobox(const QStringList &sensorsMaps, const QString &currentMap);

private: //slots
    void onRegisterAddressChanged();
    void onTextChanged(const QString &);
    void onCurrentIndexChangedOnTypeCombobox(int index);

private:
    Ui::SensorSettingsWidget *ui;
    RegisterAddressEditWidget *mRegisterAddressEditWidget;
};

