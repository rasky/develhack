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
# devkitPRO + devkitARM + libraries
#

ENV DEVKITPRO "/usr/local/devkitPRO"
ENV DEVKITARM "${DEVKITPRO}/devkitARM"

RUN wget 'http://downloads.sourceforge.net/project/devkitpro/Automated%20Installer/devkitARMupdate.pl' -O- \
    | perl


#
# grit
#

RUN wget 'http://www.coranac.com/files/grit/grit-src-0.8.6.zip' \
	&& apt-get -y install unzip libfreeimage-dev \
	&& unzip grit-src-0.8.6.zip \
	&& sed -e 's/LIBDIRS :=/LIBDIRS := . \/usr\/lib\//g' \
	       -e 's/-static//g' \
	       -i grit_src/Makefile \
	&& make -C grit_src \
	&& PREFIX=/usr/local/bin make -C grit_src install

#
# Code
#

RUN mkdir -p /home/code
WORKDIR /home/code
