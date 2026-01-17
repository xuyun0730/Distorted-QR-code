#include "ImageUtils.h"
#include <fstream>
#include <vector>

using namespace std;

// --- ImageGray 实现 ---
ImageGray::ImageGray(int w, int h, uint8_t color) : width(w), height(h) {
    data.resize(w * h, color);
}

uint8_t ImageGray::getPixel(int x, int y) const {
    if (x < 0) x = 0; if (x >= width) x = width - 1;
    if (y < 0) y = 0; if (y >= height) y = height - 1;
    return data[y * width + x];
}

void ImageGray::setPixel(int x, int y, uint8_t val) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        data[y * width + x] = val;
    }
}

// --- BMP 结构定义 (仅内部使用) ---
#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t fileType{ 0x4D42 };
    uint32_t fileSize{ 0 };
    uint16_t reserved1{ 0 };
    uint16_t reserved2{ 0 };
    uint32_t offsetData{ 0 };
};
struct BMPInfoHeader {
    uint32_t size{ 0 };
    int32_t width{ 0 };
    int32_t height{ 0 };
    uint16_t planes{ 1 };
    uint16_t bitCount{ 0 };
    uint32_t compression{ 0 };
    uint32_t sizeImage{ 0 };
    int32_t xPixelsPerMeter{ 0 };
    int32_t yPixelsPerMeter{ 0 };
    uint32_t colorsUsed{ 0 };
    uint32_t colorsImportant{ 0 };
};
#pragma pack(pop)

bool saveBMP(const string& filename, const ImageGray& img) {
    ofstream file(filename, ios::binary);
    if (!file) return false;

    int rowStride = (img.width * 3 + 3) & (~3);
    int dataSize = rowStride * img.height;

    BMPFileHeader fHeader;
    fHeader.fileSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + dataSize;
    fHeader.offsetData = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

    BMPInfoHeader iHeader;
    iHeader.size = sizeof(BMPInfoHeader);
    iHeader.width = img.width;
    iHeader.height = -img.height;
    iHeader.planes = 1;
    iHeader.bitCount = 24;
    iHeader.sizeImage = dataSize;

    file.write((char*)&fHeader, sizeof(fHeader));
    file.write((char*)&iHeader, sizeof(iHeader));

    std::vector<uint8_t> rowBuffer(rowStride, 0);
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            uint8_t val = img.getPixel(x, y);
            rowBuffer[x * 3 + 0] = val;
            rowBuffer[x * 3 + 1] = val;
            rowBuffer[x * 3 + 2] = val;
        }
        file.write((char*)rowBuffer.data(), rowStride);
    }
    return true;
}

bool saveSVG(const string& filename, const ImageGray& img, int threshold) {
    ofstream file(filename);
    if (!file) return false;

    file << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    file << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" "
         << "width=\"" << img.width << "\" height=\"" << img.height << "\" "
         << "viewBox=\"0 0 " << img.width << " " << img.height << "\">\n";

    for (int y = 0; y < img.height; ++y) {
        int startX = -1;
        for (int x = 0; x < img.width; ++x) {
            bool isBlack = (img.getPixel(x, y) < threshold);
            if (isBlack) {
                if (startX == -1) startX = x;
            } else {
                if (startX != -1) {
                    file << "<rect x=\"" << startX << "\" y=\"" << y 
                         << "\" width=\"" << (x - startX) << "\" height=\"1\" "
                         << "fill=\"black\" shape-rendering=\"crispEdges\" />\n";
                    startX = -1;
                }
            }
        }
        if (startX != -1) {
            file << "<rect x=\"" << startX << "\" y=\"" << y 
                 << "\" width=\"" << (img.width - startX) << "\" height=\"1\" "
                 << "fill=\"black\" shape-rendering=\"crispEdges\" />\n";
        }
    }
    file << "</svg>\n";
    return true;
}