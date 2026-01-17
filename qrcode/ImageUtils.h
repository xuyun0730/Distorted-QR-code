#pragma once
#include <vector>
#include <string>
#include <cstdint>

// 简单的灰度图像容器
struct ImageGray {
    int width = 0;
    int height = 0;
    std::vector<uint8_t> data;

    ImageGray() {}
    ImageGray(int w, int h, uint8_t color = 255);

    uint8_t getPixel(int x, int y) const;
    void setPixel(int x, int y, uint8_t val);
};

// BMP 文件保存
bool saveBMP(const std::string& filename, const ImageGray& img);

// SVG 矢量文件保存 (RLE 优化)
bool saveSVG(const std::string& filename, const ImageGray& img, int threshold = 128);