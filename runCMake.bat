@cd cmake-build && @cd .. && @rmdir /s /q cmake-build
(cmake -S . -B cmake-build -DCMAKE_BUILD_TYPE=Release && cmake --build cmake-build --target weather-forecast && @mklink weather-forecast.exe cmake-build\Debug\weather-forecast.exe && echo weather-forecast installed successfully && .\weather-forecast.exe -h) || echo Something went wrong
