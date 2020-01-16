#pragma once

#include <QMainWindow>

class QDockWidget;
class QPushButton;
class QComboBox;
class QScrollArea;
class QLabel;
class QSpinBox;
class ImageEngine;


class ImageView : public QMainWindow
{
    Q_OBJECT

// when the list becomes too long one should create a pimpl ..
private /*variables*/:
    /*mainview*/
    QLabel *_imageLabel = nullptr;
    QScrollArea *_scrollArea = nullptr;

    size_t _interval_ms = 2000;
    QString _curDir = "";

    /*settings*/
    QDockWidget *_dockSettings = nullptr;
    QPushButton *_butSearchDir = nullptr;
    QPushButton *_butStartStop = nullptr;
    QComboBox *_imageManipulation = nullptr;
    QSpinBox *_timeInterval;

    ImageEngine *_imgEngine = nullptr;

private /*functions*/:
    void prepareImageView();
    void prepareDockSettings();
    void connectDockSettings();

signals:
    void si_imageSearchDirChanged(QString);

private slots:
    //! some feedback for the user printed into status bar
    void sl_printStatusMsg(QString);
    //! create and connect the slide show thread
    void sl_createImgEngine();
    //! renames the start button for pause/resume purposes
    void sl_renameStartStopButton(bool engine_paused);
    //! renders the image from the thread into a label
    void sl_show(const QPixmap &);
    //! change search path for slide show
    void sl_selSearchDir();

protected:
    void keyPressEvent( QKeyEvent * event ) override;

public:
    ImageView(QWidget *parent = nullptr);
    ~ImageView();
};
