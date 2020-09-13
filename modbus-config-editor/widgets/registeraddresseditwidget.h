#pragma once

#include <QWidget>

#include "../modbusentities.h"

namespace Ui {
class RegisterAddressEditWidget;
}

class RegisterAddressEditWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterAddressEditWidget(QWidget *parent = nullptr);
    ~RegisterAddressEditWidget();

    void setSettings(const ModbusConfig::RegisterAddress &address);
    ModbusConfig::RegisterAddress settings() const;

private: //slots
    void onTextChanged(const QString &);

private:
    void fillValueTypeCombobox();
    void fillRegisterTypeCombobox();

signals:
    void settingsChanged();



private:
    Ui::RegisterAddressEditWidget *ui;
};

