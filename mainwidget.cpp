#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    //this->setWindowFlag(Qt::FramelessWindowHint);
    QScreen* screen = QGuiApplication::screens().at(0);
    QRect screenRect = screen->availableGeometry();
    this->resize(screenRect.width()*2/3, screenRect.height()*2/3);

    title.setFixedHeight(50);
    title.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    splitTitle->addWidget(&title);
    splitTitle->addWidget(mainW);
    splitTitle->setMargin(0);
    splitTitle->setSpacing(1);

    title.setStyleSheet("background-color: rgb(47, 53, 66)");
    mainW->setStyleSheet("background-color: white");
    ecgiiPlot.setStyleSheet("background-color: rgb(47, 53, 66)");
    spo2Plot.setStyleSheet("background-color: rgb(47, 53, 66)");
    bpPlot.setStyleSheet("background-color: rgb(47, 53, 66)");
    hr.setStyleSheet("background-color: rgb(47, 53, 66)");
    st.setStyleSheet("background-color: rgb(47, 53, 66)");
    nibp.setStyleSheet("background-color: rgb(47, 53, 66)");
    spo2.setStyleSheet("background-color: rgb(47, 53, 66)");
    temp.setStyleSheet("background-color: rgb(47, 53, 66)");
    resp.setStyleSheet("background-color: rgb(47, 53, 66)");
    co2.setStyleSheet("background-color: rgb(47, 53, 66)");

    leftrightLayout->addWidget(left);
    leftrightLayout->addWidget(right);
    leftrightLayout->setStretch(0, 2);
    leftrightLayout->setStretch(1, 1);
    leftrightLayout->setMargin(0);
    leftrightLayout->setSpacing(1);
    leftPlotsLayout->addWidget(&ecgiiPlot);
    leftPlotsLayout->addWidget(&spo2Plot);
    leftPlotsLayout->addWidget(&bpPlot);
    leftPlotsLayout->setMargin(0);
    leftPlotsLayout->setSpacing(1);

    rightGrids->addWidget(&hr,0, 0, 1, 1);
    rightGrids->addWidget(&st,0, 1, 1, 1);
    rightGrids->addWidget(&nibp,1, 0, 1, 2);
    rightGrids->addWidget(&spo2,2, 0, 1, 1);
    rightGrids->addWidget(&temp,2, 1, 1, 1);
    rightGrids->addWidget(&resp,3, 0, 1, 1);
    rightGrids->addWidget(&co2,3, 1, 1, 1);
    rightGrids->setMargin(0);
    rightGrids->setSpacing(1);
    rightGrids->setRowStretch(0, 1);
    rightGrids->setRowStretch(1, 1);
    rightGrids->setRowStretch(2, 1);
    rightGrids->setRowStretch(3, 1);
    rightGrids->setColumnStretch(1, 1);
    rightGrids->setColumnStretch(0, 1);

    left->setLayout(leftPlotsLayout);
    right->setLayout(rightGrids);
    mainW->setLayout(leftrightLayout);

    this->setLayout(splitTitle);

    connect(&title, &TitleBar::enablefullscreen, this, &MainWidget::setFull);
    connect(&title, &TitleBar::disablefullscreen, this, &MainWidget::setNotFull);

    //测试区
    ecgiiPlot.y0 = 0;
    ecgiiPlot.yScale = 4096;
    ecgiiPlot.xScale = 1000;
    ecgiiPlot.updateInterval = 1;
    ecgiiPlot.color = Qt::green;
    ecgiiPlot.text = "ECG";
    ecgiiPlot.plotflag = 1;

    spo2Plot.y0 = 0;
    spo2Plot.yScale = 300;
    spo2Plot.xScale = 1000;
    spo2Plot.updateInterval = 1;
    spo2Plot.color = Qt::red;
    spo2Plot.text = "SPO2";
    spo2Plot.plotflag = 2;

    bpPlot.y0 = 0;
    bpPlot.yScale = 300;
    bpPlot.xScale = 1000;
    bpPlot.updateInterval = 1;
    bpPlot.color = Qt::yellow;
    bpPlot.text = "BP";
    bpPlot.plotflag = 3;

    connect(gs, &GetSerial::receivedECGdata, &ecgiiPlot, &PlotWidget::sendData);
    connect(gs, &GetSerial::receivedBPdata, &bpPlot, &PlotWidget::sendData);
    connect(gs, &GetSerial::receivedSPO2data, &spo2Plot, &PlotWidget::sendData);

    connect(gs, &GetSerial::receivedHRdata, &hr, &HRWidget::setData);

    connect(gs, &GetSerial::receivedST1data, &st, &STWidget::setST1Num);
    connect(gs, &GetSerial::receivedST2data, &st, &STWidget::setST2Num);
    connect(gs, &GetSerial::receivedPVCdata, &st, &STWidget::setPVCNum);

    connect(gs, &GetSerial::receivedLBPdata, &nibp, &NIBPWidget::setLowPa);
    connect(gs, &GetSerial::receivedHBPdata, &nibp, &NIBPWidget::setHighPa);
    connect(gs, &GetSerial::receivedIBPdata, &nibp, &NIBPWidget::setAverage);

    connect(gs, &GetSerial::receivedSPO2Wdata, &spo2, &SPO2Widget::setDataNum);
    connect(gs, &GetSerial::receivedBPMdata, &spo2, &SPO2Widget::setBPMNum);

    connect(gs, &GetSerial::receivedT1data, &temp, &TEMPWidget::setT1Data);
    connect(gs, &GetSerial::receivedT2data, &temp, &TEMPWidget::setT2Data);
    connect(gs, &GetSerial::receivedTDdata, &temp, &TEMPWidget::setTDData);

    connect(gs, &GetSerial::receivedRESPdata, &resp, &RESPWidget::setData);

    connect(gs, &GetSerial::receivedCO2data, &co2, &CO2Widget::setCO2Num);
    connect(gs, &GetSerial::receivedAWRRdata, &co2, &CO2Widget::setAWRRNum);
    connect(gs, &GetSerial::receivedINSdata, &co2, &CO2Widget::setINSNum);

    gs->connectPort();

    connect(&title, &TitleBar::middleTCP, this, &MainWidget::setTCPIP);

    connect(tcpSendTimer, &QTimer::timeout, this, &MainWidget::sendTCPOthers);
    connect(socket, &TCPSocket::dealMessage, this, &MainWidget::TCPGetMessage);

    connect(&ecgiiPlot, &PlotWidget::dataFulledForTCP, this, &MainWidget::sendTCPWaves);
    connect(&spo2Plot, &PlotWidget::dataFulledForTCP, this, &MainWidget::sendTCPWaves);
    connect(&bpPlot, &PlotWidget::dataFulledForTCP, this, &MainWidget::sendTCPWaves);

}

