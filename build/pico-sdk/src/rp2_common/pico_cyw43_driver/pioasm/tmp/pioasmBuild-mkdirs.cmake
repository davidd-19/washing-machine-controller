# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/david/pico/pico-sdk/tools/pioasm"
  "/mnt/e/uni/emb soft/retake/build/pioasm"
  "/mnt/e/uni/emb soft/retake/build/pioasm-install"
  "/mnt/e/uni/emb soft/retake/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/tmp"
  "/mnt/e/uni/emb soft/retake/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp"
  "/mnt/e/uni/emb soft/retake/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src"
  "/mnt/e/uni/emb soft/retake/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/mnt/e/uni/emb soft/retake/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/mnt/e/uni/emb soft/retake/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
