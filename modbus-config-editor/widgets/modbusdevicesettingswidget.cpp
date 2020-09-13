#include "modbusdevicesettingswidget.h"
#include "ui_modbusdevicesettingswidget.h"

#include "../utils.h"

using namespace ModbusConfig;

ModbusDeviceSettingsWidget::ModbusDeviceSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModbusDeviceSettingsWidget)
{
    ui->setupUi(this);

    fillConnectionTypeCombobox();
    fillBaudrateCombobox();
    fillDatabitsCombobox();
    fillFlowControlCombobox();
    fillParityCombobox();
    fillStopbitsCombobox();

    connect(ui->lineEditDevId, &QLineEdit::textEdited,
        this, &ModbusDeviceSettingsWidget::onTextChanged);
    connect(ui->lineEditDevName, &QLineEdit::textEdited,
        this, &ModbusDeviceSettingsWidget::onTextChanged);
    connect(ui->lineEditIpAddress, &QLineEdit::textEdited,
        this, &ModbusDeviceSettingsWidget::onTextChanged);
    connect(ui->lineEditPortName, &QLineEdit::textEdited,
        this, &ModbusDeviceSettingsWidget::onTextChanged);

    connect(ui->spinBoxIpPort,
        static_cast<void (QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged),
        this, &ModbusDeviceSettingsWidget::onTextChanged);

    connect(ui->comboBoxBaudrate, &QComboBox::currentTextChanged,
        this, &ModbusDeviceSettingsWidget::onTextChanged);
    connect(ui->comboBoxConntctionType,
        static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &ModbusDeviceSettingsWidget::onComboboxConTypeIndexChanged);
    connect(ui->comboBoxDatabits, &QComboBox::currentTextChanged,
        this, &ModbusDeviceSettingsWidget::onTextChanged);
    connect(ui->comboBoxFlowcontrol, &QComboBox::currentTextChanged,
        this, &ModbusDeviceSettingsWidget::onTextChanged);
    connect(ui->comboBoxParity, &QComboBox::currentTextChanged,
        this, &ModbusDeviceSettingsWidget::onTextChanged);
    connect(ui->comboBoxStopbits, &QComboBox::currentTextChanged,
        this, &ModbusDeviceSettingsWidget::onTextChanged);
}

ModbusDeviceSettingsWidget::~ModbusDeviceSettingsWidget()
{
    delete ui;
}

void ModbusDeviceSettingsWidget::setSettings(const ModbusConfig::DeviceSettings &settings)
{
    ui->lineEditDevName->setText(settings.description);
    ui->lineEditDevId->setText(toString(settings.id));
    ui->comboBoxConntctionType->blockSignals(true);
    setComboboxBasedOnValue(ui->comboBoxConntctionType, toInt(settings.connectionParams.type));
    ui->comboBoxConntctionType->blockSignals(false);
    switch (settings.connectionParams.type) {
    case ConnectionParams::Type::RtuSerial:
        setSerialConnectionSettings(settings.connectionParams);
        break;
    case ConnectionParams::Type::Tcp:
        setTcpConnectionSettings(settings.connectionParams);
        break;
    }
}

void ModbusDeviceSettingsWidget::setTcpConnectionSettings(const ConnectionParams &settings)
{
    ui->stackedWidget->setCurrentWidget(ui->pageTcp);
    ui->lineEditIpAddress->setText(settings.address);
    ui->spinBoxIpPort->blockSignals(true);
    ui->spinBoxIpPort->setValue(settings.port);
    ui->spinBoxIpPort->blockSignals(false);
}

void ModbusDeviceSettingsWidget::setSerialConnectionSettings(const ConnectionParams &settings)
{
    ui->stackedWidget->setCurrentWidget(ui->pageSerial);
    blockComboboxSignals(true);
    setComboboxBasedOnValue(ui->comboBoxBaudrate, settings.baudrate);
    setComboboxBasedOnValue(ui->comboBoxDatabits, toInt(settings.databits));
    setComboboxBasedOnValue(ui->comboBoxFlowcontrol, toInt(settings.flowControl));
    setComboboxBasedOnValue(ui->comboBoxParity, toInt(settings.parity));
    setComboboxBasedOnValue(ui->comboBoxStopbits, toInt(settings.stopBits));
    ui->lineEditPortName->setText(settings.deviceName);

    blockComboboxSignals(false);
}

void ModbusDeviceSettingsWidget::blockComboboxSignals(bool block)
{
    ui->comboBoxBaudrate->blockSignals(block);
    ui->comboBoxDatabits->blockSignals(block);
    ui->comboBoxFlowcontrol->blockSignals(block);
    ui->comboBoxParity->blockSignals(block);
    ui->comboBoxStopbits->blockSignals(block);
}

