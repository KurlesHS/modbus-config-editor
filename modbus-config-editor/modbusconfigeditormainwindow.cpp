#include "modbusconfigeditormainwindow.h"
#include "ui_modbusconfigeditormainwindow.h"

#include <QDebug>

ModbusConfigEditorMainWindow::ModbusConfigEditorMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ModbusConfigEditorMainWindow)
    , mModbusDeviceSettingsWidget(new ModbusDeviceSettingsWidget(this))
    , mSensorSettingsWidget(new SensorSettingsWidget(this))
    , mUpakSettingsWidget(new UpakSettingsWidget(this))
    , mSensorMapWidget(new SensorMapWidget(this))
    , mTextEdit(new QTextEdit(this))
    , mFakeWidget(new QWidget(this))
{
    ui->setupUi(this);
    ui->stackedWidget->addWidget(mFakeWidget);
    ui->stackedWidget->addWidget(mSensorMapWidget);
    ui->stackedWidget->addWidget(mUpakSettingsWidget);
    ui->stackedWidget->addWidget(mSensorSettingsWidget);
    ui->stackedWidget->addWidget(mModbusDeviceSettingsWidget);
    ui->stackedWidget->addWidget(mTextEdit);
    ui->stackedWidget->setCurrentWidget(mFakeWidget);

    ui->treeView->setModel(&mSettingsModel);
    ui->treeView->header()->hide();
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    mTextEdit->setReadOnly(true);

    connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, &ModbusConfigEditorMainWindow::onSelectionChanged);
    connect(ui->treeView, &QTreeView::customContextMenuRequested,
        this, &ModbusConfigEditorMainWindow::onCustomContextMenuRequested);

    connect(mUpakSettingsWidget, &UpakSettingsWidget::settingsChanged,
        this, &ModbusConfigEditorMainWindow::upakSettingsChanged);
    connect(mModbusDeviceSettingsWidget, &ModbusDeviceSettingsWidget::settingChanged,
        this, &ModbusConfigEditorMainWindow::modbusDeviceSettingsChanged);
    connect(mSensorMapWidget, &SensorMapWidget::settingChanged,
        this, &ModbusConfigEditorMainWindow::sensorMapSettingsChanged);
    connect(mSensorSettingsWidget, &SensorSettingsWidget::settingChanged,
        this, &ModbusConfigEditorMainWindow::sensorSettingsChanged);
}

ModbusConfigEditorMainWindow::~ModbusConfigEditorMainWindow()
{
    delete ui;
}

void ModbusConfigEditorMainWindow::setJson(const QString &text)
{
    mTextEdit->setPlainText(text);
}

void ModbusConfigEditorMainWindow::setError(const QString &error)
{
    mError = error;
    ui->statusbar->showMessage(error);
}

void ModbusConfigEditorMainWindow::setCommonSetting(const ModbusConfig::Settings &settings)
{
    mUpakSettingsWidget->setSettings(settings);
}

void ModbusConfigEditorMainWindow::setDeviceSettings(
    const QUuid &prevId, const ModbusConfig::DeviceSettings &settings)
{
    mSettingsModel.updateDeviceSettings(prevId, settings.id, settings.description);
    mModbusDeviceSettingsWidget->setSettings(settings);
}

void ModbusConfigEditorMainWindow::setSensorMapSettings(
    const ModbusConfig::SensorsMap &settings)
{
    mSensorMapWidget->setSettings(settings);
}

void ModbusConfigEditorMainWindow::setSensorSettings(
    const QUuid &devId, const ModbusConfig::Sensor &settings)
{
    mSensorSettingsWidget->setSettings(mSettingsModel.sensorMapsForDevice(devId), settings);
}

void ModbusConfigEditorMainWindow::updateDeviceInModel(
    const QUuid &prevId, const ModbusConfig::DeviceSettings &settings)
{
    mSettingsModel.updateDeviceSettings(prevId, settings.id, settings.description);
}

