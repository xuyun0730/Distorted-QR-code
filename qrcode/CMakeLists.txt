cmake_minimum_required(VERSION 3.10)

project(QRCodeGen)

add_executable(QRCodeGen main.cpp qrcodegen.cpp Distortion.cpp ImageUtils.cpp)