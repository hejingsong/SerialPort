#include <QStatusBar>
#include <QTimerEvent>
#include <QSerialPort>
#include "config.h"

SerialPort::SerialPort(QWidget *parent)
    : QWidget(parent)
{
    this->init();
}

SerialPort::~SerialPort()
{
#if Debug
    qDebug() << "application exit.";
#endif
}

void SerialPort::bindPort( SystemPort* port ) {
    QStringList ports;

    this->port = port;
    ports = this->port->getExistPort();
    this->fillComm(ports);
}

void SerialPort::init() {
    this->rx_num = 0;
    this->tx_num = 0;
    this->is_open = false;
    this->enable_send_hex = false;
    this->enable_show_hex = false;
    this->enable_show = true;
    this->setWindowTitle(this->titleName);
    this->setMinimumSize(900, 600);

    this->initWidget();
    this->initStatusBar();
    this->fillContent();
    this->setLayout(this->mainLayout);
    this->initConnect();
}

void SerialPort::initWidget() {
    int i;
    this->mainLayout = new QVBoxLayout;
    this->topLayout = new QHBoxLayout;
    this->bottomLayout = new QHBoxLayout;
    this->topLeftLayout = new QGridLayout;
    this->bottomLeftLayout = new QGridLayout;

    this->labels = new QLabel[7];
    this->imgShow = new QLabel(this);
    this->serialPortSelect = new QComboBox(this);
    this->rateSelect = new QComboBox(this);
    this->checkBitSelect = new QComboBox(this);
    this->dataBitSelect = new QComboBox(this);
    this->stopBitSelect = new QComboBox(this);
    this->flowControlSelect = new QComboBox(this);
    this->openSerialBtn = new QPushButton(tr("打开串口"), this);
    this->clearShowBtn = new QPushButton(tr("清除显示"), this);
    this->stopShowBtn = new QPushButton(tr("停止显示"), this);
    this->hexShowBox = new QCheckBox(tr("Hex显示"), this);
    this->messageEdit = new QTextEdit(this);
    this->autoSendBox = new QCheckBox(tr("自动发送"), this);
    this->hexSendBox = new QCheckBox(tr("Hex发送"), this);
    this->timerEdit = new QLineEdit(tr("1000"), this);
    this->senderEdit = new QTextEdit(this);
    this->sendBtn = new QPushButton(tr("发送"), this);

    this->messageEdit->setReadOnly(true);
    this->labels[6].setParent(this);
    this->labels[6].setText(this->labelInfos[6]);
    this->imgShow->setPixmap(QPixmap(":/img/light_off.png"));
    this->timerEdit->setMinimumWidth(50);
    this->timerEdit->setMaximumWidth(90);
    this->senderEdit->setMaximumHeight(80);

    for ( i=0; i<6; i++ ) {
        this->labels[i].setText(this->labelInfos[i]);
        this->labels[i].setParent(this);
        this->labels[i].setFixedSize(this->labels[i].sizeHint());
        this->topLeftLayout->addWidget(&this->labels[i], i, 0, Qt::AlignRight);
    }
    this->topLeftLayout->addWidget(this->serialPortSelect, 0, 1);
    this->topLeftLayout->addWidget(this->rateSelect, 1, 1);
    this->topLeftLayout->addWidget(this->checkBitSelect, 2, 1);
    this->topLeftLayout->addWidget(this->dataBitSelect, 3, 1);
    this->topLeftLayout->addWidget(this->stopBitSelect, 4, 1);
    this->topLeftLayout->addWidget(this->flowControlSelect, 5, 1);
    this->topLeftLayout->addWidget(this->imgShow, 6, 0, Qt::AlignRight);
    this->topLeftLayout->addWidget(this->openSerialBtn, 6, 1);
    this->topLeftLayout->addWidget(this->hexShowBox, 7, 1, Qt::AlignLeft);
    this->topLeftLayout->addWidget(this->clearShowBtn, 9, 0);
    this->topLeftLayout->addWidget(this->stopShowBtn, 9, 1);
    this->topLeftLayout->setRowStretch(8, 1);
    this->topLeftLayout->setContentsMargins(0, 0, 0, 0);
    this->topLeftLayout->setColumnMinimumWidth(0, 20);

    this->topLayout->setContentsMargins(5, 5, 5, 5);
    this->topLayout->addLayout(this->topLeftLayout);
    this->topLayout->addWidget(this->messageEdit);

    this->bottomLeftLayout->addWidget(this->autoSendBox, 0, 1);
    this->bottomLeftLayout->addWidget(this->hexSendBox, 1, 1);
    this->bottomLeftLayout->addWidget(&this->labels[6], 2, 0);
    this->bottomLeftLayout->addWidget(this->timerEdit, 2, 1);
    this->bottomLeftLayout->addWidget(this->sendBtn, 3, 0, 1, 2);

    this->bottomLayout->setContentsMargins(5, 0, 5, 5);
    this->bottomLayout->addLayout(this->bottomLeftLayout);
    this->bottomLayout->addWidget(this->senderEdit, Qt::AlignLeft);

    this->mainLayout->setContentsMargins(0, 0, 0, 0);
    this->mainLayout->addLayout(this->topLayout);
    this->mainLayout->addLayout(this->bottomLayout);
}

