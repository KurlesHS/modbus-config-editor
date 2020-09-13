#pragma once

#include <QVariant>
#include <QString>
#include <QUuid>

namespace ModbusConfig {

struct Settings {
    QString upakServeUrl;
    QString upakUserName;
    QString upakPassword;
    int writeRequestTtl;
};


struct RegisterAddress {
    enum class RegisterType {
        Unknown,
        DiscreteOutputCoils,
        DiscreteInputContacts,
        AnalogInputRegisters,
        AnalogOutputHoldingRegisters
    };

    enum class ValType {
        Unknown,
        Bool,
        Int8,
        UInt8,
        Int16,
        UInt16,
        Int32,
        UInt32,
        Int64,
        UInt64,
        Float,
        Double
    };

    quint8 slaveAddress;
    int regAddress;
    QString typeOrder;
    ValType valType;
    RegisterType regType;
};

struct SensorsMap {
    QString id;
    RegisterAddress registеrAddress;
    int valueCount{};
    QVariant defaultValue;
};

struct ConnectionParams {
    enum class Type {
        Tcp,
        RtuSerial
    };

    enum class FlowControl {
        NoFlowControl,
        HardwareControl,
        SoftwareControl,
        UnknownFlowControl
    };

    enum class Parity {
        NoParity,
        EvenParity,
        OddParity,
        SpaceParity,
        MarkParity,
        UnknownParity
    };

    enum class StopBits {
        OneStop,
        OneAndHalfStop,
        TwoStop,
        UnknownStopBits
    };

    Type type;

    // tcp
    QString address;
    quint16 port{};

    // rtu serial
    QString deviceName;
    quint32 baudrate{};
    quint8 databits{};
    FlowControl flowControl;
    Parity parity;
    StopBits stopBits{};
};


struct Sensor {
    enum class Mode {
        Read,
        Write,
        ReadWrite
    };

    enum class Type {
        Separate,
        Map
    };

    QUuid id;
    QString description;
    Type type;
    RegisterAddress registerAddress;
    QString correctFunction;
    Mode mode;
    double updateThreshold{};
    QVariant minValue;
    QVariant maxValue;

    QString mapId;
    int mapOffset{};
};

struct DeviceSettings {
    QUuid id;
    QString description;
    ConnectionParams connectionParams;
};

struct Device {
    DeviceSettings settings;
    QHash<QString, SensorsMap> maps;
    QHash<QUuid, Sensor> sensors;
};

}

Q_DECLARE_METATYPE(ModbusConfig::Settings)
Q_DECLARE_METATYPE(ModbusConfig::Device)
Q_DECLARE_METATYPE(ModbusConfig::Sensor)
Q_DECLARE_METATYPE(ModbusConfig::SensorsMap)
