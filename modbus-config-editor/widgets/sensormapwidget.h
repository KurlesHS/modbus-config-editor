#pragma once

#include <QWidget>

#include "modbusentities.h"
#include "registeraddresseditwidget.h"

namespace Ui {
class SensorMapWidget;
}

class SensorMapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SensorMapWidget(QWidget *parent = nullptr);
    ~SensorMapWidget();

    void setSettings(const ModbusConfig::SensorsMap &settings);

signals:
    void settingChanged(const ModbusConfig::SensorsMap &settings);

private: //slots
    void onRegisterAddressChanged();
    void onTextEdited(const QString &);

private:
    Ui::SensorMapWidget *ui;
    RegisterAddressEditWidget *mRegisterAddressEditWidget;
};


