#include "image_view.h"
#include "image_engine.h"
#include "image_operations.h"
#include "app.h"

#include <QDockWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QStandardPaths>
#include <QFileDialog>
#include <QStatusBar>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPixmap>


class ImageView::Impl {
public:
    /*mainview*/
    QLabel *_imageLabel = nullptr;
    QScrollArea *_scrollArea = nullptr;

    /*docker*/
    QDockWidget *_dockSettings = nullptr;
    QPushButton *_butSearchDir = nullptr;
    QPushButton *_butStartStop = nullptr;
    QComboBox *_imageManipulation = nullptr;
    QSpinBox *_timeInterval;

    /*thread*/
    ImageEngine *_imgEngine = nullptr;

    /*settings*/
    size_t _interval_ms = 2000;
    QString _curDir = "";
};


ImageView::ImageView(QWidget *parent)
    : QMainWindow(parent)
    , d(new Impl)
{
    this->setWindowTitle(getApp()->appName());
    this->setStatusBar(new QStatusBar(this));
    this->resize(800, 600);

    prepareImageView();
    prepareDockSettings();
    connectDockSettings();
}

void ImageView::prepareImageView() {
    d->_imageLabel = new QLabel(this);
    d->_imageLabel->setBackgroundRole(QPalette::Base);
    d->_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    d->_imageLabel->setScaledContents(true);

    d->_scrollArea = new QScrollArea(this);
    d->_scrollArea->setBackgroundRole(QPalette::Dark);
    d->_scrollArea->setWidget(d->_imageLabel);
    d->_scrollArea->setWidgetResizable(true);
    setCentralWidget(d->_scrollArea);

    this->statusBar()->showMessage("Choose directory", -1);
}

void ImageView::sl_createImgEngine() {
    if(d->_curDir.isEmpty()) {
        this->statusBar()->showMessage("Cannot start with undefined directory");
        return;
    }

    // create thread once and then just pause/continue
    if(d->_imgEngine == nullptr) {
        d->_imgEngine = new ImageEngine(d->_curDir, d->_interval_ms, this);

        disconnect(d->_butStartStop, &QPushButton::pressed, this, &ImageView::sl_createImgEngine);

        connect(d->_butStartStop, &QPushButton::pressed, d->_imgEngine, &ImageEngine::sl_toggle);
        connect(d->_imageManipulation, QOverload<int>::of(&QComboBox::activated), d->_imgEngine, &ImageEngine::sl_setMode);
        connect(d->_timeInterval, QOverload<int>::of(&QSpinBox::valueChanged), d->_imgEngine, &ImageEngine::sl_setTime);

        connect(d->_imgEngine, &ImageEngine::finished, this, &ImageView::deleteLater);
        connect(d->_imgEngine, &ImageEngine::si_statusMsg, this, &ImageView::sl_printStatusMsg);
        connect(d->_imgEngine, &ImageEngine::si_paused, this, &ImageView::sl_renameStartStopButton);
        connect(d->_imgEngine, &ImageEngine::si_curImage, this, &ImageView::sl_show);
        connect(d->_imgEngine, &ImageEngine::si_curImageName, this->statusBar(), [=](QString name){
            this->statusBar()->showMessage("Current Image: " + name);
        });

        d->_imageManipulation->setEnabled(true);
        d->_timeInterval->setEnabled(true);
        d->_imgEngine->start();
    }
}

void ImageView::sl_printStatusMsg(QString msg) {
    this->statusBar()->showMessage(msg);
}

void ImageView::sl_renameStartStopButton(bool engine_paused) {
    if(engine_paused) {
        d->_butStartStop->setText("Continue");
    }
    else {
        d->_butStartStop->setText("Pause");
    }
}

ImageView::~ImageView() {
}

void ImageView::sl_show(const QPixmap &img) {
    d->_imageLabel->setPixmap(img);
}

void ImageView::sl_selSearchDir() {
    const QString homeDir = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);
    d->_curDir = QFileDialog::getExistingDirectory(this, "Select Search Directory", homeDir, QFileDialog::DontResolveSymlinks); // QT bug ShowDirsOnly always on behavior

    if(d->_imgEngine) {
        d->_imgEngine->sl_setSearchDirectory(d->_curDir);
    }
}

void ImageView::connectDockSettings() {
    connect(d->_butStartStop, &QPushButton::pressed, this, &ImageView::sl_createImgEngine);
    connect(d->_butSearchDir, &QPushButton::pressed, this, &ImageView::sl_selSearchDir);
}

void ImageView::keyPressEvent( QKeyEvent * event ) {
    if( event->key() == Qt::Key_F1 ) {
        QMessageBox::about(this, "Contact", getApp()->author());
    }
}

void ImageView::prepareDockSettings() {
    d->_dockSettings = new QDockWidget("Settings", this);
    d->_dockSettings->setFeatures(QDockWidget::DockWidgetMovable);

    QWidget *settings_widget = new QWidget(this);

    d->_butSearchDir = new QPushButton("Define Image Folder", this);
    d->_butStartStop = new QPushButton("Start", this);
    d->_imageManipulation = new QComboBox(this);
    d->_imageManipulation->setEnabled(false);
    d->_timeInterval = new QSpinBox(this);
    d->_timeInterval->setRange(100, 10000);
    d->_timeInterval->setValue(2000);
    d->_timeInterval->setEnabled(false);
    // add labels
    for(auto e : ImageOperations::getMirrorModes()) {
        d->_imageManipulation->insertItem(e.first, e.second);
    }

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(d->_butSearchDir);
    layout->addWidget(d->_butStartStop);
    layout->addWidget(new QLabel("Mirror Mode", this));
    layout->addWidget(d->_imageManipulation);
    layout->addWidget(new QLabel("Interval [ms]", this));
    layout->addWidget(d->_timeInterval);
    layout->addStretch();

    settings_widget->setLayout(layout);
    d->_dockSettings->setWidget(settings_widget);
    this->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, d->_dockSettings);
}
