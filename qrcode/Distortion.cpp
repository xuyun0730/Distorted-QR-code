#include "Distortion.h"
#include <cmath>
#include <algorithm>

using namespace std;

// 私有辅助函数：双线性插值
static uint8_t interpolatePixel(const ImageGray& src, double srcX, double srcY) {
    int x1 = (int)floor(srcX);
    int y1 = (int)floor(srcY);
    int x2 = x1 + 1;
    int y2 = y1 + 1;

    if (x1 < 0) x1 = 0; if (x2 >= src.width) x2 = src.width - 1;
    if (y1 < 0) y1 = 0; if (y2 >= src.height) y2 = src.height - 1;

    uint8_t v11 = src.getPixel(x1, y1);
    uint8_t v21 = src.getPixel(x2, y1);
    uint8_t v12 = src.getPixel(x1, y2);
    uint8_t v22 = src.getPixel(x2, y2);

    double wx = srcX - floor(srcX);
    double wy = srcY - floor(srcY);

    double valTop = v11 * (1.0 - wx) + v21 * wx;
    double valBot = v12 * (1.0 - wx) + v22 * wx;
    return (uint8_t)(valTop * (1.0 - wy) + valBot * wy);
}

ImageGray renderQrToImage(const qrcodegen::QrCode& qr, int scale, int border) {
    int qrSize = qr.getSize();
    int imgSize = (qrSize + border * 2) * scale;
    ImageGray img(imgSize, imgSize, 255);

    for (int y = 0; y < qrSize; y++) {
        for (int x = 0; x < qrSize; x++) {
            if (qr.getModule(x, y)) {
                int startX = (x + border) * scale;
                int startY = (y + border) * scale;
                for (int py = 0; py < scale; py++) {
                    for (int px = 0; px < scale; px++) {
                        img.setPixel(startX + px, startY + py, 0);
                    }
                }
            }
        }
    }
    return img;
}

ImageGray generateCompositeDistortion(const ImageGray& src, double radiusPx, double tiltDeg, double distPx) {
    double tiltRad = tiltDeg * 3.14159265358979 / 180.0;
    
    // 自动画布扩容
    double expansionFactor = 1.0 + std::abs(tan(tiltRad)) * 0.8;
    double scaleTotal = expansionFactor * 1.2;

    double halfSrcW = src.width / 2.0;
    double ratio = halfSrcW / radiusPx;
    if (ratio > 1.0) ratio = 1.0; 
    
    double maxAngle = asin(ratio);
    int baseArcWidth = (int)ceil(2.0 * radiusPx * maxAngle);
    
    int dstWidth = (int)(baseArcWidth * scaleTotal);
    int dstHeight = (int)(src.height * scaleTotal);
    if (dstWidth % 2 != 0) dstWidth++;
    if (dstHeight % 2 != 0) dstHeight++;

    ImageGray dst(dstWidth, dstHeight, 255);

    double dstCenterX = dstWidth / 2.0;
    double dstCenterY = dstHeight / 2.0;
    double srcCenterX = src.width / 2.0;
    double srcCenterY = src.height / 2.0;

    double cosT = cos(tiltRad);
    double sinT = sin(tiltRad);

    for (int y = 0; y < dstHeight; ++y) {
        for (int x = 0; x < dstWidth; ++x) {
            double s = x - dstCenterX; 
            double theta = s / radiusPx;
            if (std::abs(theta) >= 1.5707) continue; 

            double Px = radiusPx * sin(theta);
            double Py = y - dstCenterY; 
            double Pz = radiusPx * (1.0 - cos(theta)); 

            double RotX = Px;
            double RotY = Py * cosT - Pz * sinT;
            double RotZ = Py * sinT + Pz * cosT;

            double perspectiveFactor = 1.0 + (RotZ / distPx);
            if (perspectiveFactor <= 0.001) continue;

            double visualX = RotX / perspectiveFactor;
            double visualY = RotY / perspectiveFactor;

            double srcX = srcCenterX + visualX;
            double srcY = srcCenterY + visualY;

            if (srcX >= 0 && srcX < src.width - 1 && srcY >= 0 && srcY < src.height - 1) {
                dst.setPixel(x, y, interpolatePixel(src, srcX, srcY));
            }
        }
    }
    return dst;
}