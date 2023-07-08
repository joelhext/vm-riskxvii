FROM ubuntu

RUN apt-get --assume-yes --quiet update \
 && apt-get --assume-yes --quiet install \
    apt-utils gosu sudo autotools-dev curl libmpc-dev \
    autoconf automake libmpfr-dev libgmp-dev gawk build-essential  \
    bison flex texinfo gperf libtool patchutils bc zlib1g-dev git libexpat1-dev


# RUN mkdir /opt/riscv32i 

# RUN git clone https://github.com/riscv/riscv-gnu-toolchain riscv-gnu-toolchain-rv32i \
#  && cd /riscv-gnu-toolchain-rv32i \
#  && git checkout 51c7370 \
#  && git submodule update --init --recursive \
#  && mkdir build && cd build \
#  && ../configure --with-arch=rv32i --prefix=/opt/riscv32i \
#  && make -j$(nproc) \
#  && rm -rf /riscv-gnu-toolchain-rv32i 



ENTRYPOINT ["/bin/bash"]