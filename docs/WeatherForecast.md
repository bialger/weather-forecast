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

### Linux и MacOs

* Сначала, если Вы этого не сделали, клонируйте проект и перейдите в его каталог^

```shell
git clone https://github.com/bialger/weather-forecast.git && cd weather-forecast
```

Затем есть два варианта.

#### Автоматизированная сборка (только для Linux)

* Сделайте исполняемым и запустите [shell-script установки](../runCMake.sh), затем
  следуйте инструкциям:

```shell
chmod +x ./runCMake.sh && ./runCMake.sh
```

#### Ручная сборка

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

### Windows 