void ModbusConfigEditorMainWindow::updateSensorMapInModel(
    const QUuid &devId, const QString &prevId, const ModbusConfig::SensorsMap &settings)
{
    mSettingsModel.updateSensorMapSettings(devId, prevId, settings.id);
}

void ModbusConfigEditorMainWindow::updateSensorInModel(
    const QUuid &devId, const QUuid &prevId, const ModbusConfig::Sensor &settings)
{
    mSettingsModel.updateSensorSettings(devId, prevId, settings.id, settings.description);
}

void ModbusConfigEditorMainWindow::addDevice(const QUuid &id, const QString &name)
{
    int cntPrev = mSettingsModel.deviceCount();
    auto item = mSettingsModel.addDevice(id, name);
    int cntCurrent = mSettingsModel.deviceCount();
    if (cntPrev == 0 && cntCurrent == 1) {
        ui->treeView->expand(mSettingsModel.modbusSettingsItem()->index());
    }
    if (item) {
        ui->treeView->expand(item->index());
    }
}

void ModbusConfigEditorMainWindow::deleteDevice(const QUuid &id)
{
    mSettingsModel.deleteDevice(id);
}

void ModbusConfigEditorMainWindow::deleteSensor(const QUuid &devId, const QUuid &sensorId)
{
    mSettingsModel.deleteSensor(devId, sensorId);
}

void ModbusConfigEditorMainWindow::deleteSensorMap(const QUuid &devId, const QString &sensorMapId)
{
    mSettingsModel.deleteSensorMap(devId, sensorMapId);
}

void ModbusConfigEditorMainWindow::addSensorMap(const QUuid &devId, const QString &sensorId)
{
    addAndExpandItem(mSettingsModel.addSensorMap(devId, sensorId));
}

void ModbusConfigEditorMainWindow::addSensor(
    const QUuid &devId, const QUuid &sensorId, const QString &name)
{
    addAndExpandItem(mSettingsModel.addSensor(devId, sensorId, name));
}

void ModbusConfigEditorMainWindow::requestAddRegisterMap(const QUuid &devId)
{
    emit addRegisterMapRequest(devId);
}

void ModbusConfigEditorMainWindow::requestAddSensor(const QUuid &devId)
{
    emit addSensorRequest(devId);
}

void ModbusConfigEditorMainWindow::addAndExpandItem(QStandardItem *item)
{
    if (item && item->parent() && item->parent()->rowCount() == 0x01) {
        ui->treeView->expand(item->parent()->index());
    }
}

void ModbusConfigEditorMainWindow::requestDeleteDevice(const QUuid &id)
{
    emit deleteDeviceRequest(id);
}

void ModbusConfigEditorMainWindow::onSelectionChanged(
    const QItemSelection &currentSelection, const QItemSelection &previousSelection)
{
    auto index = [](const QItemSelection &currentSelection) -> QModelIndex {
        if (currentSelection.isEmpty()) {
            return {};
        }
        auto indexes = currentSelection.at(0).indexes();
        return indexes.isEmpty() ? QModelIndex{} : indexes.at(0);
    };

    QModelIndex current = index(currentSelection);
    QModelIndex previous = index(previousSelection);    
    QStandardItem *currentItem = mSettingsModel.itemFromIndex(current);

    if (!mError.isEmpty()) {
        // where is uncorrected error, it's forbidden to switch to anther page
        ui->treeView->selectionModel()->blockSignals(true);
        ui->treeView->selectionModel()->setCurrentIndex(
            previous, QItemSelectionModel::Current | QItemSelectionModel::Select);
        ui->treeView->selectionModel()->blockSignals(false);
        return;
    }
    requestDataByItem(currentItem);
}

