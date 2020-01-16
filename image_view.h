#pragma once

#include <QMainWindow>
#include <QString>
#include <QPixmap>
#include <memory>


class ImageView : public QMainWindow
{
    Q_OBJECT

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
    ~ImageView() override;

private:
    class Impl;
    std::unique_ptr<Impl> d;
};
