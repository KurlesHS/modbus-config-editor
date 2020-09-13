#include "upaksettingswidget.h"
#include "ui_upaksettingswidget.h"

UpakSettingsWidget::UpakSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpakSettingsWidget)
{
    qRegisterMetaType<ModbusConfig::Settings>("ModbusConfig::Settings");
    ui->setupUi(this);
    connect(ui->lineEditUpakServerAddress, &QLineEdit::textEdited,
        this, &UpakSettingsWidget::onTextChanged);
    connect(ui->lineEditPassword, &QLineEdit::textEdited,
        this, &UpakSettingsWidget::onTextChanged);
    connect(ui->lineEditUserName, &QLineEdit::textEdited,
        this, &UpakSettingsWidget::onTextChanged);
    connect(ui->spinBoxWriteRequestTtl,
        static_cast<void (QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged),
        this, &UpakSettingsWidget::onTextChanged);
}

UpakSettingsWidget::~UpakSettingsWidget()
{
    delete ui;
}

void UpakSettingsWidget::setSettings(const ModbusConfig::Settings &settings)
{
    ui->lineEditUserName->setText(settings.upakUserName);
    ui->lineEditPassword->setText(settings.upakPassword);
    ui->lineEditUpakServerAddress->setText(settings.upakServeUrl);
    ui->spinBoxWriteRequestTtl->blockSignals(true);
    ui->spinBoxWriteRequestTtl->setValue(settings.writeRequestTtl);
    ui->spinBoxWriteRequestTtl->blockSignals(false);
}

void UpakSettingsWidget::onTextChanged(const QString &)
{
    ModbusConfig::Settings settings;
    settings.upakPassword = ui->lineEditPassword->text();
    settings.upakServeUrl = ui->lineEditUpakServerAddress->text();
    settings.upakUserName = ui->lineEditUserName->text();
    settings.writeRequestTtl = ui->spinBoxWriteRequestTtl->value();
    emit settingsChanged(settings);
}
