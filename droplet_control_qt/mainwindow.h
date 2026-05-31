#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    QPushButton *dropletReleaseBtn, *dropletAbsorbBtn;
    QPushButton *homeBtn, *pickPlaceBtn;

    // 右下：参数调试区
    QSpinBox    *feedSpinBox;
    QSpinBox    *gripForceSpinBox;
    QDoubleSpinBox *visionThreshSpinBox;
    QDoubleSpinBox *jogStepSpinBox;

    void buildLayout();
    QGroupBox *buildStatusGroup();
    QGroupBox *buildControlGroup();
    QGroupBox *buildParamGroup();
    QWidget   *buildCameraPanel();
};

#endif // MAINWINDOW_H