void SerialPort::initStatusBar() {
    this->statusLayout = new QHBoxLayout;
    this->portStatusLabel = new QLabel(tr("Status: COM is close"), this);
    this->txStatusLabel = new QLabel(tr("Tx: 0"), this);
    this->rxStatusLabel = new QLabel(tr("Rx: 0"), this);

    this->statusLayout->addWidget(this->portStatusLabel, 1);
    this->statusLayout->addWidget(this->rxStatusLabel);
    this->statusLayout->addWidget(this->txStatusLabel);

    this->statusLayout->setContentsMargins(5, 0, 5, 5);

    this->mainLayout->addLayout(this->statusLayout);
}

void SerialPort::initConnect() {
    connect(this->openSerialBtn, SIGNAL(clicked(bool)), this, SLOT(openSerial()));
    connect(this->hexShowBox, SIGNAL(stateChanged(int)), this, SLOT(showHex(int)));
    connect(this->clearShowBtn, SIGNAL(clicked(bool)), this, SLOT(clearWindow()));
    connect(this->stopShowBtn, SIGNAL(clicked(bool)), this, SLOT(stopWindowShow()));
    connect(this->autoSendBox, SIGNAL(stateChanged(int)), this, SLOT(autoSend(int)));
    connect(this->hexSendBox, SIGNAL(stateChanged(int)), this, SLOT(sendHex(int)));
    connect(this->sendBtn, SIGNAL(clicked(bool)), this, SLOT(sendEvent()));
}

void SerialPort::fillContent() {
    this->rateSelect->addItems(this->rates);
    this->rateSelect->setCurrentText("9600");
    this->checkBitSelect->addItems(this->checks);
    this->checkBitSelect->setCurrentText("None");
    this->dataBitSelect->addItems(this->dataBits);
    this->dataBitSelect->setCurrentText("8");
    this->stopBitSelect->addItems(this->stopBits);
    this->stopBitSelect->setCurrentText("1");
    this->flowControlSelect->addItems(this->controls);
    this->flowControlSelect->setCurrentText("NoFlowControl");
}

void SerialPort::fillComm(const QStringList ports) {
    this->serialPortSelect->addItems(ports);
}

void SerialPort::setPortStatus( const QString& status ) {
    this->portStatusLabel->setText( "status: "+status );
}

void SerialPort::setNum(const int rx, const int tx) {
    this->rxStatusLabel->setText("Rx: "+QString::number(rx));
    this->txStatusLabel->setText("Tx: "+QString::number(tx));
}


