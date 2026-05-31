#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>

class QLabel;
class QPushButton;
class QGroupBox;
class QSpinBox;
class QDoubleSpinBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 夹爪
    void onGripperOpenPressed();
    void onGripperOpenReleased();
    void onGripperClosePressed();
    void onGripperCloseReleased();
    void doGripperOpen();
    void doGripperClose();

    // 三轴点动
    void onJogXPlus();
    void onJogXMinus();
    void onJogYPlus();
    void onJogYMinus();
    void onJogZPlus();
    void onJogZMinus();
    void onHome();

    // 三轴串口响应
    void onSliderReadyRead();

private:
    // 左侧：摄像头画面区
    QLabel *cameraView;

    // 右上：状态数据区
    QLabel *posXVal, *posYVal, *posZVal;

    // 右中：功能控制区
    QPushButton *startBtn, *stopBtn, *emergencyBtn;
    QPushButton *autoModeBtn, *manualModeBtn;
    QPushButton *jogXPlusBtn, *jogXMinusBtn;
    QPushButton *jogYPlusBtn, *jogYMinusBtn;
    QPushButton *jogZPlusBtn, *jogZMinusBtn;
    QPushButton *gripperOpenBtn, *gripperCloseBtn;
    QPushButton *homeBtn;

    // 右下：参数调试区
    QSpinBox       *feedSpinBox;
    QSpinBox       *gripForceSpinBox;
    QDoubleSpinBox *visionThreshSpinBox;
    QDoubleSpinBox *jogStepSpinBox;
    QDoubleSpinBox *gripperStepSpinBox;

    // 夹爪串口
    QSerialPort *gripperSerial;
    QTimer      *gripperOpenTimer;
    QTimer      *gripperCloseTimer;
    int          gripperPosition;

    void initGripperSerial();
    void sendModbus(quint16 reg, quint16 value);
    quint16 crc16(const QByteArray &data);

    // 三轴串口
    QSerialPort *sliderSerial;
    QString      sliderRxBuf;
    bool         sliderBusy;

    void initSliderSerial();
    void sliderSend(const QString &cmd);
    void updatePosDisplay(const QString &resp);

    void buildLayout();
    QGroupBox *buildStatusGroup();
    QGroupBox *buildControlGroup();
    QGroupBox *buildParamGroup();
    QWidget   *buildCameraPanel();
};

#endif // MAINWINDOW_H
