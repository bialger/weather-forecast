# Архитектура продукта

В этом документе описывается архитектура продукта - парсера аргументов командной
строки, разработанная на основании [требований](requirements.md).

## Системная архитектура

Продукт состоит из нескольких связанных подсистем. Несколько из них - внешние
библиотеки.

* Используется модуль UI для взаимодействия с пользовательским вводом любого рода и
  вывода информации.
* Используется модуль [ArgParser](https://github.com/bialger/ArgParser/tree/v1.1.0) - подсистема
  для обработки аргументов командной строки.
* Используется модуль Forecast для выполнения и обработки запросов прогноза.
* Используется библиотека [C++ Requests](https://github.com/libcpr/cpr) для выполнения HTTP-запросов.
* Используется библиотека [nlohmann_json](https://github.com/nlohmann/json) для
  обработки JSON-файлов
* Используется библиотека [FTXUI](https://github.com/ArthurSonzogni/FTXUI) для
  работы с TUI, включая пользовательский ввод.

#### UML-диаграмма

```mermaid
---
title: Project system diagram
---
flowchart LR
    subgraph Third-party libraries
        nodeArgParser([ArgParser])
        nodeCPR([C++ Requests])
        nodeJSON([JSON])
        nodeFTXUI([FTXUI])
    end
    nodeForecast([Forecast])
    nodeUI([UI])
    nodeMain[main]
    nodeArgParser --> nodeUI
    nodeForecast --> nodeUI
    nodeCPR --> nodeForecast
    nodeJSON --> nodeForecast
    nodeFTXUI --> nodeUI
    nodeUI --> nodeMain
```

## Архитектура подсистем

Архитектура внешних подсистем лежит вне области рассмотрения данного документа.

### Архитектура подсистемы "UI"

Эта подсистема представляет собой набор классов и связей между ними для любого рода 
взаимодействия с пользователем: от аргументов командной строки до вывода результата.

#### UML-диаграмма

```mermaid
---
title: Diagram of the module UI
---
%%{
  init: {
    'theme': 'base',
    'classDiagram': { 'curve': 'linear' },
  }
}%%
classDiagram
  direction TB
  class TextUserInterface {
    +string kProgramName$
    +CompositeString kDefaultConfigPath$
    +CompositeString kDefaultLogPath$
    -ostream& out_
    -istream& in_
    -ConditionalOutput error_output_
    -ostringstream background_errors_
    -ostringstream background_logs_
    -ArgParser parser_
    -CompositeString config_path_
    +GetPotentialConfigDirectories()$ vector~string~
    +Start(vector~string~ args) int
    -BeginForecast(ConfigParser& config) int
  }
  class TuiWorker {
    +map~int, vector<string>~ kWeatherIcons$;
    -int kMaxWidth$
    -int kMaxHeight$
    -int kFocusLen$
    -Forecaster& forecaster_
    -ConditionalOutput error_output_
    -ConditionalOutput log_output_
    -int interval_
    -vector~Element~ elements_
    -ScreenInteractive screen_
    -size_t start_focus_
    -mutex mutex_
    -int result_
    +Run() int
    -HandleEvent(Event event) bool;
    -RefreshElements() void;
    -RedrawScreen() void;
    -ReloadScreen(Action action) int;
    -FocusNext() void;
    -FocusPrev() void;
    -GetWeatherIcon(int weather_code)$ Element;
    -GetUnit(WeatherTimeUnit unit, string header = "")$ Element;
    -GetDay(WeatherDay day)$ Element;
    -GetCurrentUnit() Element;
  }
  class ConfigParser {
    +int kLowerLimitIntervalSize$
    +int kUpperLimitIntervalSize$
    +string kDefaultLocation$
    -int interval_
    -int days_count_
    -int location_index_
    -ConditionalOutput error_output_
    -string config_dir_
    -vector~string~ locations_
    -string api_key_
    -string config_path_
    +IsValidConfig(string str_config)$ bool
    +ParseConfig() int
    +GetApiKey() string
    +GetConfigDir() string
    +GetInterval() int
    +GetDaysCount() int
    +GetLocationIndex() int
    +GetLocations() vector~string~
    -SetApiKey(string api_key_path) int
  }
  class Action {
    <<enumeration>>
    kNext,
    kPrev,
    kRefresh,
    kAddLine,
    kRemoveLine,
    kNextLine,
    kPrevLine
  }
  TextUserInterface <.. TuiWorker
  TextUserInterface <.. ConfigParser
  TuiWorker <.. Action
```

#### Класс TextUserInterface

Этот класс является основным классом модуля. Должен определять способ вывода ошибок и
то, как обрабатываются аргументы командной строки. Должен содержать метод запуска 
пользовательского интерфейса. Должен выводить ошибки и логи, если они требуются. 
Непосредственно взаимодействует с модулем ArgParser.

#### Класс TuiWorker

Этот класс является представлением текстового интерфейса программы. Должен содержать
метод запуска и все необходимые методы для работы TUI, включая реакцию на 
пользовательский ввод. Должен выводить ошибки и логи, если они требуются. 
Непосредственно взаимодействует с модулем Forecast.

#### Класс ConfigParser

Этот класс является представлением конфигурационного файла и его парсера. Он должен
содержать метод парсинга указанного конфигурационного файла и все прочие необходимые 
методы. Также должен иметь методы возврата полученных значений. Должен выводить ошибки.

### Архитектура подсистемы "Forecast"

Эта подсистема представляет собой набор классов и связей между ними, которые выполняют
поиск прогноза погоды с данными параметрами, и передают назад обработанные результаты
для показа пользователю.

#### UML-диаграмма

```mermaid
---
title: Diagram of the module Forecast
---
%%{
  init: {
    'theme': 'base',
    'classDiagram': { 'curve': 'linear' },
  }
}%%
classDiagram
  direction TB
  class Forecaster {
    +int kLowerLimitDaysCount$
    +int kUpperLimitDaysCount$
    -vector~string~ locations_
    -int days_count_
    -int location_index_
    -string last_time_
    -string api_key_
    -Geocoder geocoder_
    -vector~WeatherDay~ forecast_
    -WeatherTimeUnit current_weather_
    -JsonCache geocoder_cache_
    -ConditionalOutput error_output_
    -ConditionalOutput log_output_
    +ObtainForecast() int
    +SwapToNext() int
    +SwapToPrev() int
    +AddDay() int
    +RemoveDay() int
    +GetForecast() vector~WeatherDay~
    +GetCurrentWeather() WeatherTimeUnit
    +GetLocation() string
    +GetLastForecastTime() string
    +GetErrorOutput() ConditionalOutput
    +GetLogOutput() ConditionalOutput
    -RequestPosition() int
    -RequestForecast() int
    -ProcessPosition(json answer) int
    -ProcessForecast(json answer) int
  }
  class JsonCache {
    -string cache_group_
    -string cache_dir_
    +PutJsonToCache(string cache_name, json data) void
    +GetJsonFromCache(string cache_name) json
    -GetCacheFilename(string cache_name) string
  }
  class WeatherDay {
    +int kUnitsInDay$
    +int kHoursInDay$
    +int kDaysInForecast$
    +map~int, string~ kWeatherCodeToString$
    +WeatherCharNames kShownNames$
    +string kWeatherUrl$
    -string date_
    -vector~WeatherTimeUnit~ units_
    -map~string, json~ hourly_values_
    -map~string, json~ daily_values_
    +GetCurrentWeather(json forecast)$ WeatherTimeUnit
    +SetForecast(json forecast, int day_number) bool
    +GetForecastUnits() vector~WeatherTimeUnit~
    +GetDate() string
    -SetJsonProperties(json forecast) bool
  }
  class WeatherTimeUnit {
    +map~string, string~ kChargeUnits$
    +map~int, string~ kWeatherCodeToString$
    +WeatherCharNames kShownNames$
    +int weather_type
    +int real_temperature
    +int felt_temperature
    +int wind_speed_lower
    +int wind_speed_upper
    +double visibility;
    +double pressure;
    +double precipitation;
    +double uv_index;
    +int humidity
    -string name_
    +GetAllAsMap() map~string, string~
    +GetName() string
  }
  class WeatherCharNames {
    +string time
    +string weather_code
    +string temperature
    +string apparent_temperature
    +string wind_speed
    +string visibility
    +string pressure
    +string precipitation
    +string uv_index
    +string humidity
    +string dates
  }
  class Geocoder {
    +string KGeocoderUrl$
    -string longitude_
    -string latitude_
    -string api_key_
    +SetCoordinates(json geocode) int
    +SetApiKey(string path) int
    +GetLongitude() string
    +GetLatitude() string
  }
  Forecaster *-- Geocoder
  Forecaster *-- WeatherTimeUnit
  Forecaster *-- WeatherDay
  Forecaster *-- JsonCache
  WeatherDay *-- WeatherTimeUnit
  WeatherDay *-- WeatherCharNames
  WeatherTimeUnit *-- WeatherCharNames
```

#### Класс Forecaster

Этот класс является основным классом модуля. Должен выводить ошибки и логи, если они 
требуются. Должен содержать методы запросов к 
[Yandex Geocoder API](https://yandex.ru/dev/geocode/doc/ru/) и
[Open Meteo API](https://open-meteo.com/en/docs#latitude=59.94&longitude=30.31&hourly=temperature_2m&forecast_days=16),
метод получения результата по дням, а также методы навигации по дням и локациям. 
Результат хотя бы некоторых запросов должен быть кеширован.

#### Класс JsonCache

Этот класс является абстракцией для представления кешированного результата запроса к
API в формате JSON. Должен содержать методы для кеширования и получения из кэша
запроса по имени.

#### Класс Geocoder

Этот класс представляет собой набор данных для геокодирования. Должен содержать 
информацию для выполнения запроса и его результат. Должен содержать методы для 
обработки конфигурационных файлов и ответов. Не обязан иметь метод для 
Internet-запроса.

#### Класс WeatherDay

Этот класс является абстракцией для представления погоды за день. Должен содержать
методы для обработки погоды за каждый наименьший временной промежуток и метод 
получения результатов.

#### Класс WeatherTimeUnit

Этот класс является абстракцией для представления погоды за наименьший временной 
промежуток: время суток. Должен иметь публичные поля для всех характеристик и метод,
универсально возвращающий их всех.