MainWidget::~MainWidget()
{
    socket->close();
}

void MainWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
}

void MainWidget::setFull()
{
    this->showFullScreen();
}

void MainWidget::setNotFull()
{
    this->showNormal();
}

void MainWidget::setTCPIP(QString ip, quint16 mport)
{
    ipAddr = ip;
    port = mport;
    socket->abort();
    socket->connectToServer(ipAddr, port);
    socket->SendMessage(QString("facility,%1").arg(facilitySerialNum).toUtf8());
    tcpSendTimer->start(1000);
    connected = true;
    //tcpWaveSendTimer->start(10000);
}

void MainWidget::sendTCPOthers()
{
    if(counterX == 1)
    {
        counterX++;
        socket->SendMessage(QString("hr,%1,%2").arg(facilityName).arg(hr.dataForTCPSend).toUtf8());
    }
    else if(counterX == 2)
    {
        counterX++;
        socket->SendMessage(QString("st,%1,%2,%3,%4").arg(facilityName)
                        .arg(st.dataForTCPSend1)
                        .arg(st.dataForTCPSend2)
                        .arg(st.dataForTCPSend3).toUtf8());
    }
    else if(counterX == 3)
    {
        counterX++;
    socket->SendMessage(QString("nibp,%1,%2,%3,%4").arg(facilityName)
                        .arg(nibp.dataForTCPSend1)
                        .arg(nibp.dataForTCPSend2)
                        .arg(nibp.dataForTCPSend3).toUtf8());
    }

    else if(counterX == 4)
    {
        counterX++;
    socket->SendMessage(QString("spo2,%1,%2,%3").arg(facilityName)
                        .arg(spo2.dataForTCPSend1)
                        .arg(spo2.dataForTCPSend2).toUtf8());
    }
    else if(counterX == 5)
    {
        counterX++;
    socket->SendMessage(QString("temp,%1,%2,%3,%4").arg(facilityName)
                        .arg(temp.dataForTCPSend1)
                        .arg(temp.dataForTCPSend2)
                        .arg(temp.dataForTCPSend3).toUtf8());
    }
    else if(counterX == 6)
    {
        counterX++;
    socket->SendMessage(QString("resp,%1,%2").arg(facilityName)
                        .arg(resp.dataForTCPSend).toUtf8());
    }
    else if(counterX == 7)
    {
        counterX = 1;
    socket->SendMessage(QString("co2,%1,%2,%3,%4").arg(facilityName)
                        .arg(co2.dataForTCPSend1)
                        .arg(co2.dataForTCPSend2)
                        .arg(co2.dataForTCPSend3).toUtf8());
    }
}

