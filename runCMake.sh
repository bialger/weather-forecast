#!/usr/bin/bash
if (cd ./cmake-build 2>/dev/null); then
  rm -rf ./cmake-build
fi
if (cmake -S ./ -B ./cmake-build && cmake --build ./cmake-build); then
  cp ./cmake-build/weather-forecast ./weather-forecast.out
  echo ''
  if (./weather-forecast.out -h >/dev/null 2>/dev/null); then
    echo 'Congratulations! weather-forecast was compiled successfully.'
    echo ''
    ./weather-forecast.out -h
    echo 'Usage example: '
    echo 'Prompt: '
#    echo './weather-forecast.out tests/testFile.txt -d "\x3b" --tail --lines=0b11'
    echo 'Output: '
#    ./weather-forecast.out tests/testFile.txt -d "\x3b" --tail --lines=0b11
    echo 'Do you want to add this utility to /usr/bin (y/n)?'
    read -r CHOISE
    if [ "$CHOISE" = "y" ]; then
      if (sudo cp ./weather-forecast.out /usr/bin/weather-forecast); then
        echo 'Accepted, run utility with "weather-forecast"'
      else
        echo 'Please try again.'
      fi
    else
      echo 'Declined, run utility with "./weather-forecast.out"'
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
