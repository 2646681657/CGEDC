#include "mainwindow.h"

#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QSizePolicy>
#include <QFont>
#include <QStatusBar>
#include <QScrollArea>
#include <QThread>

// ── 小工具：带标题的数值标签对 ──────────────────────────────────────
static QWidget *makeDataItem(const QString &title, QLabel *&valueOut,
                             const QString &initVal = "--")
{
    QWidget *w = new QWidget;
    QVBoxLayout *vl = new QVBoxLayout(w);
    vl->setContentsMargins(4, 2, 4, 2);
    vl->setSpacing(1);

    QLabel *titleLbl = new QLabel(title);
    titleLbl->setObjectName("titleLabel");
    titleLbl->setAlignment(Qt::AlignCenter);

    valueOut = new QLabel(initVal);
    valueOut->setObjectName("valueLabel");
    valueOut->setAlignment(Qt::AlignCenter);

    vl->addWidget(titleLbl);
    vl->addWidget(valueOut);
    return w;
}

// ── 小工具：带标题的参数行 ──────────────────────────────────────────
static QWidget *makeParamRow(const QString &title, QWidget *editor)
{
    QWidget *w = new QWidget;
    QHBoxLayout *hl = new QHBoxLayout(w);
    hl->setContentsMargins(0, 0, 0, 0);
    QLabel *lbl = new QLabel(title);
    lbl->setFixedWidth(110);
    hl->addWidget(lbl);
    hl->addWidget(editor);
    return w;
}

// ── 摄像头占位面板 ──────────────────────────────────────────────────
QWidget *MainWindow::buildCameraPanel()
{
    QWidget *panel = new QWidget;
    QVBoxLayout *vl = new QVBoxLayout(panel);
    vl->setContentsMargins(4, 4, 4, 4);
    vl->setSpacing(4);

    // 标题栏
    QLabel *title = new QLabel("摄像头实时画面");
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    QFont tf = title->font();
    tf.setPointSize(13);
    tf.setBold(true);
    title->setFont(tf);

    // 画面占位
    cameraView = new QLabel;
    cameraView->setAlignment(Qt::AlignCenter);
    cameraView->setStyleSheet(
        "background-color:#0d1120;"
        "border:1px solid #2e3a50;"
        "color:#3a4a6a;"
        "font-size:16px;");
    cameraView->setText("[ 摄像头未连接 ]");
    cameraView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 底部坐标信息栏
    QWidget *infoBar = new QWidget;
    infoBar->setFixedHeight(28);
    infoBar->setStyleSheet("background-color:#141824; border-top:1px solid #2e3a50;");
    QHBoxLayout *hl = new QHBoxLayout(infoBar);
    hl->setContentsMargins(8, 0, 8, 0);
    QLabel *pixLbl  = new QLabel("像素: --");
    QLabel *mmLbl   = new QLabel("机械坐标: --");
    QLabel *devLbl  = new QLabel("偏差: --");
    pixLbl->setObjectName("titleLabel");
    mmLbl->setObjectName("titleLabel");
    devLbl->setObjectName("titleLabel");
    hl->addWidget(pixLbl);
    hl->addStretch();
    hl->addWidget(mmLbl);
    hl->addStretch();
    hl->addWidget(devLbl);

    vl->addWidget(title);
    vl->addWidget(cameraView, 1);
    vl->addWidget(infoBar);
    return panel;
}

// ── 右上：状态数据区 ────────────────────────────────────────────────
QGroupBox *MainWindow::buildStatusGroup()
{
    QGroupBox *grp = new QGroupBox("系统状态");
    QGridLayout *gl = new QGridLayout(grp);
    gl->setSpacing(4);
    gl->setContentsMargins(6, 12, 6, 6);

    gl->addWidget(makeDataItem("X 坐标 (mm)", posXVal,  "0.000"), 0, 0);
    gl->addWidget(makeDataItem("Y 坐标 (mm)", posYVal,  "0.000"), 0, 1);
    gl->addWidget(makeDataItem("Z 坐标 (mm)", posZVal,  "0.000"), 0, 2);

    return grp;
}