void ModbusConfigEditorMainWindow::onCustomContextMenuRequested(const QPoint &pos)
{
    using IT = ModbusConfig::SettingsModel::ItemType;
    QStandardItem *item = mSettingsModel.itemFromIndex(ui->treeView->indexAt(pos));
    if (!item) {
        return;
    }
    QMenu *menu{};
    switch (mSettingsModel.itemType(item)) {
    case IT::NoneType:
        break;
    case IT::CommonType:
        menu = commonSettingsContextMenu(item);
        break;
    case IT::ModbusRootSettings:
        menu = modbusRootMenuContextMenu(item);        
        break;
    case IT::ModbusDevice:
        menu = modbusDeviceMenuContextMenu(item);
        break;
    case IT::SensorMapsRoot:
        menu = sensorsMapRootMenuContextMenu(item);
        break;
    case IT::SensorsRoot:
        menu = sensorRootMenuContextMenu(item);
        break;
    case IT::Sensor:
        menu = sensorMenuContextMenu(item);
        break;
    case IT::SensorMap:
        menu = sensorMapMenuContextMenu(item);
        break;
    default:
        break;
    }
    if (menu) {
        connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater, Qt::QueuedConnection);
        menu->popup(QCursor::pos());
    }
}

QMenu *ModbusConfigEditorMainWindow::commonSettingsContextMenu(QStandardItem *item)
{
    Q_UNUSED(item)
    return nullptr;
}

QMenu *ModbusConfigEditorMainWindow::modbusRootMenuContextMenu(QStandardItem *item)
{
    Q_UNUSED(item)
    auto menu = new QMenu(this);
    auto action = menu->addAction(tr("Добавить новое modbus устройствo"));
    connect(action, &QAction::triggered,
        this, &ModbusConfigEditorMainWindow::addModbusDeviceRequest);
    return menu;
}

QMenu *ModbusConfigEditorMainWindow::modbusDeviceMenuContextMenu(QStandardItem *item)
{
    auto menu = new QMenu(this);
    QUuid devId = mSettingsModel.getDeviceId(item);
    addAddSensorsMapMenuAction(menu, devId, item->text());
    addAddSensorMenuAction(menu, devId, item->text());
    menu->addSeparator();
    auto action = menu->addAction(tr("Удалить устройствo '%0'").arg(item->text()));
    connect(action, &QAction::triggered,
        this, [this, devId]() {
            requestDeleteDevice(devId);
        });
    return menu;
}

QMenu *ModbusConfigEditorMainWindow::sensorsMapRootMenuContextMenu(QStandardItem *item)
{
    auto devInfo = mSettingsModel.getDeviceIinfoBySensorsMapRootItem(item);
    if (devInfo.first.isNull()) {
        return {};
    }
    auto menu = new QMenu(this);
    addAddSensorsMapMenuAction(menu, devInfo.first, devInfo.second);
    return menu;
}

QMenu *ModbusConfigEditorMainWindow::sensorRootMenuContextMenu(QStandardItem *item)
{
    auto devInfo = mSettingsModel.getDeviceIinfoBySensorsMapRootItem(item);
    if (devInfo.first.isNull()) {
        return {};
    }
    auto menu = new QMenu(this);
    addAddSensorMenuAction(menu, devInfo.first, devInfo.second);
    return menu;
}

QMenu *ModbusConfigEditorMainWindow::sensorMenuContextMenu(QStandardItem *item)
{
    auto devInfo = mSettingsModel.getDeviceIinfoBySensorsMapRootItem(item->parent());
    auto devId = devInfo.first;
    if (devId.isNull()) {
        return {};
    }

    auto sensorId = mSettingsModel.getSensorIdBySensorItem(item);
    if (sensorId.isNull()) {
        return {};
    }

    auto menu = new QMenu(this);
    auto action = menu->addAction(tr("Удалить датчик '%0'").arg(item->text()));
    connect(action, &QAction::triggered,
        this, [this, devId, sensorId]() {
            emit deleteSensorRequest(devId, sensorId);
        });
    return menu;
}

