project("Xli")

cmake_minimum_required(VERSION 2.8)

include(cmake/modules/XliLibSetup.cmake)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${LIBRARY_OUTPUT_PATH})

if (APPLE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-switch-enum -Wno-logical-op-parentheses -Wno-parentheses-equality -Wno-extra-tokens -Wno-deprecated-declarations")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch-enum -Wno-logical-op-parentheses -Wno-parentheses-equality")
endif()

add_subdirectory(src/Xli)

if (NOT XLI_NO_PLATFORM)
    add_subdirectory(src/XliPlatform)
endif()

if (NOT XLI_NO_MAIN)
    add_subdirectory(src/XliMain)
endif()

if (NOT XLI_NO_GL)
    add_subdirectory(src/XliGL)
endif()

if (NOT XLI_NO_IMAGE)
    add_subdirectory(src/XliImage)
endif()

if (NOT XLI_NO_FONT)
    add_subdirectory(src/XliFont)
endif()

if (NOT XLI_NO_JSON)
    add_subdirectory(src/XliJson)
endif()

if (NOT XLI_NO_ZIP)
    add_subdirectory(src/XliZip)
endif()

if (NOT XLI_NO_HTTP_CLIENT)
    add_subdirectory(src/XliHttpClient)
endif()

if (NOT XLI_NO_SOUND_PLAYER)
    add_subdirectory(src/XliSoundPlayer)
endif()

if (NOT XLI_NO_FEATURES)
    add_subdirectory(src/XliFeatures)
endif()
