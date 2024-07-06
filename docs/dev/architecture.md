# Архитектура продукта

В этом документе описывается архитектура продукта - парсера аргументов командной
строки, разработанная на основании [требований](requirements.md).

## Системная архитектура

Продукт состоит из нескольких связанных подсистем. Несколько из них - внешние
библиотеки.

* Используется модуль UI для взаимодействия с пользовательским вводом любого рода и
  вывода информации.
* Используется модуль [ArgParser](../../lib/argparser/docs/README.md) - подсистема
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
    subgraph Independent systems
        nodeArgParser([ArgParser])
        subgraph Third-party libraries
            nodeCPR([C++ Requests])
            nodeJSON([JSON])
            nodeFTXUI([FTXUI])
        end
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
    -string kIntervalDescription$
    -string kDaysCountDescription$
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

### Архитектура подсистемы "ArgParser"

Эта подсистема представляет собой набор классов и связей между ними, которые выполняют
непосредственно парсинг аргументов командной строки, передаваемых в подсистему. Все
классы находятся в пространстве имён ArgumentParser.

#### UML-диаграмма

```mermaid
---
title: Diagram of the module ArgParser
---
%%{
  init: {
    'theme': 'base',
    'classDiagram': { 'curve': 'linear' },
  }
}%%
classDiagram
    direction TB
    note for ArgParser "Has pseudonym functions for AddArgument and GetValue for each argument type"
    class ArgParser {
        -const char[] name_;
        -vector~ArgumentBuilder*~ argument_builders_;
        -vector~Argument*~ arguments_;
        -vector~string~ allowed_typenames_;
        -vector~string~ allowed_typenames_for_help_;
        -map~string, map~ string, size_t~~ arguments_by_type_;
        -map~char, string~ short_to_long_names_;
        -size_t help_index_;
        +Parse(vector~string~ args, ConditionalOutput error_output=()) bool
        +Parse(int argc, char[][] argv, ConditionalOutput error_output=()) bool
        +Help() bool
        +HelpDescription() string
        +AddHelp(char short_name, const char[] long_name, const char[] description="") ConcreteArgumentBuilder~bool~ &
        +AddHelp(const char[] long_name, const char[] description="") ConcreteArgumentBuilder~bool~ &
        +AddArgument~T~(char short_name, const char[] long_name, const char[] description="") ConcreteArgumentBuilder~T~ &
        +AddArgument~T~(const char[] long_name, const char[] description="") ConcreteArgumentBuilder~T~ &
        +GetValue~T~(const char[] long_name, size_t index=0) T
        -Parse_(vector~string~ args, ConditionalOutput error_output) bool
        -GetLongKeys(string current_argument) vector~string~
        -ParsePositionalArguments(vector~string~ argv, const vector~size_t~ & used_positions) void
        -HandleErrors(ConditionalOutput error_output) bool
        -RefreshArguments() void
        -AddArgument_~T~(char short_name, const char[] long_name, const char[] description) ConcreteArgumentBuilder~T~ &
        -GetValue_~T~(const char* long_name, size_t index) T
    }
    class Argument {
        <<interface>>
        +ValidateArgument(vector~string~ argv, size_t position)* vector~size_t~
        +CheckLimit()* bool
        +GetValueStatus()* ArgumentParsingStatus
        +GetType()* string
        +GetInfo()* ArgumentInformation
        +GetUsedValues()* size_t
        +ClearStored()* void
        #ObtainValue(vector~string~ argv, string& value_string, vector~size_t~ & used_values, size_t position)* size_t
    }
    class ArgumentBuilder {
        <<interface>>
        +GetInfo()* ArgumentInformation
        +GetDefaultValue()* string
        +build()* Argument*
    }
    class ConcreteArgument~T~ {
        -ArgumentInformation info_
        -ArgumentParsingStatus value_status_
        -size_t value_counter_
        -T value_
        -T default_value_
        -T* stored_value_
        -vector~T~* stored_values_
        +GetValue(size_t index) T
        +ValidateArgument(vector~string~ argv, size_t position) vector~size_t~
        +CheckLimit() bool
        +GetValueStatus() ArgumentParsingStatus
        +GetType() string
        +GetInfo() ArgumentInformation
        +GetUsedValues() size_t
        +ClearStored() void
        #ObtainValue(vector~string~ argv, string& value_string, vector~size_t~ & used_values, size_t position) size_t
    }
    class ConcreteArgumentBuilder~T~ {
        -ArgumentInformation info_;
        -T default_value_;
        -T* stored_value_;
        -vector~T~* stored_values_;
        -bool was_created_temp_vector_;
        +MultiValue(size_t min=0) ConcreteArgumentBuilder &
        +Positional() ConcreteArgumentBuilder&
        +StoreValue(T& value) ConcreteArgumentBuilder&
        +StoreValues(vector~T~ & values) ConcreteArgumentBuilder&
        +Default(T value) ConcreteArgumentBuilder&
        +AddValidate(function~bool(string&)~ validate) ConcreteArgumentBuilder&
        +AddIsGood(function~bool(string&)~ is_good) ConcreteArgumentBuilder&
        +GetInfo() ArgumentInformation
        +GetDefaultValue() string
        +build() Argument*`
    }
    class ArgumentInformation {
        +char short_key = kBadChar
        +const char[] long_key = ""
        +const char[] description = ""
        +string type
        +size_t minimum_values = 0
        +bool is_multi_value = false
        +bool is_positional = false
        +bool has_store_values = false
        +bool has_store_value = false
        +bool has_default = false
        +function~bool(string&)~ validate = &AlwaysTrue
        ++function~bool(string&)~ is_good = &AlwaysTrue
    }
    class ArgumentParsingStatus {
        <<enumeration>>
        NoArgument
        InvalidArgument
        InsufficientArguments
        Success
    }

    ArgParser *-- Argument
    ArgParser *-- ArgumentBuilder
    ArgParser <.. ConcreteArgument
    ArgParser <.. ConcreteArgumentBuilder
    Argument <|.. ConcreteArgument
    ArgumentBuilder <.. Argument
    ArgumentBuilder <|.. ConcreteArgumentBuilder
    ConcreteArgument *-- ArgumentInformation
    ConcreteArgument *-- ArgumentParsingStatus
    ConcreteArgumentBuilder <.. ConcreteArgument
    ConcreteArgumentBuilder *-- ArgumentInformation
