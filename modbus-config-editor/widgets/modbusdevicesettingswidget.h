#pragma once

#include <QWidget>

#include "../modbusentities.h"

namespace Ui {
class ModbusDeviceSettingsWidget;
}

class ModbusDeviceSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModbusDeviceSettingsWidget(QWidget *parent = nullptr);
    ~ModbusDeviceSettingsWidget();

    void setSettings(const ModbusConfig::DeviceSettings &settings);

    void setTcpConnectionSettings(const ModbusConfig::ConnectionParams &settings);
    void setSerialConnectionSettings(const ModbusConfig::ConnectionParams &settings);

signals:
    void settingChanged(const ModbusConfig::DeviceSettings &settings);

private: //slots

private:
    void blockComboboxSignals(bool block);

    void fillConnectionTypeCombobox();
    void fillFlowControlCombobox();
    void fillStopbitsCombobox();
    void fillDatabitsCombobox();
    void fillBaudrateCombobox();
    void fillParityCombobox();

    void onSettingChanged();

    void onTextChanged(const QString &);
    void onComboboxConTypeIndexChanged(int);

private:
    Ui::ModbusDeviceSettingsWidget *ui;
};

