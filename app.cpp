#include "app.h"
#include "image_view.h"


namespace {
    App *s_app = nullptr;
}

class App::Impl {
public:
    std::unique_ptr<ImageView> mainWindow;
};

App *getApp() {
    return s_app;
}

App::App() : d(new Impl)
{
    // first, set s_app singleton accessor
    s_app = this;

    // init in order
    d->mainWindow.reset(new ImageView);
}

// accessor to main window
ImageView *App::getGUI() const {
    return d->mainWindow.get();
}

App::~App() {}
