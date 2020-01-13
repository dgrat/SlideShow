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


ImageView::ImageView(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle(getApp()->appName());
    this->setStatusBar(new QStatusBar(this));
    this->resize(800, 600);

    prepareImageView();
    prepareDockSettings();

    connectDockSettings();
}

void ImageView::prepareImageView() {
    _imageLabel = new QLabel(this);
    _imageLabel->setBackgroundRole(QPalette::Base);
    _imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    _imageLabel->setScaledContents(true);

    _scrollArea = new QScrollArea(this);
    _scrollArea->setBackgroundRole(QPalette::Dark);
    _scrollArea->setWidget(_imageLabel);
    _scrollArea->setWidgetResizable(true);
    setCentralWidget(_scrollArea);

    this->statusBar()->showMessage("Choose directory", -1);
}

void ImageView::sl_createImgEngine() {
    if(_curDir.isEmpty()) {
        this->statusBar()->showMessage("Cannot start with undefined directory");
        return;
    }

    // create thread once and then just pause/continue
    if(_imgEngine == nullptr) {
        _imgEngine = new ImageEngine(_curDir, _interval_ms, this);

        disconnect(_butStartStop, &QPushButton::pressed, this, &ImageView::sl_createImgEngine);

        connect(_butStartStop, &QPushButton::pressed, _imgEngine, &ImageEngine::sl_toggle);
        connect(_imageManipulation, QOverload<int>::of(&QComboBox::activated), _imgEngine, &ImageEngine::sl_setMode);
        connect(_timeInterval, QOverload<int>::of(&QSpinBox::valueChanged), _imgEngine, &ImageEngine::sl_setTime);

        connect(_imgEngine, &ImageEngine::finished, this, &ImageView::deleteLater);
        connect(_imgEngine, &ImageEngine::si_statusMsg, this, &ImageView::sl_printStatusMsg);
        connect(_imgEngine, &ImageEngine::si_paused, this, &ImageView::sl_renameStartStopButton);
        connect(_imgEngine, &ImageEngine::si_curImage, this, &ImageView::sl_show);
        connect(_imgEngine, &ImageEngine::si_curImageName, this->statusBar(), [=](QString name){
            this->statusBar()->showMessage("Current Image: " + name);
        });

        _imageManipulation->setEnabled(true);
        _timeInterval->setEnabled(true);

        _imgEngine->start();
    }
}

void ImageView::sl_printStatusMsg(QString msg) {
    this->statusBar()->showMessage(msg);
}

void ImageView::sl_renameStartStopButton(bool engine_paused) {
    if(engine_paused) {
        _butStartStop->setText("Continue");
    }
    else {
        _butStartStop->setText("Pause");
    }
}

ImageView::~ImageView() {
}

void ImageView::sl_show(const QPixmap &img) {
    _imageLabel->setPixmap(img);
}

void ImageView::sl_selSearchDir() {
    const QString homeDir = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);
    _curDir = QFileDialog::getExistingDirectory(this, "Select Search Directory", homeDir, QFileDialog::DontResolveSymlinks); // QT bug ShowDirsOnly always on behavior

    if(_imgEngine) {
        _imgEngine->sl_setSearchDirectory(_curDir);
    }
}

void ImageView::connectDockSettings() {
    connect(_butStartStop, &QPushButton::pressed, this, &ImageView::sl_createImgEngine);
    connect(_butSearchDir, &QPushButton::pressed, this, &ImageView::sl_selSearchDir);
}

void ImageView::prepareDockSettings() {
    _dockSettings = new QDockWidget("Settings", this);
    _dockSettings->setFeatures(QDockWidget::DockWidgetMovable);

    QWidget *settings_widget = new QWidget(this);

    _butSearchDir = new QPushButton("Define Image Folder", this);
    _butStartStop = new QPushButton("Start", this);
    _imageManipulation = new QComboBox(this);
    _imageManipulation->setEnabled(false);
    _timeInterval = new QSpinBox(this);
    _timeInterval->setRange(100, 10000);
    _timeInterval->setValue(2000);
    _timeInterval->setEnabled(false);
    // add labels
    for(auto e : ImageOperations::getMirrorModes()) {
        _imageManipulation->insertItem(e.first, e.second);
    }

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(_butSearchDir);
    layout->addWidget(_butStartStop);
    layout->addWidget(new QLabel("Mirror Mode", this));
    layout->addWidget(_imageManipulation);
    layout->addWidget(new QLabel("Interval [ms]", this));
    layout->addWidget(_timeInterval);
    layout->addStretch();

    settings_widget->setLayout(layout);
    _dockSettings->setWidget(settings_widget);
    this->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, _dockSettings);
}
