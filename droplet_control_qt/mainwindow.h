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
class QComboBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onGripperOpenPressed();
    void onGripperOpenReleased();
    void onGripperClosePressed();
    void onGripperCloseReleased();
    void doGripperOpen();
    void doGripperClose();

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

    // 夹爪串口
    QSerialPort *gripperSerial;
    QTimer      *gripperOpenTimer;
    QTimer      *gripperCloseTimer;
    int          gripperPosition;

    void initGripperSerial();
    void sendModbus(quint16 reg, quint16 value);
    quint16 crc16(const QByteArray &data);

    void buildLayout();
    QGroupBox *buildStatusGroup();
    QGroupBox *buildControlGroup();
    QGroupBox *buildParamGroup();
    QWidget   *buildCameraPanel();
};

#endif // MAINWINDOW_H
