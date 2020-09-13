#pragma once

#include "modbusconfigmodel.h"

#include <QJsonObject>

namespace ModbusConfig {

class Serializer
{
public:
    Serializer() = default;

    QJsonObject serialize(const ModbusConfigModel &model);
    ModbusConfigModel deserialize(const QJsonObject &root, QString *error);
};

}
