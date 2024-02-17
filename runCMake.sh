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
  rm -f ~/weather-forecast.run
  ln -s ~/CMakeBuilds/weather-forecast/bin/weather-forecast ~/weather-forecast.run
  echo ''

  if (~/weather-forecast.run -h >/dev/null 2>/dev/null); then
    echo 'Congratulations! weather-forecast was compiled successfully.'
    echo ''
    ~/weather-forecast.run -h
    echo 'Do you want to add this utility to /usr/bin (y/n)? '
    read -r CHOISE

    if [ "$CHOISE" = "y" ]; then
      sudo rm -f /usr/bin/weather-forecast

      if (sudo ln -s ~/CMakeBuilds/weather-forecast/bin/weather-forecast /usr/bin/weather-forecast); then
        if [ -e /etc/weather-forecast ]; then
          sudo rm -rf /etc/weather-forecast
        fi

        if [ -e /opt/weather-forecast ]; then
          sudo rm -rf /opt/weather-forecast
        fi

        sudo rm -f /usr/bin/weather-forecast
        sudo cp -r ~/CMakeBuilds/weather-forecast /opt/weather-forecast
        sudo cp -r ./.config /etc/weather-forecast
        sudo ln -s /opt/weather-forecast/bin/weather-forecast /usr/bin/weather-forecast
        echo 'Accepted, run utility with "weather-forecast"'
      else
        echo 'Please try again later.'
        echo 'Declined, run utility with "~/weather-forecast.run"'
      fi
    else
      echo 'Declined, run utility with "~/weather-forecast.run"'
    fi

    exit 0
  else
    echo 'Oops! Could not execute the program.'
    exit 1
  fi
else
  echo 'Error occurred during running CMake. Make sure that it is installed and configured properly.'
  exit 1
fi