void MainWidget::sendTCPWaves(int flag)
{
    if(connected)
    {
        QString strForTCP = "";
        TCPSocket *socket2 = new TCPSocket;
        TCPSocket *socket3 = new TCPSocket;
        TCPSocket *socket4 = new TCPSocket;
        socket2->connectToServer(ipAddr, port);
        socket3->connectToServer(ipAddr, port);
        socket4->connectToServer(ipAddr, port);
        if(flag == 1)
        {
            strForTCP= QString("ecgplot,%1,").arg(facilityName);
            qDebug() << facilityName;
            qDebug() << strForTCP;
            for(int i=0;i<ecgiiPlot.dataForTCP.size();i++)
            {
                strForTCP.append(QString("%1").arg(ecgiiPlot.dataForTCP.at(i)));
                if(i < ecgiiPlot.dataForTCP.size() - 1)
                {
                    strForTCP.append(".");
                }
            }
            socket2->SendMessage(strForTCP.toUtf8());
            socket2->close();
            ecgiiPlot.dataForTCP.clear();
            qDebug() <<"success";
        }
        else if(flag == 2)
        {
            strForTCP= QString("spo2plot,%1,").arg(facilityName);
            for(int i=0;i<spo2Plot.dataForTCP.size();i++)
            {
                strForTCP.append(QString("%1;").arg(spo2Plot.dataForTCP.at(i)));
                if(i < spo2Plot.dataForTCP.size() - 1)
                {
                    strForTCP.append(".");
                }
            }
            socket3->SendMessage(strForTCP.toUtf8());
            socket3->close();
            spo2Plot.dataForTCP.clear();
        }
        else if(flag == 3)
        {
            strForTCP= QString("bpplot,%1,").arg(facilityName);
            for(int i=0;i<bpPlot.dataForTCP.size();i++)
            {
                strForTCP.append(QString("%1;").arg(bpPlot.dataForTCP.at(i)));
                if(i < bpPlot.dataForTCP.size() - 1)
                {
                    strForTCP.append(".");
                }
            }
            socket4->SendMessage(strForTCP.toUtf8());
            socket4->close();
            bpPlot.dataForTCP.clear();
        }
    }
}

void MainWidget::TCPGetMessage(QByteArray ba)
{
    QString msg = QString(ba);
    QStringList msgList = msg.split(",");
    if(msgList[0] == "faciID")
    {
        facilityName = msgList[1];
        qDebug() << facilityName;
    }
}

