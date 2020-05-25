#!/bin/sh

cmake_ninja_verbose_opt() {
  echo '-v'
}

cmake_make_verbose_opt() {
  echo 'VERBOSE=1'
}

cmake_generator_verbose_opt() {
  "cmake_${1}_verbose_opt"
}
