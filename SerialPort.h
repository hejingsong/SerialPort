#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QLabel>
#include <QWidget>
#include <QLayout>
#include <QCheckBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

#include "systemport.h"

class SerialPort : public QWidget
{
    Q_OBJECT

public:
    SerialPort(QWidget *parent = 0);
    ~SerialPort();
    void            bindPort(SystemPort *port );
private:
    QVBoxLayout     *mainLayout;
    QHBoxLayout     *topLayout;
    QHBoxLayout     *bottomLayout;
    QGridLayout     *topLeftLayout;
    QGridLayout     *bottomLeftLayout;
    QHBoxLayout     *statusLayout;

    QLabel          *labels;
    QLabel          *imgShow;
    QComboBox       *serialPortSelect;
    QComboBox       *rateSelect;
    QComboBox       *checkBitSelect;
    QComboBox       *dataBitSelect;
    QComboBox       *stopBitSelect;
    QComboBox       *flowControlSelect;
    QPushButton     *openSerialBtn;
    QPushButton     *clearShowBtn;
    QPushButton     *stopShowBtn;
    QCheckBox       *hexShowBox;
    QTextEdit       *messageEdit;
    QCheckBox       *autoSendBox;
    QCheckBox       *hexSendBox;
    QLineEdit       *timerEdit;
    QTextEdit       *senderEdit;
    QPushButton     *sendBtn;
    QLabel          *portStatusLabel;
    QLabel          *txStatusLabel;
    QLabel          *rxStatusLabel;

    SystemPort      *port;
private:
    const QString titleName = "串口助手";
    int rx_num;
    int tx_num;
    int timerId;
    bool is_open;
    bool enable_show;
    bool enable_send_hex;
    bool enable_show_hex;
    const QStringList labelInfos = {
        "串　口",
        "波特率",
        "校验位",
        "数据位",
        "停止位",
        "硬件流",
        "发送周期(ms)",
    };
    const QStringList rates = {
        "110",
        "300",
        "600",
        "1200",
        "2400",
        "4800",
        "9600",
        "14400",
        "19200",
        "38400",
        "43000",
        "56000",
        "57600",
        "115200",
        "128000",
        "256000"
    };
    const QStringList checks = {
        "NoParity",
        "EvenParity",
        "OddParity"
    };
    const QStringList dataBits = {
        "5",
        "6",
        "7",
        "8"
    };
    const QStringList stopBits = {
        "1",
        "1.5",
        "2"
    };
    const QStringList controls = {
        "NoFlow",
        "Hardware",
        "Software"
    };
private:
    void        init();
    void        initWidget();
    void        initConnect();
    void        fillContent();
    void        initStatusBar();
    QString     getEditerText();
    void        fillComm(const QStringList ports);
    void        setPortStatus( const QString& status );
    void        setNum(const int rx, const int tx);
private slots:
    void        openSerial();
    void        showHex(int);
    void        clearWindow();
    void        stopWindowShow();
    void        autoSend(int);
    void        sendHex(int);
    void        sendEvent();
    void        recvEvent();
    void        showCommunicate(bool status, const QString &msg);
    void        timerEvent(QTimerEvent *event);
};

#endif // SERIALPORT_H
