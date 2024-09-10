cmake_minimum_required(VERSION 3.12)

project(SentimentAnalysis CXX)

set(CMAKE_MODULE_PATH cmake)

file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS src/*.h)

add_executable(SentimentAnalysis ${SOURCES} ${HEADERS})

target_include_directories(SentimentAnalysis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/lib)

if(WIN32)
	target_link_libraries(SentimentAnalysis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/CURL/libcurl_a.lib)
	target_link_libraries(SentimentAnalysis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/CURL/libcurl_a_debug.lib)
	target_link_libraries(SentimentAnalysis PRIVATE Normaliz.lib Ws2_32.lib Wldap32.lib Crypt32.lib advapi32.lib)
	target_link_libraries(SentimentAnalysis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/haru/hpdf.lib)
endif(WIN32)
if(UNIX)
	find_package(CURL REQUIRED)
	target_link_libraries(SentimentAnalysis PRIVATE CURL::libcurl)
	find_package(OpenSSL REQUIRED)
	target_link_libraries(SentimentAnalysis PRIVATE OpenSSL::SSL)
	target_link_libraries(SentimentAnalysis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/haru/libhpdf.a)
endif(UNIX)

# ------------
# Google tests
# ------------

include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(SaTests "${CMAKE_CURRENT_SOURCE_DIR}/srcTests/test.cpp")

target_include_directories(SaTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/lib)

if(WIN32)
	target_link_libraries(SaTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/CURL/libcurl_a.lib)
	target_link_libraries(SaTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/CURL/libcurl_a_debug.lib)
	target_link_libraries(SaTests PRIVATE Normaliz.lib Ws2_32.lib Wldap32.lib Crypt32.lib advapi32.lib)
	target_link_libraries(SaTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/haru/hpdf.lib)
endif(WIN32)
if(UNIX)
	target_link_libraries(SaTests PRIVATE CURL::libcurl)
	target_link_libraries(SaTests PRIVATE OpenSSL::SSL)
	target_link_libraries(SaTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/haru/libhpdf.a)
endif(UNIX)

target_link_libraries(SaTests PRIVATE GTest::gtest_main)


include(GoogleTest)
gtest_discover_tests(SaTests)