QMenu *ModbusConfigEditorMainWindow::sensorMapMenuContextMenu(QStandardItem *item)
{
    auto devInfo = mSettingsModel.getDeviceIinfoBySensorsMapRootItem(item->parent());
    auto devId = devInfo.first;
    if (devId.isNull()) {
        return {};
    }

    auto sensorMapId = mSettingsModel.getSensorsMapIdBySensorsMapItem(item);
    if (sensorMapId.isEmpty()) {
        return {};
    }

    auto menu = new QMenu(this);
    auto action = menu->addAction(tr("Удалить карту '%0'").arg(item->text()));
    connect(action, &QAction::triggered,
        this, [this, devId, sensorMapId]() {
            emit deleteSensorMapRequest(devId, sensorMapId);
        });
    return menu;
}

void ModbusConfigEditorMainWindow::addAddSensorMenuAction(
    QMenu *menu, const QUuid &devId, const QString &deviceName)
{
    auto action = menu->addAction(tr("Добавить датчик для устройства '%0'")
                                 .arg(deviceName));
    connect(action, &QAction::triggered, [this, devId]() {
        requestAddSensor(devId);
    });
}

void ModbusConfigEditorMainWindow::addAddSensorsMapMenuAction(
    QMenu *menu, const QUuid &devId, const QString &deviceName)
{
    auto action = menu->addAction(tr("Добавить карту регистров для устройства '%0'")
                                      .arg(deviceName));
    connect(action, &QAction::triggered, [this, devId]() {
        requestAddRegisterMap(devId);
    });
}

void ModbusConfigEditorMainWindow::requestDataByItem(QStandardItem *item)
{
    QWidget *switchTo = mFakeWidget;
    if (item) {
        using IT = ModbusConfig::SettingsModel::ItemType;
        switch (mSettingsModel.itemType(item)) {
        case IT::CommonType:
            switchTo = mUpakSettingsWidget;
            requestUpakSettings(item);
            break;
        case IT::ModbusRootSettings:
            switchTo = mTextEdit;
            emit showJsonRequest();
            break;
        case IT::ModbusDevice:
            switchTo = mModbusDeviceSettingsWidget;
            requestDeviceSettings(item);
            break;
        case IT::Sensor:
            switchTo = mSensorSettingsWidget;
            requestSensorSettings(item);
            break;
        case IT::SensorMap:
            switchTo = mSensorMapWidget;
            requestSensorMapSettings(item);
            break;
        default:
            break;
        }
    }
    ui->stackedWidget->setCurrentWidget(switchTo);
}

void ModbusConfigEditorMainWindow::requestUpakSettings(QStandardItem *item)
{
    Q_UNUSED(item);
    emit upakSettingRequest();
}

void ModbusConfigEditorMainWindow::requestDeviceSettings(QStandardItem *item)
{
    emit deviceSettingsRequest(mSettingsModel.getDeviceId(item));
}

void ModbusConfigEditorMainWindow::requestSensorMapSettings(QStandardItem *item)
{
    auto devInfo = mSettingsModel.getDeviceIinfoBySensorsMapRootItem(item->parent());
    if (devInfo.first.isNull()) {
        // something goes wrong
        return;
    }
    auto sensorMapId = mSettingsModel.getSensorsMapIdBySensorsMapItem(item);
    if (sensorMapId.isEmpty()) {
        // something goes wrong
        return;
    }
    emit sensorsMapSettingsRequest(devInfo.first, sensorMapId);
}

void ModbusConfigEditorMainWindow::requestSensorSettings(QStandardItem *item)
{
    auto devInfo = mSettingsModel.getDeviceIinfoBySensorsMapRootItem(item->parent());
    if (devInfo.first.isNull()) {
        // something goes wrong
        return;
    }
    auto sensorId = mSettingsModel.getSensorIdBySensorItem(item);
    if (sensorId.isNull()) {
        // something goes wrong
        return;
    }
    emit sensorSettingsRequest(devInfo.first, sensorId);
}

