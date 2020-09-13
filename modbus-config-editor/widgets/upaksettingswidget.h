#pragma once

#include <QWidget>

#include "../modbusentities.h"

namespace Ui {
class UpakSettingsWidget;
}

class UpakSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UpakSettingsWidget(QWidget *parent = nullptr);
    ~UpakSettingsWidget();

    void setSettings(const ModbusConfig::Settings &settings);

signals:
    void settingsChanged(const ModbusConfig::Settings &settings);

private: // slots
    void onTextChanged(const QString&);

private:
    Ui::UpakSettingsWidget *ui;
};

