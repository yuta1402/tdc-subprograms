FROM ubuntu:18.04

RUN apt-get -y update && \
    apt-get -y install git build-essential cmake clang libc++-dev libc++abi-dev && \
    apt-get clean

CMD ["/bin/bash"]
