#include "camerathread.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <opencv2/opencv.hpp>

#define DEVICE  "/dev/video0"
#define BUFS    4

struct MmapBuf { void *start; size_t len; };

CameraThread::CameraThread(int, QObject *parent)
    : QThread(parent), m_running(false) {}

void CameraThread::stop() { m_running = false; wait(); }

void CameraThread::run()
{
    // 阻塞模式打开，与 cam_fb 一致
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) return;

    // 设置 MJPEG 640x480
    struct v4l2_format fmt = {};
    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = 640;
    fmt.fmt.pix.height      = 480;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    fmt.fmt.pix.field       = V4L2_FIELD_ANY;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) { close(fd); return; }

    // mmap 缓冲区
    struct v4l2_requestbuffers req = {};
    req.count  = BUFS;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) { close(fd); return; }

    MmapBuf bufs[BUFS];
    for (unsigned i = 0; i < req.count; i++) {
        struct v4l2_buffer b = {};
        b.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        b.memory = V4L2_MEMORY_MMAP; b.index = i;
        ioctl(fd, VIDIOC_QUERYBUF, &b);
        bufs[i].start = mmap(NULL, b.length, PROT_READ|PROT_WRITE,
                             MAP_SHARED, fd, b.m.offset);
        bufs[i].len = b.length;
        ioctl(fd, VIDIOC_QBUF, &b);
    }

    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) { close(fd); return; }

    m_running = true;

    while (m_running) {
        struct v4l2_buffer b = {};
        b.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        b.memory = V4L2_MEMORY_MMAP;
        if (ioctl(fd, VIDIOC_DQBUF, &b) < 0) break;

        if (b.bytesused > 0) {
            std::vector<uchar> data(
                (uchar*)bufs[b.index].start,
                (uchar*)bufs[b.index].start + b.bytesused);
            cv::Mat frame = cv::imdecode(data, cv::IMREAD_COLOR);
            if (!frame.empty()) {
                cv::Mat rgb;
                cv::cvtColor(frame, rgb, cv::COLOR_BGR2RGB);
                QImage img(rgb.data, rgb.cols, rgb.rows,
                           (int)rgb.step, QImage::Format_RGB888);
                emit frameReady(img.copy());
            }
        }

        ioctl(fd, VIDIOC_QBUF, &b);
    }

    ioctl(fd, VIDIOC_STREAMOFF, &type);
    for (unsigned i = 0; i < req.count; i++)
        munmap(bufs[i].start, bufs[i].len);
    close(fd);
}
