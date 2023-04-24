FROM gcc:latest

RUN apt update && apt-get install -y \
    git ninja-build build-essential libtool autoconf unzip \
    libglu1-mesa-dev freeglut3-dev mesa-common-dev wget

# Ставим новую версию cmake из Kitware, 
# ибо для LLVM cmake 3.18 версии из оф репозитория мало (нужен >= 3.20)
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null \ 
    | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null && \
    echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ bionic main' \ 
    | tee /etc/apt/sources.list.d/kitware.list >/dev/null && \
    apt-get update && rm /usr/share/keyrings/kitware-archive-keyring.gpg && \
    apt-get install kitware-archive-keyring && \
    echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ bionic-rc main' \ 
    | tee -a /etc/apt/sources.list.d/kitware.list >/dev/null && apt-get update && \
    apt-get install -y cmake

# Ставим LLVM
RUN cd home && git clone --depth 1 https://github.com/llvm/llvm-project.git && cd llvm-project && \
    mkdir build && cd build && cmake -G Ninja ../llvm -DCMAKE_BUILD_TYPE=MinSizeRel -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra"     

ADD qt-obfuscator /home/llvm-project/clang-tools-extra/qt-obfuscator

RUN cd home/llvm-project && echo 'add_subdirectory(qt-obfuscator)' >> clang-tools-extra/CMakeLists.txt && \
    cd build && ninja qt-obfuscator

ADD qt5 /usr/include/x86_64-linux-gnu/qt5
ADD examples /home/examples