// ── 右中：功能控制区 ────────────────────────────────────────────────
QGroupBox *MainWindow::buildControlGroup()
{
    QGroupBox *grp = new QGroupBox("功能控制");
    QVBoxLayout *vl = new QVBoxLayout(grp);
    vl->setSpacing(6);
    vl->setContentsMargins(6, 12, 6, 6);

    // 模式切换
    QHBoxLayout *modeRow = new QHBoxLayout;
    autoModeBtn   = new QPushButton("自动模式");
    manualModeBtn = new QPushButton("手动模式");
    autoModeBtn->setCheckable(true);
    manualModeBtn->setCheckable(true);
    autoModeBtn->setChecked(false);
    manualModeBtn->setChecked(true);
    modeRow->addWidget(autoModeBtn);
    modeRow->addWidget(manualModeBtn);
    vl->addLayout(modeRow);

    // 分隔线
    QFrame *sep1 = new QFrame; sep1->setFrameShape(QFrame::HLine); vl->addWidget(sep1);

    // 三轴点动
    QLabel *jogTitle = new QLabel("三轴点动");
    jogTitle->setObjectName("titleLabel");
    vl->addWidget(jogTitle);

    QGridLayout *jogGrid = new QGridLayout;
    jogGrid->setSpacing(4);
    jogXPlusBtn  = new QPushButton("X+");
    jogXMinusBtn = new QPushButton("X-");
    jogYPlusBtn  = new QPushButton("Y+");
    jogYMinusBtn = new QPushButton("Y-");
    jogZPlusBtn  = new QPushButton("Z+");
    jogZMinusBtn = new QPushButton("Z-");
    homeBtn      = new QPushButton("回零 $H");
    for (auto *b : {jogXPlusBtn, jogXMinusBtn, jogYPlusBtn,
                    jogYMinusBtn, jogZPlusBtn, jogZMinusBtn, homeBtn})
        b->setMinimumHeight(30);
    jogGrid->addWidget(jogXMinusBtn, 0, 0);
    jogGrid->addWidget(jogXPlusBtn,  0, 1);
    jogGrid->addWidget(jogYMinusBtn, 1, 0);
    jogGrid->addWidget(jogYPlusBtn,  1, 1);
    jogGrid->addWidget(jogZMinusBtn, 2, 0);
    jogGrid->addWidget(jogZPlusBtn,  2, 1);
    jogGrid->addWidget(homeBtn,      3, 0, 1, 2);
    vl->addLayout(jogGrid);

    QFrame *sep2 = new QFrame; sep2->setFrameShape(QFrame::HLine); vl->addWidget(sep2);

    // 夹爪控制
    QLabel *gripTitle = new QLabel("夹爪控制");
    gripTitle->setObjectName("titleLabel");
    vl->addWidget(gripTitle);
    QHBoxLayout *gripRow = new QHBoxLayout;
    gripperOpenBtn  = new QPushButton("张开");
    gripperCloseBtn = new QPushButton("闭合");
    gripperOpenBtn->setMinimumHeight(30);
    gripperCloseBtn->setMinimumHeight(30);
    gripRow->addWidget(gripperOpenBtn);
    gripRow->addWidget(gripperCloseBtn);
    vl->addLayout(gripRow);

    QFrame *sep4 = new QFrame; sep4->setFrameShape(QFrame::HLine); vl->addWidget(sep4);

    // 启动 / 停止 / 急停
    QHBoxLayout *ctrlRow = new QHBoxLayout;
    startBtn     = new QPushButton("启动");
    stopBtn      = new QPushButton("停止");
    emergencyBtn = new QPushButton("急  停");
    startBtn->setObjectName("startBtn");
    stopBtn->setObjectName("stopBtn");
    emergencyBtn->setObjectName("emergencyBtn");
    startBtn->setMinimumHeight(36);
    stopBtn->setMinimumHeight(36);
    emergencyBtn->setMinimumHeight(36);
    ctrlRow->addWidget(startBtn);
    ctrlRow->addWidget(stopBtn);
    ctrlRow->addWidget(emergencyBtn);
    vl->addLayout(ctrlRow);

    return grp;
}

