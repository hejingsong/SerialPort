#ifndef SYSTEMPORT_H
#define SYSTEMPORT_H

#include <QString>
#include <QWidget>
#include <QSerialPort>
#include <iostream>

struct PortStruct {
public:
    PortStruct( const QString &portName,
                const QSerialPort::BaudRate rate = QSerialPort::Baud9600,
                const QSerialPort::DataBits dataBit = QSerialPort::Data8,
                const QSerialPort::Parity parity = QSerialPort::NoParity,
                const QSerialPort::StopBits stopBit = QSerialPort::OneStop,
                const QSerialPort::FlowControl control = QSerialPort::NoFlowControl)
        : portName(portName),
          rate(rate),
          dataBit(dataBit),
          parity(parity),
          stopBit(stopBit),
          flowControl(control) {}
    QString portName;
    QSerialPort::BaudRate rate;
    QSerialPort::DataBits dataBit;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBit;
    QSerialPort::FlowControl flowControl;
};

class SystemPort
{
public:
    SystemPort(QWidget *parent);
    ~SystemPort();
    int send(const QString &msg, bool send_hex);
    QByteArray recv(void);
    bool openPort( const PortStruct& portStruct );
    bool closePort();
    inline QString getCurrentPortName() {
        if (this->currentPort == nullptr ) return "";
        return this->currentPort->portName();
    }
    inline bool isReadyRead() {
        if (this->currentPort==nullptr) return false;
        return this->currentPort->isReadable();
    }
    inline const QStringList &getExistPort() {
        this->getSystemPort();
        return this->portNames;
    }
private:
    const QString DevicePath = "HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\SERIALCOMM\\";
    QStringList portNames;
    QSerialPort *currentPort;
    QWidget *parent;
private:
    void    getSystemPort();
    QString getComm( const int index );
};

#endif // SYSTEMPORT_H
