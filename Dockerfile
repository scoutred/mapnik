FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive
ARG GIT_REF=d4c7a15bc235b986fa80255cae0df9784c8b78c6 # tag v4.1.4

# We need a newer version of cmake and the build tools than ship with
# Ubuntu. This repo has them.
RUN apt-get update && apt-get install -y gpg wget \
    && wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null \
    && echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ jammy main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null \
    && apt-get update \
    && apt-get install -y \
        git \
        cmake \
        ninja-build \
        build-essential \
        libicu-dev \
        libfreetype6-dev \
        libharfbuzz-dev \
        libxml2-dev \
        libjpeg-dev \
        libtiff-dev \
        libwebp-dev \
        libcairo2-dev \
        libproj-dev \
        libgdal-dev \
        libboost-filesystem-dev \
        libboost-program-options-dev \
        libboost-regex-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN cmake --preset linux-gcc-release -DBUILD_DEMO_VIEWER=OFF -DBUILD_TESTING=OFF -DBUILD_DEMO_CPP=OFF -DBUILD_BENCHMARK=OFF \
    && cmake --build --preset linux-gcc-release \
    && cmake --build --preset linux-gcc-release --target install \
    && cd / \
    && echo "/usr/local/lib" > /etc/ld.so.conf.d/mapnik.conf \
    && ldconfig
