#LICENSE public domain (Unlicense)
#Created for https://github.com/electrorys/tfcipher
#AUTHORS:
# Daniel Norte de Moraes <danielcheagle@gmail.com>
# Renan Oliveira  <renanoliveira1988@gmail.com>

cmake_minimum_required(VERSION 3.9)
project(tfcipher VERSION 20190818 DESCRIPTION "Skein and Threefisth symmetric ciphers" LANGUAGES C)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(MY_INCLUDE_HEADERS tfdef.h skein.h tfe.h tfprng.h)

set(MY_MAIN_SRC tfe.c tfprng.c tfblk.c tfctrapi.c tfecb.c tftweakapi.c
                numconv.c tfctr.c tfocb.c tfstream.c tfxts.c skein.c tfcbc.c
                tfdec.c tfenc.c tfpcbc.c)
set(MY_SUPPORT_SRC mhexdump.c xstrlcpy.c)

set(CMAKE_C_FLAGS "-Wall -Wextra -Wshadow -Wundef -march=native -mtune=native")

set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O3")

add_library(threefish STATIC ${MY_MAIN_SRC} ${MY_INCLUDE_HEADERS})
target_include_directories(threefish PRIVATE . )

add_library(threefish_supp STATIC ${MY_SUPPORT_SRC})
target_include_directories(threefish_supp PRIVATE . )

add_library(threefish_shared SHARED ${MY_MAIN_SRC} ${MY_INCLUDE_HEADERS})
set_target_properties(threefish_shared PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1)
target_include_directories(threefish PRIVATE . )

add_library(threefish_supp_shared SHARED ${MY_SUPPORT_SRC})
set_target_properties(threefish_supp_shared PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1)
target_include_directories(threefish_supp PRIVATE . )

if (build_examples)
  add_executable(tfcrypt tfcrypt.c)
  target_link_libraries(tfcrypt threefish threefish_supp)

  add_executable(tftwcrypt tftwcrypt.c)
  target_link_libraries(tftwcrypt threefish threefish_supp)

  add_executable(skeinhash skeinhash.c)
  target_link_libraries(skeinhash threefish threefish_supp)

  add_executable(tftest tftest.c)
  target_link_libraries(tftest threefish threefish_supp)

  add_executable(tfrand tfrand.c)
  target_link_libraries(tfrand threefish threefish_supp)

  add_executable(tfblktst tfblktst.c)
  target_link_libraries(tfblktst threefish threefish_supp)

  add_executable(tferand tferand.c)
  target_link_libraries(tferand threefish threefish_supp)

  add_executable(tfecrypt tfecrypt.c)
  target_link_libraries(tfecrypt threefish threefish_supp)

  add_executable(tfetest tfetest.c)
  target_link_libraries(tfetest threefish threefish_supp)

  add_executable(genrconst genrconst.c)
  target_link_libraries(genrconst threefish threefish_supp)

endif()


include(GNUInstallDirs)

install(TARGETS threefish threefish_supp threefish_shared threefish_supp_shared
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(FILES ${MY_INCLUDE_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tfcipher/ )