// ── 右下：参数调试区 ────────────────────────────────────────────────
QGroupBox *MainWindow::buildParamGroup()
{
    QGroupBox *grp = new QGroupBox("参数调试");
    QVBoxLayout *vl = new QVBoxLayout(grp);
    vl->setSpacing(4);
    vl->setContentsMargins(6, 12, 6, 6);

    feedSpinBox = new QSpinBox;
    feedSpinBox->setRange(10, 1000); feedSpinBox->setValue(500);
    feedSpinBox->setSuffix(" mm/min");
    vl->addWidget(makeParamRow("运动速度:", feedSpinBox));

    visionThreshSpinBox = new QDoubleSpinBox;
    visionThreshSpinBox->setRange(1.0, 100.0); visionThreshSpinBox->setValue(14.0);
    visionThreshSpinBox->setSingleStep(1.0);
    vl->addWidget(makeParamRow("视觉阈值:", visionThreshSpinBox));

    jogStepSpinBox = new QDoubleSpinBox;
    jogStepSpinBox->setRange(0.1, 50.0); jogStepSpinBox->setValue(5.0);
    jogStepSpinBox->setSuffix(" mm");
    vl->addWidget(makeParamRow("点动步长:", jogStepSpinBox));

    gripForceSpinBox = new QSpinBox;
    gripForceSpinBox->setRange(1, 100); gripForceSpinBox->setValue(10);
    vl->addWidget(makeParamRow("夹爪力度:", gripForceSpinBox));

    return grp;
}

// ── 主布局 ──────────────────────────────────────────────────────────
void MainWindow::buildLayout()
{
    QWidget *central = new QWidget(this);
    QHBoxLayout *root = new QHBoxLayout(central);
    root->setContentsMargins(6, 6, 6, 6);
    root->setSpacing(6);

    // 左侧：摄像头（占 60%）
    QWidget *camPanel = buildCameraPanel();
    camPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 右侧：三个功能区放入可滚动容器
    QWidget *rightContent = new QWidget;
    QVBoxLayout *rvl = new QVBoxLayout(rightContent);
    rvl->setContentsMargins(0, 0, 4, 0);
    rvl->setSpacing(6);

    QGroupBox *statusGrp  = buildStatusGroup();
    QGroupBox *controlGrp = buildControlGroup();
    QGroupBox *paramGrp   = buildParamGroup();

    rvl->addWidget(statusGrp);
    rvl->addWidget(controlGrp);
    rvl->addWidget(paramGrp);
    rvl->addStretch();

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(rightContent);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFixedWidth(336);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(
        "QScrollArea { border: none; background: transparent; }"
        "QScrollBar:vertical { background:#1a1e2e; width:8px; border-radius:4px; }"
        "QScrollBar::handle:vertical { background:#3a4a6a; border-radius:4px; min-height:20px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }"
    );

    root->addWidget(camPanel, 1);
    root->addWidget(scrollArea, 0);

    setCentralWidget(central);
    statusBar()->showMessage("系统就绪");
}

// ── 构造 / 析构 ─────────────────────────────────────────────────────
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), gripperPosition(500)
{
    setWindowTitle("液滴无损微物体搬运系统");
    resize(1024, 600);
    buildLayout();
    initGripperSerial();

    // 夹爪长按定时器
    gripperOpenTimer  = new QTimer(this);
    gripperCloseTimer = new QTimer(this);
    gripperOpenTimer->setInterval(120);
    gripperCloseTimer->setInterval(120);
    connect(gripperOpenTimer,  &QTimer::timeout, this, &MainWindow::doGripperOpen);
    connect(gripperCloseTimer, &QTimer::timeout, this, &MainWindow::doGripperClose);

    // 夹爪按钮信号
    connect(gripperOpenBtn,  &QPushButton::pressed,  this, &MainWindow::onGripperOpenPressed);
    connect(gripperOpenBtn,  &QPushButton::released, this, &MainWindow::onGripperOpenReleased);
    connect(gripperCloseBtn, &QPushButton::pressed,  this, &MainWindow::onGripperClosePressed);
    connect(gripperCloseBtn, &QPushButton::released, this, &MainWindow::onGripperCloseReleased);
}

