set(CONAN_URL_PREFIX "https://github.com/conan-io/conan/releases/latest/download")

set(CONAN_ERROR_MESSAGE "Please install Conan globally (https://docs.conan.io/2/installation.html), \
                            make sure it is available in the PATH and call CMake again with -DUSE_CONAN_FROM_PATH=ON.")
set(PLATFORM_ERROR_MESSAGE "Your platform could not be detected or Conan does not provide \
                    a binary package for your system architecture. ${CONAN_ERROR_MESSAGE}")

if (USE_CONAN_FROM_PATH)
    message(STATUS "[of] Using Conan from PATH")
    set(CONAN_EXECUTABLE "conan" CACHE STRING "" FORCE)
else()
    of_probe_if_host_is_64bit(
        HOST_VALUES "AMD64" "ARM64" "amd64" "arm64" "x86_64" "x64" "x86-64"
        OUTPUT_VARIABLE IS_64_BIT
    )

    if (WIN32)
        if (IS_64_BIT)
            set(CONAN_URL "${CONAN_URL_PREFIX}/conan-win-64.zip")
        else()
            set(CONAN_URL "${CONAN_URL_PREFIX}/conan-win-32.zip")
        endif()

    elseif (APPLE)
        if (${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "arm64" OR ${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "ARM64")
            set(CONAN_URL "${CONAN_URL_PREFIX}/conan-macos-arm64.zip")
        else()
            message(FATAL_ERROR ${PLATFORM_ERROR_MESSAGE})
        endif()

    elseif (UNIX)
        if (IS_64_BIT)
            set(CONAN_URL "${CONAN_URL_PREFIX}/conan-linux-64.tar.gz")
        else()
            message(FATAL_ERROR ${PLATFORM_ERROR_MESSAGE})
        endif()
    else()
        message(FATAL_ERROR ${PLATFORM_ERROR_MESSAGE})
    endif()

    message(STATUS "[of] Initializing Conan")
    include(FetchContent)
    FetchContent_Declare(conan URL ${CONAN_URL})
    FetchContent_MakeAvailable(conan)

    if (WIN32)
        set(CONAN_COMMAND "${conan_SOURCE_DIR}/conan.exe" CACHE FILEPATH "" FORCE)
    else()
        set(CONAN_COMMAND "${conan_SOURCE_DIR}/conan" CACHE FILEPATH "" FORCE)
    endif()
endif()

execute_process(
    COMMAND ${CONAN_COMMAND} --version
    RESULT_VARIABLE CONAN_VERSION_RESULT
    OUTPUT_VARIABLE CONAN_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (NOT CONAN_VERSION_RESULT EQUAL 0)
    message(FATAL_ERROR "Conan executable not found. ${CONAN_ERROR_MESSAGE}")
else()
    message(STATUS "[of] ${CONAN_VERSION} found: ${CONAN_COMMAND}")
endif()

set(__PROVIDER_URL "https://raw.githubusercontent.com/conan-io/cmake-conan/develop2/conan_provider.cmake")
set(__PROVIDER_FILE "${CMAKE_BINARY_DIR}/conan_provider.cmake")

if (NOT EXISTS ${__PROVIDER_FILE})
    message(STATUS "[of] Downloading conan_provider.cmake")
    file(DOWNLOAD
        ${__PROVIDER_URL}
        ${__PROVIDER_FILE}
        SHOW_PROGRESS 
        TLS_VERIFY ON
        STATUS __STATUS_LIST
    )
    list(GET __STATUS_LIST 0 __STATUS)
    if (NOT ${__STATUS} EQUAL 0)
        file(REMOVE ${__PROVIDER_FILE})
        message(FATAL_ERROR "Failed to download conan provider from ${PROVIDER_URL}. \
                                Either your internet connection is bad or the \
                                file was moved, in this case please report the incident.")
    endif()
endif()