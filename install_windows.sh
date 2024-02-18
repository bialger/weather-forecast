#!/usr/bin/bash

if [ -e ~/CMakeBuilds/weather-forecast ]; then
  rm -rf ~/CMakeBuilds/weather-forecast
fi

if [ -e ~/.config/weather-forecast ]; then
  rm -rf ~/.config/weather-forecast
fi

if (cmake -S . -B ~/CMakeBuilds/weather-forecast -DCMAKE_BUILD_TYPE=Release && cmake --build ~/CMakeBuilds/weather-forecast --target weather-forecast); then
  echo 'Enter your Yandex Geocoder API key: '
  read -r API_KEY && echo "$API_KEY" > "./.config/yandex_api_key.apikey"
  cp -rf ./.config ~/.config/weather-forecast
  rm -f ~/weather-forecast.exe
  ln -s ~/CMakeBuilds/weather-forecast/Debug/weather-forecast.exe ~/weather-forecast.exe
  echo ''

  if (~/weather-forecast.exe -h >/dev/null 2>/dev/null); then
    echo 'Congratulations! weather-forecast was compiled successfully.'
    echo ''
    ~/weather-forecast.exe -h
    echo "Run with ~/weather-forecast.exe"
    exit 0
  else
    echo 'Oops! Could not execute the program.'
    exit 1
  fi
else
  echo 'Error occurred during running CMake. Make sure that it is installed and configured properly.'
  exit 1
fi
