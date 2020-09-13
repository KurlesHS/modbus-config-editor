#include "sensormapwidget.h"
#include "ui_sensormapwidget.h"

#include <QVBoxLayout>

#include "../utils.h"

#include <QDebug>

using namespace ModbusConfig;

SensorMapWidget::SensorMapWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SensorMapWidget),
    mRegisterAddressEditWidget(new RegisterAddressEditWidget(this))
{
    ui->setupUi(this);
    auto lay = new QVBoxLayout(ui->groupBoxRegisterAddress);
    lay->addWidget(mRegisterAddressEditWidget);

    connect(ui->spinBoxValCount,
        static_cast<void (QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged),
        this, &SensorMapWidget::onTextEdited);
    connect(ui->doubleSpinBoxDefaultVal,
        static_cast<void (QDoubleSpinBox::*)(const QString &)>(&QDoubleSpinBox::valueChanged),
        this, &SensorMapWidget::onTextEdited);
    connect(ui->lineEditId, &QLineEdit::textEdited,
        this, &SensorMapWidget::onTextEdited);
    connect(mRegisterAddressEditWidget, &RegisterAddressEditWidget::settingsChanged,
        this, &SensorMapWidget::onRegisterAddressChanged);
}

SensorMapWidget::~SensorMapWidget()
{
    delete ui;
}

void SensorMapWidget::setSettings(const SensorsMap &settings)
{
    auto blockSignalsHelper = [this](bool b) {
        ui->doubleSpinBoxDefaultVal->blockSignals(b);
        ui->spinBoxValCount->blockSignals(b);
    };
    blockSignalsHelper(true);
    ui->spinBoxValCount->setValue(settings.valueCount);
    ui->doubleSpinBoxDefaultVal->setValue(settings.defaultValue.toDouble());
    ui->lineEditId->setText(settings.id);
    blockSignalsHelper(false);
    mRegisterAddressEditWidget->setSettings(settings.registеrAddress);
}

void SensorMapWidget::onRegisterAddressChanged()
{
    ModbusConfig::SensorsMap map;
    map.id = ui->lineEditId->text();
    map.defaultValue = ui->doubleSpinBoxDefaultVal->value();
    map.valueCount = ui->spinBoxValCount->value();
    map.registеrAddress = mRegisterAddressEditWidget->settings();
    emit settingChanged(map);
}

void SensorMapWidget::onTextEdited(const QString &)
{
    onRegisterAddressChanged();
}
