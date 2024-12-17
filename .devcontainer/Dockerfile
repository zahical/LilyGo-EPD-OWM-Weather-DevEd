FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive

RUN : \
  && apt-get update \
  && apt-get install -y \
    apt-utils \
    ca-certificates \
    curl \
    git \
    python3 \
    python3-pip \
    python3-venv \
    unzip \
    zip \
    libncurses5-dev \
    flex \
    bison \
    libssl-dev \
    libcrypto++-dev \
  && apt-get autoremove -y \
  && rm -rf /var/lib/apt/lists/* \
  && update-alternatives --install /usr/bin/python python /usr/bin/python3 10 \
  && :

RUN pip install pillow freetype-py

RUN pip install platformio==6.1.11

RUN pio system completion install bash

RUN echo "source /root/.bash_completion" >> /root/.bashrc

ENV LC_ALL=C.UTF-8
CMD [ "/bin/bash" ]
