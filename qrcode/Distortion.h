#pragma once
#include "ImageUtils.h"
#include "qrcodegen.hpp"

// 将 QR 对象渲染为图片
ImageGray renderQrToImage(const qrcodegen::QrCode& qr, int scale, int border);

// 生成复合畸变图像 (核心函数)
// 参数: 原图, 半径(px), 倾斜角(deg), 视距(px)
ImageGray generateCompositeDistortion(const ImageGray& src, double radiusPx, double tiltDeg, double distPx);