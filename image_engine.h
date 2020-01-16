#pragma once

#include "image_operations.h"

#include <set>

#include <QImage>
#include <QString>
#include <QThread>
#include <QPixmap>
#include <QMutex>
#include <QWaitCondition>


class ImageEngine : public QThread {
  Q_OBJECT

private:
    QString _dir = "";
    std::set<QString> _images;
    std::set<QString>::iterator _nxt_image = _images.begin();
    std::set<QString>::iterator _cur_image = _images.begin();

    size_t _interval_ms = 2000;
    ImageOperations::mirror_mode _mirror_mode = ImageOperations::nothing;

    bool _stop = false;
    bool _wait = false;
    bool _pause = false;
    bool _continue = true;
    QMutex _mutex;
    QWaitCondition _pauseCond;
    QWaitCondition _waitCond;

protected:
    QImage loadImage(std::set<QString>::iterator) const;
    void check_cur_dir_images();

signals:
    // some process state signals
    void si_curImageName(QString);
    // final result image to display
    void si_curImage(const QPixmap &);
    void si_paused(bool);
    void si_statusMsg(QString);

public slots:
    //! starts the thread loop
    void run() override;
    //! stops the threads run() function
    void sl_stop();

    //! just pauses the thread's function
    void sl_pause();
    //! resumes the thread's function after pause
    void sl_resume();
    //! switches between pause and resume
    void sl_toggle();
    //! changes the image mirror modes
    void sl_setMode(int);
    //! changes the interval time between the images
    void sl_setTime(size_t interv_ms);
    //! changes the search directory
    void sl_setSearchDirectory(const QString &);

public:
    //! create the thread
    ImageEngine(const QString &dir = "", const size_t interval_ms = 2000, QObject *parent = nullptr);
    //! stop the thread
    ~ImageEngine();
};