void ModbusDeviceSettingsWidget::fillConnectionTypeCombobox()
{
    ui->comboBoxConntctionType->addItem(tr("TCP Modbus"), toInt(ConnectionParams::Type::Tcp));
    ui->comboBoxConntctionType->addItem(
        tr("RtuSerial Modbus"), toInt(ConnectionParams::Type::RtuSerial));
}

void ModbusDeviceSettingsWidget::fillFlowControlCombobox()
{
    ui->comboBoxFlowcontrol->addItem(
        tr("No Flow Control"), toInt(ConnectionParams::FlowControl::NoFlowControl));
    ui->comboBoxFlowcontrol->addItem(
        tr("Software Control"), toInt(ConnectionParams::FlowControl::SoftwareControl));
    ui->comboBoxFlowcontrol->addItem(
        tr("Hardware"), toInt(ConnectionParams::FlowControl::HardwareControl));
}

void ModbusDeviceSettingsWidget::fillStopbitsCombobox()
{
    ui->comboBoxStopbits->addItem(
        tr("Один стоповый бит"), toInt(ConnectionParams::StopBits::OneStop));
    ui->comboBoxStopbits->addItem(
        tr("Полтора стоповых бита"), toInt(ConnectionParams::StopBits::OneAndHalfStop));
    ui->comboBoxStopbits->addItem(
        tr("Два стоповых бита"), toInt(ConnectionParams::StopBits::TwoStop));
}

void ModbusDeviceSettingsWidget::fillDatabitsCombobox()
{
    ui->comboBoxDatabits->addItem(tr("Пять"), 5);
    ui->comboBoxDatabits->addItem(tr("Шесть"), 6);
    ui->comboBoxDatabits->addItem(tr("Семь"), 7);
    ui->comboBoxDatabits->addItem(tr("Восемь"), 8);
    ui->comboBoxDatabits->addItem(tr("Девять"), 9);
}

void ModbusDeviceSettingsWidget::fillBaudrateCombobox()
{
    QList<int> baudrates = {
        1200, 1800, 2400, 4800, 7200, 9600, 14400, 19200,
        31250, 38400, 56000, 57600, 76800, 115200, 128000,
        230400, 256000};

    for (auto baudrate : baudrates) {
        ui->comboBoxBaudrate->addItem(QString::number(baudrate), baudrate);
    }
}

void ModbusDeviceSettingsWidget::fillParityCombobox()
{
    ui->comboBoxParity->addItem(
        tr("No Parity"), toInt(ConnectionParams::Parity::NoParity));
    ui->comboBoxParity->addItem(
        tr("Even Parity"), toInt(ConnectionParams::Parity::EvenParity));
    ui->comboBoxParity->addItem(
        tr("Odd Parity"), toInt(ConnectionParams::Parity::OddParity));
    ui->comboBoxParity->addItem(
        tr("Mark Parity"), toInt(ConnectionParams::Parity::MarkParity));
    ui->comboBoxParity->addItem(
        tr("Space Parity"), toInt(ConnectionParams::Parity::SpaceParity));
}

void ModbusDeviceSettingsWidget::onSettingChanged()
{
    DeviceSettings settings;
    settings.description = ui->lineEditDevName->text();
    settings.id = ui->lineEditDevId->text();
    settings.connectionParams.type =
        getValueBasedOnCombobox<ConnectionParams::Type>(ui->comboBoxConntctionType);
    settings.connectionParams.flowControl =
        getValueBasedOnCombobox<ConnectionParams::FlowControl>(ui->comboBoxFlowcontrol);
    settings.connectionParams.parity =
        getValueBasedOnCombobox<ConnectionParams::Parity>(ui->comboBoxParity);
    settings.connectionParams.stopBits =
        getValueBasedOnCombobox<ConnectionParams::StopBits>(ui->comboBoxStopbits);
    settings.connectionParams.databits = ui->comboBoxDatabits->currentData().toInt();
    settings.connectionParams.baudrate = ui->comboBoxBaudrate->currentData().toInt();
    settings.connectionParams.address = ui->lineEditIpAddress->text();
    settings.connectionParams.port = ui->spinBoxIpPort->value();
    settings.connectionParams.deviceName = ui->lineEditPortName->text();
    emit settingChanged(settings);
}

void ModbusDeviceSettingsWidget::onTextChanged(const QString &)
{
    onSettingChanged();
}

void ModbusDeviceSettingsWidget::onComboboxConTypeIndexChanged(int)
{
    switch (static_cast<ConnectionParams::Type>(ui->comboBoxConntctionType->currentData().toInt())) {
    case ConnectionParams::Type::RtuSerial:
        ui->stackedWidget->setCurrentWidget(ui->pageSerial);
        break;
    case ConnectionParams::Type::Tcp:
        ui->stackedWidget->setCurrentWidget(ui->pageTcp);
        break;
    }
    onSettingChanged();
}
