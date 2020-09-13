#pragma once

#include <QUuid>
#include <QVariant>
#include <QComboBox>
#include <QJsonValue>

#include "modbusentities.h"

namespace ModbusConfig {

template<typename T>
int toInt(T v) {
    return static_cast<int>(v);
}

template<typename T>
T getValueBasedOnCombobox(QComboBox *combobox) {
    return static_cast<T>(combobox->currentData().toInt());
}

void setComboboxBasedOnValue(QComboBox *combobox, int data);

QString toString(const QUuid &id);
QUuid toUuid(const QString &str);

QString toString(RegisterAddress::RegisterType type);
QString toHumanString(RegisterAddress::RegisterType type);
QString toHumanString(Sensor::Mode mode);
QString toString(Sensor::Mode mode);
QString toString(RegisterAddress::ValType type);

RegisterAddress::RegisterType toRegisterType(const QString &str);
RegisterAddress::ValType toValueType(const QString &str);
QString checkRegisterAddress(RegisterAddress::RegisterType type, int address);
QString checkRegisterAddress(const RegisterAddress &address);
QString checkSensorMode(RegisterAddress::RegisterType type, Sensor::Mode mode);

QJsonValue toJsonValue(const QVariant &value, RegisterAddress::ValType type);
QVariant toVariant(const QJsonValue &value, RegisterAddress::ValType type);

ConnectionParams toConnectionParams(
    const QString &address, const QString &serverId, QString *error);
QString toString(const ConnectionParams &params);


} // namespace ModbusConfig
