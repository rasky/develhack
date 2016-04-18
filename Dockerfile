FROM ubuntu:16.04

#
# Setup
#

ENV DEBIAN_FRONTEND noninteractive

RUN sed -e 's/archive.ubuntu.com/it.archive.ubuntu.com/g' \
        -e '/deb-src/d' \
        -i /etc/apt/sources.list \
    && apt-get update \
    && apt-get -y install \
        build-essential \
        cmake \
        git \
        wget

#
# devkitARM
#

ENV DEVKITPRO "/usr/local/devkitPRO"
ENV DEVKITARM "${DEVKITPRO}/devkitARM"

RUN mkdir "${DEVKITPRO}" \
    && wget 'http://heanet.dl.sourceforge.net/project/devkitpro/devkitARM/devkitARM_r45/devkitARM_r45-x86_64-linux.tar.bz2' -O- \
    | tar jx -C "${DEVKITPRO}"

#
# libnds
#

RUN git clone https://github.com/devkitPro/libnds libnds -b v1.5.12 \
    && cd libnds \
    && make \
    && make install \
    && cd .. \
    && rm -rf libnds

#
# Code
#

RUN mkdir -p /home/code
WORKDIR /home/code
