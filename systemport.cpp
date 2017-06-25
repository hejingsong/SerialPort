#include <QSettings>
#include <qt_windows.h>
#include "config.h"

SystemPort::SystemPort(QWidget *parent)
    : parent(parent)
{
    this->currentPort = nullptr;
}

SystemPort::~SystemPort() {
    this->closePort();
}

int SystemPort::send( const QString &msg, bool send_hex ) {
    char *data = NULL;
    QByteArray temp = msg.toLocal8Bit();
    if ( !send_hex ) {
        data = temp.data();
    }else {
        data = temp.toHex().data();
    }
    return this->currentPort->write(data);
}

QByteArray SystemPort::recv(void) {
    return this->currentPort->readAll();
}

/**
 * @brief SystemPort::getSystemPort
 * 获取系统注册表中可用的串口
 */
void SystemPort::getSystemPort() {
    int size = 0, i;
    QString value;
    QSettings *setting = new QSettings(this->DevicePath, QSettings::NativeFormat);
    QStringList keys = setting->allKeys();

    size = keys.size();
    for ( i=0; i<size; i++ ) {
        value = this->getComm( i );
        if ( value.length() == 0 )
            continue;
        this->portNames.append(value);
    }
}

QString SystemPort::getComm( const int index ) {
    HKEY hKey;
    DWORD keysize, type, valuesize;
    wchar_t keyname[256];
    char keyvalue[256];
    QString message;
    unsigned int i;

    if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hKey) != 0 ) {
#if Debug
        qDebug() << "open regedit fail.";
#endif
        return "";
    }

    keysize = sizeof(keyname);
    valuesize = sizeof(keyvalue);
    if ( RegEnumValue( hKey, index, keyname, &keysize, 0, &type, (BYTE*)keyvalue, &valuesize ) != 0 ) {
#if Debug
        qDebug() << "read key fail.";
#endif
        goto ERR;
    }

    for ( i=0; i<valuesize; i++ ) {
        if ( keyvalue[i] != 0x00 )
            message.append( keyvalue[i] );
    }
    return message;
ERR:
    RegCloseKey(hKey);
    return "";
}

bool SystemPort::openPort( const PortStruct &portStruct ) {
    if ( portStruct.portName.length() == 0 ) {
        return false;
    }
    this->currentPort = new QSerialPort(this->parent);
    this->currentPort->setPortName(portStruct.portName);
    this->currentPort->open(QIODevice::ReadWrite);
    this->currentPort->setBaudRate(portStruct.rate);
    this->currentPort->setDataBits(portStruct.dataBit);
    this->currentPort->setParity(portStruct.parity);
    this->currentPort->setStopBits(portStruct.stopBit);
    this->currentPort->setFlowControl(portStruct.flowControl);
    /* 当数据准备好的时候 */
    this->parent->connect(this->currentPort, SIGNAL(readyRead()), this->parent, SLOT(recvEvent()));
#if Debug
        qDebug() << "portName: " << portStruct.portName << " rate: " << portStruct.rate << " dataBit: " << portStruct.dataBit
                 << " parity: " << portStruct.parity << " stopBit: " << portStruct.stopBit << "flow: " << portStruct.flowControl;
#endif
    return this->currentPort->isOpen();
}

bool SystemPort::closePort() {
    if ( this->currentPort == nullptr )
        return true;
    this->currentPort->close();
    this->currentPort = nullptr;
    return true;
}
