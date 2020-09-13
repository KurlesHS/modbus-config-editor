#include "utils.h"

#include <QHostAddress>

namespace {
constexpr const char *boolVal = "bool";
constexpr const char *uint8Val = "uint8";
constexpr const char *int8Val = "int8";
constexpr const char *uint16Val = "uint16";
constexpr const char *int16Val = "int16";
constexpr const char *uint32Val = "uint32";
constexpr const char *int32Val = "int32";
constexpr const char *uint64Val = "uint64";
constexpr const char *int64Val = "int64";
constexpr const char *floatVal = "float";
constexpr const char *doubleVal = "double";

constexpr const char * discreteOutputCoilsValue = "discrete_output_coils";
constexpr const char * discreteInputContactsValue = "discrete_input_contacts";
constexpr const char * analogInputRegistersValue = "analog_input_registers";
constexpr const char * analogOutputHoldingRegistersValue = "analog_output_holding_registers";

constexpr const char * discreteOutputCoilsHumanValue = "Discrete Output Coils";
constexpr const char * discreteInputContactsHumanValue = "Discrete Input Contacts";
constexpr const char * analogInputRegistersHumanValue = "Analog Input Registers";
constexpr const char * analogOutputHoldingRegistersHumanValue = "Analog Output Holding Registers";


QString checkValOrder(
    const QString &valOrder, ModbusConfig::RegisterAddress::ValType type, int numBytes) {
    if (numBytes == 0) {
        return QObject::tr("У типа '%0' не может быть порядка следования байт")
            .arg(toString(type));
    }
    if (valOrder.size() != numBytes) {
        return QObject::tr(
            "Для типа '%0' количество симовлов в порядке следования байт должно быть: %1")
            .arg(toString(type)).arg(numBytes);
    }
    for (int i = 1; i <= numBytes; ++i) {
        if (!valOrder.contains(QString::number(i))) {
            QString valOrderTemplate;
            for (int i = 1; i <= numBytes; ++i) {
                valOrderTemplate.append(QString::number(i));
            }
            return QObject::tr(
                "Для типа '%0' порядок следования байт должен состоять из "
                "неповторяющихся символов %1")
                .arg(toString(type)).arg(valOrderTemplate);
        }
    }
    return {};
}

template <typename T>
bool getNum(const QString &text, T *result,
    const qlonglong min = std::numeric_limits<qlonglong>::min(),
    const qlonglong max = std::numeric_limits<qlonglong>::max())
{
    bool ok;
    qlonglong tmp = text.toLongLong(&ok);
    if (!ok || tmp < min || tmp > max) {
        return false;
    }
    *result = static_cast<T>(tmp);
    return true;
}


quint8 dataBits(const QString &text)
{
    qlonglong tmp = text.toLongLong();
    if (tmp >= 5 && tmp <= 9) {
        return static_cast<quint8>(tmp);
    }
    return 0x00;
}

bool stopBits(const QString &text, ModbusConfig::ConnectionParams::StopBits *result)
{
    using namespace ModbusConfig;
    bool ok = true;
    if (text == QStringLiteral("1")) {
        *result = ConnectionParams::StopBits::OneStop;
    }
    else if (text == QStringLiteral("2")) {
        *result = ConnectionParams::StopBits::TwoStop;
    }
    else if (text == QStringLiteral("1.5")) {
        *result = ConnectionParams::StopBits::OneAndHalfStop;
    }
    else {
        ok = false;
    }
    return ok;
}

bool flowControl(const QString &text, ModbusConfig::ConnectionParams::FlowControl *result)
{
    using namespace ModbusConfig;
    bool ok = true;
    if (text.toLower() == QStringLiteral("none")) {
        *result = ConnectionParams::FlowControl::NoFlowControl;
    }
    else if (text.toLower() == QStringLiteral("soft")) {
        *result = ConnectionParams::FlowControl::SoftwareControl;
    }
    else if (text.toLower() == QStringLiteral("hard")) {
        *result = ConnectionParams::FlowControl::HardwareControl;
    }
    else {
        ok = false;
    }
    return ok;
}

bool parity(const QString &text, ModbusConfig::ConnectionParams::Parity *result)
{
    using namespace ModbusConfig;
    bool ok = true;
    if (text == QStringLiteral("N")) {
        *result = ConnectionParams::Parity::NoParity;
    }
    else if (text == QStringLiteral("E")) {
        *result = ConnectionParams::Parity::EvenParity;
    }
    else if (text == QStringLiteral("O")) {
        *result = ConnectionParams::Parity::OddParity;
    }
    else if (text == QStringLiteral("S")) {
        *result = ConnectionParams::Parity::SpaceParity;
    }
    else if (text == QStringLiteral("M")) {
        *result = ConnectionParams::Parity::MarkParity;
    }
    else {
        ok = false;
    }
    return ok;
}

ModbusConfig::ConnectionParams handleSerialConnectionParam(const QStringList &parts,
    const QString &serverId,
    QString *error)
{
    // serial_rtu:/dev/ttyS0:56000:8:N:1:none
    using namespace ModbusConfig;
    ConnectionParams result;
    result.type = ConnectionParams::Type::RtuSerial;
    QString errorStr =
        QObject::tr(
            "Неверный формат адреса для serial rtu modbus, должен быть в "
            "формате 'serial_rtu:/dev/ttyS0:56000:8:N:1:NONE' для сервера %0")
            .arg(serverId);
    if (parts.size() < 7) {
        *error = errorStr;
    }
    else {
        QString deviceName = parts.at(1);
        QString baudrateStr = parts.at(2);
        QString dataBitsStr = parts.at(3);
        QString parityStr = parts.at(4);
        QString stopBitsStr = parts.at(5);
        QString flowControlStr = parts.at(6);

        result.deviceName = deviceName;
        result.databits = dataBits(dataBitsStr);
        if (!getNum(baudrateStr, &result.baudrate, 0, 921600)) {
            *error = QObject::tr("Неверный формат ('%0') baudrate для serial "
                                 "rtu modbus устройства с идентификатором %1")
                         .arg(baudrateStr, serverId);
        } else if (result.databits == 0) {
            *error = QObject::tr("Неверный формат ('%0') databits для serial "
                                 "rtu modbus устройства с идентификатором %1")
                         .arg(dataBitsStr, serverId);
        } else if (!stopBits(stopBitsStr, &result.stopBits)) {
            *error = QObject::tr("Неверный формат ('%0') stopBits для serial "
                                 "rtu modbus устройства с идентификатором %1")
                         .arg(stopBitsStr, serverId);
        } else if (!parity(parityStr, &result.parity)) {
            *error = QObject::tr("Неверный формат ('%0') parity для serial rtu "
                                 "modbus устройства с идентификатором %1")
                         .arg(parityStr, serverId);
        } else if (!flowControl(flowControlStr, &result.flowControl)) {
            *error = QObject::tr("Неверный формат ('%0') parity для serial rtu "
                                 "modbus устройства с идентификатором %1")
                         .arg(flowControlStr, serverId);
        }
    }
    return result;
}

ModbusConfig::ConnectionParams handleTcpConnectionParam(const QStringList &parts,
    const QString &serverId,
    QString *error)
{
    using namespace ModbusConfig;

    ConnectionParams result;
    result.type = ConnectionParams::Type::Tcp;
    QString errorStr =
        QObject::tr(
            "Неверный формат адреса для tcp modbus, должен быть в формате "
            "'tcp:ip:port', пример: 'tcp:192.1.1.1:9999' (ид устройства: %0)")
            .arg(serverId);
    if (parts.size() < 3) {
        *error = errorStr;
    }
    else {
        bool ok;
        result.address = parts.at(1);
        qlonglong port = parts.at(2).toLongLong(&ok);
        QHostAddress testAddr(result.address);
        if (testAddr.isNull()) {
            errorStr = QObject::tr("Указан некорректный tcp адрес ('%0') "
                                   "устройства с id '%1' ")
                           .arg(result.address);
        }
        else if (!ok || port <= 0 || port > 65535) {
            errorStr =
                QObject::tr(
                    "Указан некорректный tcp порт ('%0') устройства с id '%1' ")
                    .arg(parts.at(2));
        }
        else {
            result.port = static_cast<quint16>(port);
        }
    }
    return result;
}

QString toString(ModbusConfig::ConnectionParams::StopBits stopBits) {
    using SB = ModbusConfig::ConnectionParams::StopBits;
    switch (stopBits) {
    case SB::OneAndHalfStop:
        return "1.5";
    case SB::TwoStop:
        return "2";
    default:
        break;
    }
    return "1";
}

QString toString(ModbusConfig::ConnectionParams::FlowControl flowControl) {
    using FC = ModbusConfig::ConnectionParams::FlowControl;
    switch (flowControl) {
    case FC::HardwareControl:
        return "hard";
    case FC::SoftwareControl:
        return "soft";
    default:
        break;
    }
    return "none";
}

QString toString(ModbusConfig::ConnectionParams::Parity parity) {
    using P = ModbusConfig::ConnectionParams::Parity;
    switch (parity) {
    case P::EvenParity:
        return "E";
    case P::MarkParity:
        return "M";
    case P::OddParity:
        return "O";
    case P::SpaceParity:
        return "S";
    default:
        break;
    }
    return "N";
}

QString toStringRtu(const ModbusConfig::ConnectionParams &params) {
    QString deviceName = params.deviceName;
    QString baudrateStr = QString::number(params.baudrate);
    QString dataBitsStr = QString::number(params.databits);
    QString parityStr = toString(params.parity);
    QString stopBitsStr = toString(params.stopBits);
    QString flowControlStr = toString(params.flowControl);
    return QString("serial_rtu:%0:%1:%2:%3:%4:%5")
        .arg(deviceName, baudrateStr, dataBitsStr, parityStr, stopBitsStr, flowControlStr);

}

QString toStringTcp(const ModbusConfig::ConnectionParams &params) {
    return QString("tcp:%0:%1").arg(params.address).arg(params.port);
}

}

