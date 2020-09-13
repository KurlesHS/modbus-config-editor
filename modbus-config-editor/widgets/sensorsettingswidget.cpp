#include "sensorsettingswidget.h"
#include "ui_sensorsettingswidget.h"

#include "../utils.h"

using namespace ModbusConfig;

SensorSettingsWidget::SensorSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SensorSettingsWidget),
    mRegisterAddressEditWidget(new RegisterAddressEditWidget(this))
{
    ui->setupUi(this);
    auto lay = new QVBoxLayout(ui->pageSingleSensor);
    lay->addWidget(mRegisterAddressEditWidget);
    lay->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    fillSensorModeCombobox();
    fillSensorTypeCombobox();

    connect(mRegisterAddressEditWidget, &RegisterAddressEditWidget::settingsChanged,
        this, &SensorSettingsWidget::onRegisterAddressChanged);
    connect(ui->spinBoxMapOffset,
        static_cast<void (QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged),
        this, &SensorSettingsWidget::onTextChanged);
    connect(ui->doubleSpinBoxMaxValue,
        static_cast<void (QDoubleSpinBox::*)(const QString &)>(&QDoubleSpinBox::valueChanged),
        this, &SensorSettingsWidget::onTextChanged);
    connect(ui->doubleSpinBoxMinValue,
        static_cast<void (QDoubleSpinBox::*)(const QString &)>(&QDoubleSpinBox::valueChanged),
        this, &SensorSettingsWidget::onTextChanged);
    connect(ui->doubleSpinBoxUpdateThreshold,
        static_cast<void (QDoubleSpinBox::*)(const QString &)>(&QDoubleSpinBox::valueChanged),
        this, &SensorSettingsWidget::onTextChanged);
    connect(ui->comboBoxMode,
        static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged),
        this, &SensorSettingsWidget::onTextChanged);
    connect(ui->comboBoxSensorType,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &SensorSettingsWidget::onCurrentIndexChangedOnTypeCombobox);
    connect(ui->comboBoxSensorsMap,
        static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged),
        this, &SensorSettingsWidget::onTextChanged);
    connect(ui->lineEditId, &QLineEdit::textEdited,
        this, &SensorSettingsWidget::onTextChanged);
    connect(ui->lineEditCorrectFunc, &QLineEdit::textEdited,
        this, &SensorSettingsWidget::onTextChanged);
    connect(ui->lineEditDescription, &QLineEdit::textEdited,
        this, &SensorSettingsWidget::onTextChanged);
}

SensorSettingsWidget::~SensorSettingsWidget()
{
    delete ui;
}

void SensorSettingsWidget::setSettings(const QStringList &sensorsMaps, const Sensor &settings)
{
    fillSensorMapCombobox(sensorsMaps, settings.mapId);
    auto blockSignalsHelper = [this](bool block) {
        ui->comboBoxMode->blockSignals(block);
        ui->comboBoxSensorType->blockSignals(block);
        ui->comboBoxSensorsMap->blockSignals(block);
        ui->spinBoxMapOffset->blockSignals(block);
        ui->doubleSpinBoxMaxValue->blockSignals(block);
        ui->doubleSpinBoxMinValue->blockSignals(block);
        ui->doubleSpinBoxUpdateThreshold->blockSignals(block);
        ui->lineEditCorrectFunc->blockSignals(block);
        ui->lineEditDescription->blockSignals(block);
        ui->lineEditId->blockSignals(block);
    };

    blockSignalsHelper(true);
    mRegisterAddressEditWidget->setSettings(settings.registerAddress);
    setComboboxBasedOnValue(ui->comboBoxMode, toInt(settings.mode));
    setComboboxBasedOnValue(ui->comboBoxSensorType, toInt(settings.type));
    ui->lineEditCorrectFunc->setText(settings.correctFunction);
    ui->lineEditDescription->setText(settings.description);
    ui->lineEditId->setText(toString(settings.id));
    ui->spinBoxMapOffset->setValue(settings.mapOffset);
    ui->doubleSpinBoxMaxValue->setValue(settings.maxValue.toDouble());
    ui->doubleSpinBoxMinValue->setValue(settings.minValue.toDouble());
    ui->doubleSpinBoxUpdateThreshold->setValue(settings.updateThreshold);

    if (settings.type == Sensor::Type::Separate) {
        ui->stackedWidget->setCurrentWidget(ui->pageSingleSensor);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->pageMapSensor);
    }
    blockSignalsHelper(false);
}

ModbusConfig::Sensor SensorSettingsWidget::settings() const
{
    Sensor result;
    result.registerAddress = mRegisterAddressEditWidget->settings();
    result.mapId = ui->comboBoxSensorsMap->currentText();
    result.id = toUuid(ui->lineEditId->text());
    result.description = ui->lineEditDescription->text();
    result.correctFunction = ui->lineEditCorrectFunc->text();
    result.mapOffset = ui->spinBoxMapOffset->value();
    result.maxValue = ui->doubleSpinBoxMaxValue->value();
    result.minValue = ui->doubleSpinBoxMinValue->value();
    result.updateThreshold = ui->doubleSpinBoxUpdateThreshold->value();
    result.mode = getValueBasedOnCombobox<Sensor::Mode>(ui->comboBoxMode);
    result.type = getValueBasedOnCombobox<Sensor::Type>(ui->comboBoxSensorType);
    return result;
}

void SensorSettingsWidget::fillSensorTypeCombobox()
{
    ui->comboBoxSensorType->addItem(tr("Отдельный датчик"), toInt(Sensor::Type::Separate));
    ui->comboBoxSensorType->addItem(tr("Датчик в карте регистров"), toInt(Sensor::Type::Map));
}

void SensorSettingsWidget::fillSensorModeCombobox()
{
    QList<Sensor::Mode> modes = {
        Sensor::Mode::Read,
        Sensor::Mode::Write,
        Sensor::Mode::ReadWrite
    };
    for (auto mode : modes) {
        ui->comboBoxMode->addItem(toHumanString(mode), toInt(mode));
    }
}

void SensorSettingsWidget::fillSensorMapCombobox(
    const QStringList &sensorsMaps, const QString &currentMap)
{
    ui->comboBoxSensorsMap->blockSignals(true);
    ui->comboBoxSensorsMap->clear();
    int index = 0;
    for (const auto &map : sensorsMaps) {
        ui->comboBoxSensorsMap->addItem(map);
        if (map == currentMap) {
            ui->comboBoxSensorsMap->setCurrentIndex(index);
        }
        ++index;
    }
    ui->comboBoxSensorsMap->blockSignals(false);
}

void SensorSettingsWidget::onRegisterAddressChanged()
{
    emit settingChanged(settings());
}

void SensorSettingsWidget::onTextChanged(const QString &)
{
    onRegisterAddressChanged();
}

void SensorSettingsWidget::onCurrentIndexChangedOnTypeCombobox(int index)
{
    if (index == 0) {
        ui->stackedWidget->setCurrentWidget(ui->pageSingleSensor);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->pageMapSensor);
    }
    onRegisterAddressChanged();
}
