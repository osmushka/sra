# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/osmushka/work/sra/build-bench/_deps/benchmark-src"
  "/home/osmushka/work/sra/build-bench/_deps/benchmark-build"
  "/home/osmushka/work/sra/build-bench/_deps/benchmark-subbuild/benchmark-populate-prefix"
  "/home/osmushka/work/sra/build-bench/_deps/benchmark-subbuild/benchmark-populate-prefix/tmp"
  "/home/osmushka/work/sra/build-bench/_deps/benchmark-subbuild/benchmark-populate-prefix/src/benchmark-populate-stamp"
  "/home/osmushka/work/sra/build-bench/_deps/benchmark-subbuild/benchmark-populate-prefix/src"
  "/home/osmushka/work/sra/build-bench/_deps/benchmark-subbuild/benchmark-populate-prefix/src/benchmark-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/osmushka/work/sra/build-bench/_deps/benchmark-subbuild/benchmark-populate-prefix/src/benchmark-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/osmushka/work/sra/build-bench/_deps/benchmark-subbuild/benchmark-populate-prefix/src/benchmark-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
