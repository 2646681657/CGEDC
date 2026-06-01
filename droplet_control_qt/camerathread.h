#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>
#include <QImage>

class CameraThread : public QThread
{
    Q_OBJECT

public:
    explicit CameraThread(int deviceIndex = 0, QObject *parent = nullptr);
    void stop();

signals:
    void frameReady(const QImage &img);

protected:
    void run() override;

private:
    int  m_deviceIndex;
    bool m_running;
};

#endif // CAMERATHREAD_H
