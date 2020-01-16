#pragma once

#include <memory>
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
    const QString author() const { return "Author: Daniel Frenzel\nMail: dan.frenzel87@gmail.com"; }

    // Examples ..
    // getLog()
    // getDocu()
    // getSharedMenu()
    // ...

private:
    class Impl;
    std::unique_ptr<Impl> d;
};