void SerialPort::openSerial() {
    if ( is_open == false ) {
        int parity, control;
        QString portName = this->serialPortSelect->currentText();
        long rate = this->rateSelect->currentText().toInt();
        QString s_parity = this->checkBitSelect->currentText();
        QString s_control = this->flowControlSelect->currentText();
        int dataBit = this->dataBitSelect->currentText().toInt();
        float stopBit = this->stopBitSelect->currentText().toFloat();

        if ( s_parity == "NoParity" )
            parity = 0;
        else if ( s_parity == "EvenParity" )
            parity = 2;
        else if ( s_parity == "OddParity" )
            parity = 3;

        if ( s_control == "NoFlow" )
            control = 0;
        else if ( s_control == "Hardware" )
            control = 1;
        else if ( s_control == "Software" )
            control = 2;

        stopBit = (stopBit == 1.5)?3:stopBit;
        PortStruct portStruct(
                    portName,
                    QSerialPort::BaudRate(rate),
                    QSerialPort::DataBits(dataBit),
                    QSerialPort::Parity(parity),
                    QSerialPort::StopBits( int(stopBit) ),
                    QSerialPort::FlowControl(control)
                    );
        if ( this->port->openPort(portStruct) ) {
            this->imgShow->setPixmap(QPixmap(":/img/light_on.png"));
            this->openSerialBtn->setText(tr("关闭串口"));
            this->is_open = true;
            this->setPortStatus( this->port->getCurrentPortName()+" is open" );
        }else {
            this->setPortStatus( this->port->getCurrentPortName()+" open fail" );
        }
    }else {
        this->setPortStatus( this->port->getCurrentPortName()+" is close" );
        this->port->closePort();
        this->imgShow->setPixmap(QPixmap(":/img/light_off.png"));
        this->openSerialBtn->setText(tr("打开串口"));
        this->is_open = false;
    }
}

QString SerialPort::getEditerText() {
    QString msg = this->senderEdit->toPlainText();
    if ( msg.length() == 0 )
        return "";
    return msg;
}

void SerialPort::showHex(int state) {
    if ( state == 2 ) {
        this->enable_show_hex = true;
    }else {
        this->enable_show_hex = false;
    }
}

void SerialPort::clearWindow() {
    this->messageEdit->clear();
}

void SerialPort::stopWindowShow() {
    if ( this->enable_show ) {
        this->enable_show = false;
        this->stopShowBtn->setText(tr("开始显示"));
    }else {
        this->enable_show = true;
        this->stopShowBtn->setText(tr("停止显示"));
    }
}

void SerialPort::autoSend(int state) {
    if ( state == 2 ) { // 打勾情况
        QString time = this->timerEdit->text();
        this->timerId = this->startTimer(time.toInt());
    }else {
        killTimer(this->timerId);
    }
}

void SerialPort::sendHex(int state) {
    if ( state == 2 ) {
        this->enable_send_hex = true;
    }else {
        this->enable_send_hex = false;
    }
}

void SerialPort::sendEvent() {
    if ( !this->is_open )
        return;
    QString msg;
    msg = this->getEditerText();
    if ( msg.length() == 0 )
        return;
    // 在send函数中进行编码转换
    this->port->send(msg, this->enable_send_hex);
    this->showCommunicate(true, msg);
}
/* 接受事件在串口数据准备好的时候执行 在systemport中连接槽 */
void SerialPort::recvEvent() {
    QByteArray bytes = this->port->recv();

    if ( bytes.size() == 0 ) return;
    // 在单片机中一般使用的是8位的Ascii, 所以这里采用8位编码就不会出现乱码情况
    if ( !this->enable_show_hex ) {
        this->showCommunicate(false, QString::fromLocal8Bit(bytes));
    }else {
        // 显示hex
        this->showCommunicate(false, QString::fromLocal8Bit( bytes.toHex() ));
    }
}

void SerialPort::showCommunicate(bool status, const QString &msg) {
    if ( status )
        this->tx_num += msg.length();
    else{
        this->rx_num += msg.length();
        if ( this->enable_show )
            this->messageEdit->insertPlainText(msg);
    }
    this->setNum( this->rx_num, this->tx_num );
}

void SerialPort::timerEvent(QTimerEvent *event) {
    this->sendEvent();
    event->accept();
}
