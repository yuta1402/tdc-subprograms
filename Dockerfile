FROM ubuntu:18.04

RUN sed -i.bak -E 's!(archive|security).ubuntu.com/ubuntu!ftp.jaist.jp/pub/Linux/ubuntu!' /etc/apt/sources.list && \
    apt-get -y update && \
    apt-get -y install git build-essential cmake clang libc++-dev libc++abi-dev && \
    apt-get clean

CMD ["/bin/bash"]
