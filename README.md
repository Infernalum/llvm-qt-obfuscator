# A simple QT obuscator based on LLVM Infrastracture

Утилита-обфускатор, целевым назнвачением которого является преобразование исходного кода программ, разрабатываемых в Qt Framework v.5.>= на языке C++ с учетом мета-объектной системы и использованием элементов QML. На данный момент реализована и протестирована лексическая обфускация кода, включающая: 
- Переименование классов, их полей/методов, функций, локальных/глобальных переменных;
- Корректное преобразование qml-элементов, реализованных в C++ классах и декларированных через `QmlRegisterType<>()`, включая изменения .qml файлов (преобразования вызовов  слотов, сигналов и свойств, помеченных с использованием `slots`, `signals` и `Q_PROPERTY`);
- Создание yaml файла-листинга соответствий старых и новых имен, их расположений.

## Требования

- Unix OS (протестировано на Linux Mint 21.0 и Ubuntu >=20.04);
- Qt Framework >= 5.12.0;
- clang >= 15.0.0;
- ninja (repo: https://github.com/ninja-build/ninja);
- LLVM >= 15.0. (repo: https://github.com/llvm/llvm-project);

## Установка
- Утилита функционирует в среде LLVM, поэтому для работы необходимо [установить библиотеки и препроцессор инфрастуктуры LLVM](https://llvm.org/docs/GettingStarted.html#getting-the-source-code-and-building-llvm) с флагами -DCMAKE_BUILD_TYPE=MinSizeRel -LLVM_BUILD_EXAMPLES=OFF -DLLVM_INSTALL_UTILS=OFF
- Переместить папку qt-obfuscator в ../llvm-project/clang-tools-extra/
- echo 'add_subdirectory(qt-obfuscator)' >> ../llvm-project/clang-tools-extra/CMakeLists.txt
- cd ../llvm-project/build && ninja qt-obfuscator
- ./bin/qt-obfuscator

В дальнейшем будет доступен образ на основе Docker или Kubernetes, без необходимости полноценной установки LLVM.

## Использование

![alt text](https://github.com/Infernalum/llvm-qt-obfuscator/blob/main/Images/help%20ref.png "help reference")

Пример для examples/sshgeygen: 
```
./bin/qt-obfuscator  --seed=1000 -e=RAND -f=../examples/sshkeygen/results.yaml ../examples/sshkeygen/KeyGenerator.cpp ../examples/sshkeygen/main.cpp --qml=../examples/sshkeygen/main.qml  --extra-arg=-I<Путь\ до\ установленных\ библиотек Qt> --
```

## License

MIT