namespace ModbusConfig {

QString toString(const QUuid &id)
{
    return id.toString().remove('{').remove('}');
}

QUuid toUuid(const QString &str)
{
    return QUuid::fromString(str);
}

RegisterAddress::RegisterType toRegisterType(const QString &str)
{
    using RT = RegisterAddress::RegisterType;
    QString type = str.toLower();
    RT result = RT::Unknown;
    if (type == discreteInputContactsValue) {
        result = RT::DiscreteInputContacts;
    } else if (type == discreteOutputCoilsValue) {
        result = RT::DiscreteOutputCoils;
    } else if (type == analogInputRegistersValue) {
        result = RT::AnalogInputRegisters;
    } else if (type == analogOutputHoldingRegistersValue) {
        result = RT::AnalogOutputHoldingRegisters;
    }
    return result;
}

RegisterAddress::ValType toValueType(const QString &str)
{
    using T = RegisterAddress::ValType;
    QString type = str.toLower();
    T result = T::Unknown;
    if (type == boolVal) {
        result = T::Bool;
    } else if (type == int8Val) {
        result = T::Int8;
    } else if (type == uint8Val) {
        result = T::UInt8;
    } else if (type == int16Val) {
        result = T::Int16;
    } else if (type == uint16Val) {
        result = T::UInt16;
    } else if (type == int32Val) {
        result = T::UInt32;
    } else if (type == uint32Val) {
        result = T::UInt32;
    } else if (type == int64Val) {
        result = T::Int64;
    } else if (type == uint64Val) {
        result = T::Int64;
    } else if (type == floatVal) {
        result = T::Float;
    } else if (type == doubleVal) {
        result = T::Double;
    }
    return result;
}

QString toString(RegisterAddress::RegisterType type)
{
    using RT = RegisterAddress::RegisterType;
    switch (type) {
    case RT::AnalogInputRegisters:
        return analogInputRegistersValue;
    case RT::AnalogOutputHoldingRegisters:
        return analogOutputHoldingRegistersValue;
    case RT::DiscreteInputContacts:
        return discreteInputContactsValue;
    case RT::DiscreteOutputCoils:
        return discreteOutputCoilsValue;
    default:
        break;
    }
    return {};
}

QString toString(RegisterAddress::ValType type)
{
    using T = RegisterAddress::ValType;
    switch (type) {
    case T::Bool:
        return boolVal;
    case T::Int8:
        return int8Val;
    case T::UInt8:
        return uint8Val;
    case T::Int16:
        return int16Val;
    case T::UInt16:
        return uint16Val;
    case T::Int32:
        return int32Val;
    case T::UInt32:
        return uint32Val;
    case T::Int64:
        return int64Val;
    case T::UInt64:
        return uint64Val;
    case T::Float:
        return floatVal;
    case T::Double:
        return doubleVal;
    default:
        break;
    }
    return {};
}

QString checkRegisterAddress(RegisterAddress::RegisterType type, int address)
{
    auto checker = [](int value, int min, int max) {
        return value >= min && value <= max;
    };

    QPair<int, int> range;

    switch (type) {
    case RegisterAddress::RegisterType::DiscreteOutputCoils:
        range = {1, 9999};
        break;
    case RegisterAddress::RegisterType::DiscreteInputContacts:
        range = {10001, 19999};
        break;
    case RegisterAddress::RegisterType::AnalogInputRegisters:
        range = {30001, 39999};
        break;
    case RegisterAddress::RegisterType::AnalogOutputHoldingRegisters:
        range = {40001, 49999};
        break;
    default:
        return QObject::tr("Указан некорректный тип регистра");
    }
    if (checker(address, range.first, range.second)) {
        return {};
    }
    return QObject::tr("Для типа регистра %0 допустимый диапазон значений для адреса регистра "
                       "от %1 до %2").arg(
        toHumanString(type)).arg(range.first).arg(range.second);
}

QString toHumanString(RegisterAddress::RegisterType type)
{
    using RT = RegisterAddress::RegisterType;
    switch (type) {
    case RT::AnalogInputRegisters:
        return analogInputRegistersHumanValue;
    case RT::AnalogOutputHoldingRegisters:
        return analogOutputHoldingRegistersHumanValue;
    case RT::DiscreteInputContacts:
        return discreteInputContactsHumanValue;
    case RT::DiscreteOutputCoils:
        return discreteOutputCoilsHumanValue;
    default:
        break;
    }
    return {};
}

QJsonValue toJsonValue(const QVariant &value, RegisterAddress::ValType type)
{
    using T = RegisterAddress::ValType;
    switch (type) {
    case T::Bool:
        return value.toBool();
    case T::UInt8:
    case T::UInt16:
    case T::UInt32:
    case T::Int8:
    case T::Int16:
    case T::Int32:
    case T::Int64:
        return value.toLongLong();
    case T::UInt64:
    case T::Float:
    case T::Double:
        return value.toDouble();
    default:
        break;
    }
    return {};
}

QVariant toVariant(const QJsonValue &value, RegisterAddress::ValType type)
{
    using T = RegisterAddress::ValType;
    switch (type) {
    case T::Bool:
        return value.toBool();
    case T::UInt8:
    case T::UInt16:
    case T::Int8:
    case T::Int16:
    case T::Int32:
        return value.toInt();
    case T::UInt32:
    case T::Int64:
    case T::UInt64:
    case T::Float:
    case T::Double:
        return value.toDouble();
    default:
        break;
    }
    return {};
}

ConnectionParams toConnectionParams(
    const QString &address, const QString &serverId, QString *error)
{
    error->clear();
    if (address.isEmpty()) {
        *error = QObject::tr("для устройства с идентификатором '%0' не "
                             "определён адрес и тип транспорта")
                     .arg(serverId);
        return ConnectionParams();
    }
    QStringList addressParts = address.split(":");

    if (addressParts.at(0) == "tcp") {
        return handleTcpConnectionParam(addressParts, serverId, error);
    } else if (addressParts.at(0) == "serial_rtu") {
        return handleSerialConnectionParam(addressParts, serverId, error);
    } else {
        *error = QObject::tr("для устройства с идентификатором '%0' не "
                             "определён адрес и тип транспорта")
                     .arg(serverId);
        return ConnectionParams();
    }
}

QString toString(const ConnectionParams &params)
{
    switch (params.type) {
    case ConnectionParams::Type::RtuSerial:
        return toStringRtu(params);
    case ConnectionParams::Type::Tcp:
        return toStringTcp(params);
    }
    return {};
}

QString checkRegisterAddress(const RegisterAddress &address)
{
    auto result = checkRegisterAddress(address.regType, address.regAddress);
    if (!result.isEmpty()) {
        return result;
    }
    int numSymbol{};
    switch (address.valType) {
    case RegisterAddress::ValType::Bool:
    case RegisterAddress::ValType::Int8:
    case RegisterAddress::ValType::UInt8:
        numSymbol = 0;
        break;
    case RegisterAddress::ValType::Int16:
    case RegisterAddress::ValType::UInt16:
        numSymbol = 2;
        break;
    case RegisterAddress::ValType::Int32:
    case RegisterAddress::ValType::UInt32:
    case RegisterAddress::ValType::Float:
        numSymbol = 4;
        break;
    case RegisterAddress::ValType::Int64:
    case RegisterAddress::ValType::UInt64:
    case RegisterAddress::ValType::Double:
        numSymbol = 8;
        break;
    case RegisterAddress::ValType::Unknown:
        return QObject::tr("Внутрення ошибка - указан неизвестный тип значений");
    }

    if (!address.typeOrder.isEmpty()) {
        QString error = checkValOrder(address.typeOrder, address.valType, numSymbol);
        if (!error.isEmpty()) {
            return error;
        }
    }
    return {};
}

QString checkSensorMode(RegisterAddress::RegisterType type, Sensor::Mode mode)
{
    if (mode == Sensor::Mode::Read) {
        return {};
    }
    switch (type) {
    case RegisterAddress::RegisterType::DiscreteInputContacts:
    case RegisterAddress::RegisterType::AnalogOutputHoldingRegisters:
        return {};
    default:
        break;
    }
    return QObject::tr("Режим датчика для устрйоства с типом регистра %0 не может быть %1")
      .arg(toHumanString(type), toHumanString(mode));
}

QString toHumanString(Sensor::Mode mode)
{
    switch (mode) {
    case Sensor::Mode::Read:
        return "Read";
    case Sensor::Mode::ReadWrite:
        return "ReadWrite";
    case Sensor::Mode::Write:
        return "Write";
    }
    return {};
}

QString toString(Sensor::Mode mode)
{
    switch (mode) {
    case Sensor::Mode::Read:
        return "r";
    case Sensor::Mode::ReadWrite:
        return "rw";
    case Sensor::Mode::Write:
        return "w";
    }
    return {};
}

void setComboboxBasedOnValue(QComboBox *combobox, int data) {
    int index = combobox->findData(data);
    if (index >= 0) {
        combobox->setCurrentIndex(index);
    }
}

} // namespace ModbusConfig
