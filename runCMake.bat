@cd cmake-build && @cd .. && @rmdir /s /q cmake-build
(cmake -S . -B cmake-build -G "Unix Makefiles" && cmake --build cmake-build && @copy cmake-build\weather-forecast.exe weather-forecast.exe && echo PrintFile installed successfully && .\weather-forecast.exe -h) || echo Something went wrong
