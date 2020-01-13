#pragma once

#include <QImage>
#include <map>

class ImageOperations {
public:
    // image manipulation modes
    enum mirror_mode {
        nothing = 0,
        vertical,
        horizontal
        // ...
    };

    //! create an assignment table for widgets coupled to image manipulation modes
    static std::map<mirror_mode, QString> getMirrorModes();
    //! apply the manipulation to an image
    static QImage mirrorImg(const QImage &image, const mirror_mode mode);
    // some other potential more fancy image modifications ..
};
