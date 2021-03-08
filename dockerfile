FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive
RUN apt update && apt install -y python3 python3-pip cmake git build-essential libgl-dev pkg-config xorg-dev xkb-data libglu1-mesa-dev libgbm-dev libx11-xcb-dev libxcb-dri3-dev libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev libxcb-randr0-dev libxcb-render-util0-dev libxcb-render0-dev libxcb-shape0-dev libxcb-sync-dev libxcb-util-dev libxcb-xfixes0-dev libxcb-xinerama0-dev libxcb-xkb-dev libjack-dev libaudio-dev
RUN pip3 install conan

COPY . /imgui-config-window

ENV CC=/usr/bin/gcc
ENV CXX=/usr/bin/g++

RUN conan remote add bincrafters  https://api.bintray.com/conan/bincrafters/public-conan
RUN cd /imgui-config-window && \
    git submodule update --init --recursive && \
    rm -rf build && \
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 && \
    cmake --build build --parallel 12

WORKDIR /imgui-config-window/build