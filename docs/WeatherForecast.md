# Документация программы weather-forecast

Этот документ описывает сборку из исходного кода и использование программы
weather-forecast.

## Зависимости

Для успешной компиляции требует установленного `Git`, `CMake` и компилятора C++,
поддерживающего стандарт C++20.
Также потребуется ключ от [Yandex Geocoder API](https://yandex.ru/dev/geocode/doc/ru/).

**Важно!**
Для ОС Windows успешная сборка и компиляция возможна **только** при
установленном `Visual Studio 2017+`.

## Как собрать

В данном документе описана исключительно процедура сборки главного исполняемого 
файла. Процесс сборки тестов опсан не будет, его можно восстановить из
[CI/CD скрипта](../.github/workflows/ci_tests.yml).

### Автоматическая сборка и установка

Сделайте исполняемым и запустите [shell-script установки](../install.sh), затем следуйте 
инструкциям. **Важно!** Для ОС Windows автоматизированная сборка возможна только
при запуске в Git Bash.

```shell
chmod +x ./install.sh && ./install.sh
```

### Ручная сборка

* Сначала, если Вы этого не сделали, клонируйте проект и перейдите в его каталог:

```shell
git clone https://github.com/bialger/weather-forecast.git && cd weather-forecast
```

#### Linux и MacOs

* Создайте Release-кеш CMake:

```shell
cmake -S . -B ~/CMakeBuilds/weather-forecast -DCMAKE_BUILD_TYPE=Release
```

* Соберите проект из этого кеша:

```shell
cmake --build ~/CMakeBuilds/weather-forecast --target weather-forecast
```

* Введите ключ от [Yandex Geocoder API](https://yandex.ru/dev/geocode/doc/ru/) и 
  запишите его в соответсвующий файл:

```shell
read -r API_KEY && echo "$API_KEY" > "./.config/yandex_api_key.apikey"
```

* Скопируйте в конфигурационный каталог файлы конфигурации:

```shell
cp -r ./.config ~/.config/weather-forecast
```

* Создайте символьную ссылку на исполняемый файл (~/weather-forecast.run):

```shell
ln -s ~/CMakeBuilds/weather-forecast/bin/weather-forecast ~/weather-forecast.run
```

#### Windows 

* Создайте Release-кеш CMake:

```shell
cmake -S . -B "%userprofile%\CMakeBuilds\weather-forecast" -DCMAKE_BUILD_TYPE=Release
```

* Соберите проект из этого кеша:

```shell
cmake --build "%userprofile%\CMakeBuilds\weather-forecast" --target weather-forecast
```

* Введите ключ от [Yandex Geocoder API](https://yandex.ru/dev/geocode/doc/ru/) и
  запишите его в соответсвующий файл:

```shell
SET /P API_KEY="Enter your Yandex Geocoder API key: " && echo %API_KEY% > ".config\yandex_api_key.apikey"
```

* Скопируйте в конфигурационный каталог файлы конфигурации:

```shell
xcopy /si .config "%userprofile%\.config\weather-forecast"
```

* Создайте символьную ссылку на исполняемый файл (HOME/weather-forecast.exe).

```shell
mklink "%userprofile%\weather-forecast.exe" "%userprofile%\CMakeBuilds\weather-forecast\Debug\weather-forecast.exe"
```

## Использование