MainWindow::~MainWindow()
{
    if (gripperSerial && gripperSerial->isOpen())
        gripperSerial->close();
}

// ── 夹爪串口初始化 ───────────────────────────────────────────────────
void MainWindow::initGripperSerial()
{
    gripperSerial = new QSerialPort(this);
    gripperSerial->setPortName("/dev/ttyUSB0");
    gripperSerial->setBaudRate(QSerialPort::Baud115200);
    gripperSerial->setDataBits(QSerialPort::Data8);
    gripperSerial->setParity(QSerialPort::NoParity);
    gripperSerial->setStopBits(QSerialPort::OneStop);
    gripperSerial->setFlowControl(QSerialPort::NoFlowControl);

    if (!gripperSerial->open(QIODevice::ReadWrite)) {
        statusBar()->showMessage("夹爪串口打开失败: " + gripperSerial->errorString());
        gripperOpenBtn->setEnabled(false);
        gripperCloseBtn->setEnabled(false);
        return;
    }

    // 初始化使能
    sendModbus(0x0100, 1);
    QThread::msleep(2000);
    statusBar()->showMessage("夹爪已就绪  /dev/ttyUSB0");
}

// ── Modbus RTU 发送 ──────────────────────────────────────────────────
void MainWindow::sendModbus(quint16 reg, quint16 value)
{
    QByteArray frame;
    frame.append((char)0x01);
    frame.append((char)0x06);
    frame.append((char)(reg >> 8));
    frame.append((char)(reg & 0xFF));
    frame.append((char)(value >> 8));
    frame.append((char)(value & 0xFF));
    quint16 crc = crc16(frame);
    frame.append((char)(crc & 0xFF));
    frame.append((char)(crc >> 8));
    gripperSerial->write(frame);
    gripperSerial->flush();
}

quint16 MainWindow::crc16(const QByteArray &data)
{
    quint16 crc = 0xFFFF;
    for (int i = 0; i < data.size(); ++i) {
        crc ^= (quint8)data[i];
        for (int j = 0; j < 8; ++j)
            crc = (crc & 0x0001) ? (crc >> 1) ^ 0xA001 : crc >> 1;
    }
    return crc;
}

// ── 夹爪长按逻辑 ────────────────────────────────────────────────────
void MainWindow::onGripperOpenPressed()
{
    gripperCloseTimer->stop();
    doGripperOpen();
    gripperOpenTimer->start();
}

void MainWindow::onGripperOpenReleased()
{
    gripperOpenTimer->stop();
    statusBar()->showMessage(QString("夹爪停止，位置: %1").arg(gripperPosition));
}

void MainWindow::onGripperClosePressed()
{
    gripperOpenTimer->stop();
    doGripperClose();
    gripperCloseTimer->start();
}

void MainWindow::onGripperCloseReleased()
{
    gripperCloseTimer->stop();
    statusBar()->showMessage(QString("夹爪停止，位置: %1").arg(gripperPosition));
}

void MainWindow::doGripperOpen()
{
    gripperPosition = qMin(gripperPosition + 15, 1000);
    sendModbus(0x0101, (quint16)gripForceSpinBox->value());
    QThread::msleep(20);
    sendModbus(0x0104, 10);
    QThread::msleep(20);
    sendModbus(0x0103, (quint16)gripperPosition);
}

void MainWindow::doGripperClose()
{
    gripperPosition = qMax(gripperPosition - 15, 0);
    sendModbus(0x0101, (quint16)gripForceSpinBox->value());
    QThread::msleep(20);
    sendModbus(0x0104, 10);
    QThread::msleep(20);
    sendModbus(0x0103, (quint16)gripperPosition);
}
