#include "registeraddresseditwidget.h"
#include "ui_registeraddresseditwidget.h"

#include "../utils.h"

using namespace ModbusConfig;

RegisterAddressEditWidget::RegisterAddressEditWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterAddressEditWidget)
{
    ui->setupUi(this);
    fillRegisterTypeCombobox();
    fillValueTypeCombobox();

    connect(ui->comboBoxRegisterType,
        static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged),
        this, &RegisterAddressEditWidget::onTextChanged);
    connect(ui->comboBoxValueType,
        static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged),
        this, &RegisterAddressEditWidget::onTextChanged);
    connect(ui->lineEditValueOrder,&QLineEdit::textEdited,
        this, &RegisterAddressEditWidget::onTextChanged);
    connect(ui->spinBoxRegisterAddress,
        static_cast<void (QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged),
        this, &RegisterAddressEditWidget::onTextChanged);
    connect(ui->spinBoxSlaveAddress,
        static_cast<void (QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged),
        this, &RegisterAddressEditWidget::onTextChanged);
}

RegisterAddressEditWidget::~RegisterAddressEditWidget()
{
    delete ui;
}

void RegisterAddressEditWidget::setSettings(const RegisterAddress &address)
{
    auto blockSignalsHelper = [this](bool b) {
        ui->spinBoxRegisterAddress->blockSignals(b);
        ui->spinBoxSlaveAddress->blockSignals(b);
        ui->comboBoxRegisterType->blockSignals(b);
        ui->comboBoxValueType->blockSignals(b);
    };
    blockSignalsHelper(true);
    ui->spinBoxRegisterAddress->setValue(address.regAddress);
    ui->spinBoxSlaveAddress->setValue(address.slaveAddress);
    ui->lineEditValueOrder->setText(address.typeOrder);
    setComboboxBasedOnValue(ui->comboBoxRegisterType, toInt(address.regType));
    setComboboxBasedOnValue(ui->comboBoxValueType, toInt(address.valType));
    blockSignalsHelper(false);
}

ModbusConfig::RegisterAddress RegisterAddressEditWidget::settings() const
{
    ModbusConfig::RegisterAddress result;
    result.regAddress = ui->spinBoxRegisterAddress->value();
    result.slaveAddress = ui->spinBoxSlaveAddress->value();
    result.typeOrder = ui->lineEditValueOrder->text();
    result.regType =
        getValueBasedOnCombobox<RegisterAddress::RegisterType>(ui->comboBoxRegisterType);
    result.valType = getValueBasedOnCombobox<RegisterAddress::ValType>(ui->comboBoxValueType);
    return result;
}

void RegisterAddressEditWidget::onTextChanged(const QString &)
{
    emit settingsChanged();
}

void RegisterAddressEditWidget::fillValueTypeCombobox()
{
    using VT = RegisterAddress::ValType;

    QList<RegisterAddress::ValType> lst = {
        VT::Bool,
        VT::Double,
        VT::Float,
        VT::Int16,
        VT::Int32,
        VT::Int64,
        VT::Int8,
        VT::UInt16,
        VT::UInt32,
        VT::UInt64,
        VT::UInt8
    };

    for (auto type : lst) {
        ui->comboBoxValueType->addItem(toString(type), toInt(type));
    }
}

void RegisterAddressEditWidget::fillRegisterTypeCombobox()
{
    using RT = RegisterAddress::RegisterType;
    QList<RegisterAddress::RegisterType> lst = {
        RT::AnalogInputRegisters,
        RT::AnalogOutputHoldingRegisters,
        RT::DiscreteInputContacts,
        RT::DiscreteOutputCoils
    };

    for (auto type : lst) {
        ui->comboBoxRegisterType->addItem(toHumanString(type), toInt(type));
    }
}