```

#### Класс ArgParser

Этот класс является основным классом модуля, именно с ним обычно взаимодействует
пользователь. Он должен предоставлять следующие возможности: добавление обрабатываемого
аргумента (любого из указанных) с указанным ключом, парсинг набора аргументов командной
строки, а также функция добавления аргумента. Кроме того, должен иметь функцию
составления справки и обрабатывать ошибки в синтаксисе аргументов командной строки,
и, в некоторых случаях, выводить их.

#### Класс ArgumentBuilder

Этот класс реализует паттерн проектирования "Builder": ссылки на объекты наследников
этого класса должна возвращать функция добавления аргумента из ArgParser, к нему
должен обращаться пользователь, добавляя информацию об аргументе. Должен
реализовывать функции добавления любой информации про аргумент, представленные в
[тестах](../../tests/argparser_unit_tests.cpp) и функцию построения. Ему должен
наследовать шаблонизированный класс конкретного Builder, который будет реализовывать
вышеуказанный функционал для каждого из типов аргумента.

#### Класс Argument

Этот класс является родительским классом для всех классов аргументов. Должен иметь
функции возврата статуса парсинга и возврата информации об аргументе, а также функцию
получения значения аргумента из аргументов командной строки. Необходимая информация,
не изменяемая в процессе парсинга, должна храниться в виде экземпляра структуры.
На данный момент планируется аргументы всех базовых значащих типов (кроме 8-битных
чисел), StringArgument и ComplexArgument (строка с валидацией и чтением пробелов).
