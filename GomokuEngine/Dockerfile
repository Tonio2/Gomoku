FROM debian

RUN export DEBIAN_FRONTEND=noninteractive DEBCONF_NONINTERACTIVE_SEEN=true \
    && apt-get -q update \
    && apt-get -q dist-upgrade -y \
    && apt-get -q install build-essential make cmake g++ python3-dev libgtest-dev -y \
    && rm -rf /var/lib/apt/lists/*
