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
        git \
        lua5.3:i386 \
        mtools \
        python-minimal \
        wget

#
# devkitPRO + devkitARM + libraries
#

ENV DEVKITPRO "/usr/local/devkitPRO"
ENV DEVKITARM "${DEVKITPRO}/devkitARM"

# Work around tar being unable to extract files on Circle CI
ENV TAR_OPTIONS "--no-same-owner"

RUN wget 'http://netcologne.dl.sourceforge.net/project/devkitpro/Automated%20Installer/devkitARMupdate.pl' -O- \
    | sed 's/download.sourceforge.net/netcologne.dl.sourceforge.net/g' \
    | perl

ENV PATH "${DEVKITARM}/bin:${PATH}"

#
# Patched Grit
#

RUN apt-get install -y autoconf automake libfreeimage-dev libtool \
    && cd /tmp \
    && git clone --depth=1 git://github.com/eli-green/grit \
    && cd grit \
    && ./autogen.sh \
    && ./configure \
    && make \
    && cp grit /usr/local/devkitPRO/devkitARM/bin/grit \
    && cd .. \
    && rm -rf grit

#
# Code
#

RUN mkdir -p /home/code
WORKDIR /home/code
