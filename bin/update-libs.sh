#/usr/bin/env bash

set -euo pipefail

PROJECT_NAME=grindertimer
LIBS_FOLDER=${PROJECT_NAME}

updateLib() {
  local repo=$1
  local branch=$2
  shift 2
  local files=$*
  local libName=$(basename ${repo})
  echo "Updating library $libName"

  for file in ${files}; do
    curl -s https://raw.githubusercontent.com/${repo}/${branch}/${file} -o ${LIBS_FOLDER}/$(basename ${file})
  done
}

updateLib Seeed-Studio/Grove_4Digital_Display master TM1637.h TM1637.cpp
