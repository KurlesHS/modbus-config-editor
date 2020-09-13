#include "serializer.h"
#include "modbusconfigmodel.h"
#include "modbusconfigeditormainwindow.h"
#include "modbusconfigeditorcontroller.h"

#include <QFile>
#include <QDebug>
#include <QApplication>
#include <QJsonDocument>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ModbusConfigEditorMainWindow view;
    using namespace ModbusConfig;

#if 0
    Serializer serializer;
    QString error;
    QFile f("d:/upak-server-modbus.conf");
    qDebug() << f.open(QIODevice::ReadOnly);
    auto res = serializer.deserialize(QJsonDocument::fromJson(f.readAll()).object(), &error);

    auto json = serializer.serialize(res);

    auto text = QString::fromUtf8(QJsonDocument(json).toJson());
    view.setJson(text);

#endif
    ModbusConfigModel model;
    ModbusConfigEditorController controller(&view, &model);
    view.show();
    return a.exec();
}
