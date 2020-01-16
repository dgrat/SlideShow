#include "image_engine.h"

#include <chrono>

#include <QImageReader>
#include <QFileInfo>
#include <QDirIterator>
#include <QDebug>
#include <QtSvg/QSvgRenderer>
#include <QPainter>
#include <QTimer>


namespace {
    QPixmap svgToPixmap(const QSize &ImageSize, const QString &SvgFile) {
        QSvgRenderer SvgRenderer(SvgFile, nullptr);
        QPixmap Image(ImageSize);

        Image.fill(Qt::transparent);

        QPainter p;
        p.begin(&Image);
        SvgRenderer.render(&p);
        p.end();

        return Image;
    }
}

// maybe user deleted or added some stuff while running
void ImageEngine::check_cur_dir_images() {
    bool something_changed = false;

    QDirIterator it(_dir);
    while (it.hasNext()) {
        const QString cur = it.next();
        bool is_image = false;

        // check for supported image formats
        for(auto e : QImageReader::supportedImageFormats()) {
            if(!cur.endsWith(e, Qt::CaseInsensitive)) continue;
            if(!QFileInfo(cur).isFile()) continue;
            is_image = true;
            break;
        }
        if(!is_image) continue;

        // there is a new image in the folder :(
        if(_images.find(cur) == _images.end()) {
            _images.insert(cur);
            something_changed = true;
        }
    }

    // maybe an image has been removed
    // then we need to remove it from the list too :D
    for(auto file : _images) {
        if(QFileInfo::exists(file)) continue;
        _images.erase(file);
        something_changed = true;
    }

    // we better reset the progress iterator at this point
    if(something_changed)
        _cur_image = _images.begin();
}

void ImageEngine::run() {
    emit si_paused(false);
    forever {
        // pause lock
        {
            QMutexLocker locker(&_mutex);
            if(_pause) {
                _pauseCond.wait(&_mutex);
            }
        }

        // break condition
        {
            QMutexLocker locker(&_mutex);
            if(_stop) {
                qDebug() << "End image engine thread";
                return;
            }
        }

        // start cycle timing
        const auto start_ms = std::chrono::steady_clock::now();

        // despite it is a bit slower, I decided to check for changes in the directory in every cycle
        check_cur_dir_images();
        if(_images.size() == 0) {
            emit si_statusMsg("Folder Empty. Choose another..");
            emit si_curImage(svgToPixmap(QSize(1200,1200), ":/sad.svg"));
            continue;
        }

        // we cycle over the images in a loop
        if(_nxt_image == _images.end()) _nxt_image = _images.begin();

        // do the actual image manipulation
        QImage img = ImageOperations::mirrorImg(QImage(*_nxt_image), _mirror_mode);
        emit si_curImageName(*_nxt_image);
        emit si_curImage(QPixmap::fromImage(img));

        _cur_image = _nxt_image;
        _nxt_image++;

        // cycle lock
        {
            QMutexLocker locker(&_mutex);

            // end cycle timing
            const auto stop_ms = std::chrono::steady_clock::now();
            const auto elaps_ms = std::chrono::duration_cast<std::chrono::milliseconds>(stop_ms-start_ms).count();

            const int res_ms = ((int)_interval_ms - (int)elaps_ms) > 0 ?((int)_interval_ms - (int)elaps_ms) : 0;
            if(res_ms == 0) {
                emit si_statusMsg("Warning: cycle interval too low");
            }

            // replaces "msleep(_interval)" without blocking event handling
            QTimer::singleShot(res_ms, this, [=](){
                _waitCond.wakeAll();
            });
            _waitCond.wait(&_mutex); // released in slot: "sl_newCycle", called by _timer
        }
    }
}

void ImageEngine::sl_setTime(size_t interv_ms) {
    {
        QMutexLocker locker(&_mutex);
        _interval_ms = interv_ms;
    }

    emit si_statusMsg("Changed interval");
}

void ImageEngine::sl_setSearchDirectory(const QString &dir) {
    QMutexLocker locker(&_mutex);
    _dir = dir;
    // clear current image list
    _images.clear();
    _nxt_image = _images.begin();
    _cur_image = _images.begin();
    emit si_statusMsg("Defined image directory: " + _dir);
}

void ImageEngine::sl_stop() {
    {
        QMutexLocker locker(&_mutex);
        _waitCond.wakeAll();
    }

    {
        QMutexLocker locker(&_mutex);
        _stop = true;
    }

    {
        QMutexLocker locker(&_mutex);
        _pause = false;
        _pauseCond.wakeAll();
    }
}

void ImageEngine::sl_pause() {
    QMutexLocker locker(&_mutex);
    _pause = true;
    emit si_paused(true);
    emit si_statusMsg("Thread Paused");
}

void ImageEngine::sl_resume() {
    QMutexLocker locker(&_mutex);
    _pause = false;
    _pauseCond.wakeAll();
    emit si_paused(false);
    emit si_statusMsg("Thread Resumed");
}

void ImageEngine::sl_toggle() {
    if(_pause) {
        sl_resume();
    }
    else {
        sl_pause();
    }
}

// mode change with direct effect
void ImageEngine::sl_setMode(int mode) {
    QMutexLocker locker(&_mutex);

    // always change mode
    _mirror_mode = static_cast<ImageOperations::mirror_mode>(mode);

    // do the actual image manipulation
    if(_images.size() == 0) return;
    QImage img = ImageOperations::mirrorImg(QImage(*_cur_image), _mirror_mode);
    emit si_curImageName(*_cur_image);
    emit si_curImage(QPixmap::fromImage(img));
    emit si_statusMsg("Changed Mode: " + ImageOperations::getMirrorModes()[_mirror_mode]);
}

ImageEngine::ImageEngine(const QString &dir, const size_t interval_ms, QObject *parent)
    : QThread(parent),
      _dir(dir),
      _interval_ms(interval_ms)
{
}

ImageEngine::~ImageEngine() {
    sl_stop();
    wait();
}
