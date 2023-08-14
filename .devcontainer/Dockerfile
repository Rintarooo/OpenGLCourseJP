# FROM nvidia/cudagl:10.1-devel-ubuntu18.04
FROM ubuntu:18.04

# set environment variable
ENV WORK_DIR /app
ARG DEBIAN_FRONTEND noninteractive

ARG TZ=Asia/Tokyo
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# WORKDIR /opt
# Create a working directory
RUN mkdir ${WORK_DIR}
WORKDIR ${WORK_DIR}

# # GPG error: https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64  InRelease: The following signatures couldn't be verified because the public key is not available: NO_PUBKEY A4B469963BF863CC
# # #0 30.83 E: The repository 'https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64  InRelease' is not signed.
# RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-keys A4B469963BF863CC

# Build tools + debugger
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    ca-certificates \
    sudo \
    tzdata \
    git \
    cmake \
    wget \
    bzip2 \
    build-essential \
    curl \
    vim \
    gdb \
    gnuplot \
    libx11-6 \
    && apt clean -y \
    && rm -rf /var/lib/apt/lists/*
	


# Media I/O:
# RUN sudo apt install -y \
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    zlib1g-dev \
    libjpeg-dev \
    libwebp-dev \
    libpng-dev \
    libtiff5-dev \
    libopenexr-dev \
    libgdal-dev \
    libgtk2.0-dev

# for OpenGL
# https://grainrigi.hatenablog.com/entry/2017/08/27/144218
# GLFW, GLEW, GLUT
# RUN sudo apt install -y \
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    libglew-dev \
    libglfw3-dev \
    freeglut3-dev \
    x11-apps \
    libgl1-mesa-glx 

RUN apt-get update \
    && apt-get install mesa-utils

