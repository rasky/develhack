FROM ubuntu:16.04

#
# Setup
#

ENV DEBIAN_FRONTEND noninteractive

RUN sed -e 's/archive.ubuntu.com/it.archive.ubuntu.com/g' \
        -e '/deb-src/d' \
        -i /etc/apt/sources.list \
    && dpkg --add-architecture i386 \
    && apt-get update \
    && apt-get -y install \
        build-essential \
        cmake \
        dosfstools \
        git \
        lua5.3:i386 \
        wget

#
# devkitPRO + devkitARM + libraries
#

ENV DEVKITPRO "/usr/local/devkitPRO"
ENV DEVKITARM "${DEVKITPRO}/devkitARM"

RUN wget 'http://downloads.sourceforge.net/project/devkitpro/Automated%20Installer/devkitARMupdate.pl' -O- \
    | perl

ENV PATH "${DEVKITARM}/bin:${PATH}"

#
# Code
#

RUN mkdir -p /home/code
WORKDIR /home/code
