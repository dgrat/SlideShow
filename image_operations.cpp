#include "image_operations.h"


QImage ImageOperations::mirrorImg(const QImage &image, const ImageOperations::mirror_mode mode) {
    switch (mode) {
    case ImageOperations::vertical:
        return image.mirrored(false, true);
    case ImageOperations::horizontal:
        return image.mirrored(true, false);
    default:
        // some error handling
        return image;
    }
}

std::map<ImageOperations::mirror_mode, QString> ImageOperations::getMirrorModes() {
    return {
        std::make_pair<ImageOperations::mirror_mode, QString>(ImageOperations::nothing, "None"),
        std::make_pair<ImageOperations::mirror_mode, QString>(ImageOperations::vertical, "Vertical"),
        std::make_pair<ImageOperations::mirror_mode, QString>(ImageOperations::horizontal, "Horizontal"),
        // ..
    };
}
