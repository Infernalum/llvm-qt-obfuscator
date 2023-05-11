# A simple QT obuscator based on LLVM Infrastracture

Утилита-обфускатор, целевым назначением которого является преобразование исходного кода программ, разрабатываемых в Qt Framework v.5.>= на языке C++ с учетом мета-объектной системы и использованием элементов QML. На данный момент реализована и протестирована лексическая обфускация кода, включающая: 
- Переименование классов, их полей/методов, функций, локальных/глобальных переменных;
- Корректное преобразование qml-элементов, реализованных в C++ классах и декларированных через `QmlRegisterType<>()`, включая изменения .qml файлов (преобразования вызовов  слотов, сигналов и свойств, помеченных с использованием `slots`, `signals` и `Q_PROPERTY`);
- Создание yaml файла-листинга соответствий старых и новых имен, их расположений.

## Требования
- Qt Framework >= 5.12.0;

## Установка
1) Прямая: скачивание утилиты (Releases); windows.zip / linux.zip
2) Вручную: для Unix - Dockerfile, Windows - аналогично Dockerfile. Для установки системы сборки Ninja можно использовать Chocolatey: `choco install ninja`

В дальнейшем будет доступен образ на основе Docker или Kubernetes, без необходимости полноценной установки LLVM.

## Использование

![alt text](https://github.com/Infernalum/llvm-qt-obfuscator/blob/main/Images/help%20ref.png "help reference")

Пример для examples/sshgeygen: 
```
./bin/qt-obfuscator  --seed=1000 -e=RAND -f=../examples/sshkeygen/results.yaml ../examples/sshkeygen/KeyGenerator.cpp ../examples/sshkeygen/main.cpp --qml=../examples/sshkeygen/main.qml  --extra-arg=-I<Путь\ до\ установленных\ библиотек Qt> --
```

## License

MIT
