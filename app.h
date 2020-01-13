#pragma once

#include <QString>

class ImageView;

class App;
App *getApp();

class App {
public:
    App();
    ~App();

    // delete copy ctors
    App(App const&) = delete;
    App& operator=(App const&) = delete;

    //! accessor to main window
    ImageView *getGUI() const;

    //! the application name
    const QString appName() const { return "Slide Show"; }

    // Examples ..
    // getLog()
    // getDocu()
    // getAuthor()
    // getSharedMenu()
    // ...

private:
    class Impl;
    Impl * const d;
};
