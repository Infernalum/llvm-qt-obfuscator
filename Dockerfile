FROM gcc:latest

# Для LLVM используются дефолтные системы сборки cmake и ninja
RUN apt update && apt-get install -y \
    git ninja-build build-essential libtool autoconf unzip \
    libglu1-mesa-dev freeglut3-dev mesa-common-dev wget cmake

# Ставим LLVM
RUN cd home && git clone --depth 1 https://github.com/llvm/llvm-project.git && cd llvm-project && \
    mkdir build && cd build && cmake -G Ninja ../llvm -DCMAKE_BUILD_TYPE=MinSizeRel -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra"     

ADD qt-obfuscator /home/llvm-project/clang-tools-extra/qt-obfuscator

RUN cd home/llvm-project && echo 'add_subdirectory(qt-obfuscator)' >> clang-tools-extra/CMakeLists.txt && \
    cd build && ninja qt-obfuscator

ADD qt5 /usr/include/x86_64-linux-gnu/qt5
ADD examples /home/examples

