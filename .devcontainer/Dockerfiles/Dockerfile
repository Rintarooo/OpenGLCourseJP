# https://hub.docker.com/r/nvidia/cudagl/tags?page=1&ordering=last_updated&name=10.1
FROM nvidia/cudagl:10.1-devel-ubuntu18.04

# set environment variable
ARG DEBIAN_FRONTEND noninteractive
ENV WORK_DIR /app
WORKDIR ${WORK_DIR}

ARG TZ=Asia/Tokyo
RUN ln -snf /usr/share/zoneinfo/${TZ} /etc/localtime && echo ${TZ} > /etc/timezone

# GPG error: https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64  InRelease: The following signatures couldn't be verified because the public key is not available: NO_PUBKEY A4B469963BF863CC
# #0 30.83 E: The repository 'https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64  InRelease' is not signed.
RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-keys A4B469963BF863CC

# Build tools + debugger
RUN apt -y update --no-install-recommends \
    && apt install -y --no-install-recommends \
    sudo \
    tzdata \
    git \
    cmake \
    wget \
    unzip \
    build-essential \
    curl \
    vim \
    gdb \
    gnuplot \
    nvidia-cuda-gdb \
    x11-apps \
    zlib1g-dev \
    libjpeg-dev \
    libwebp-dev \
    libpng-dev \
    libtiff5-dev \
    libopenexr-dev \
    libgdal-dev \
    libgtk2.0-dev \
    && apt autoremove -y \
	&& apt clean -y 

# install GLFW, GLEW, GLUT for OpenGL
# https://grainrigi.hatenablog.com/entry/2017/08/27/144218
RUN apt -y update --no-install-recommends \
    && apt install -y --no-install-recommends \
    libglew-dev \
    libglfw3-dev \
    freeglut3-dev \
    && apt autoremove -y \
	&& apt clean -y 