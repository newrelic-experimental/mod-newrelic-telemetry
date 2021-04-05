cmake_minimum_required(VERSION 3.16)
project(mod_newrelic_telemetry)

set(CMAKE_CXX_STANDARD 17)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")


#
# Boost
# Download Boost 1.75.0, decompress it, set these two variables to point to the resulting directory
#
set(BOOST_ROOT "$HOME/CLionProjects/boost_1_75_0")
set(Boost_INCLUDE_DIR "/home/mike/CLionProjects/boost_1_75_0")
set(BOOST_REQUESTED_VERSION 1.75.0)
set(BOOST_USE_STATIC_LIBS false)
set(OPENSSL_USE_STATIC_LIBS  TRUE)

find_package(Boost 1.75.0 REQUIRED)
find_package(APR REQUIRED)
find_package(APACHE REQUIRED)
find_package(OpenSSL)

include_directories(${APR_INCLUDE_DIR})
include_directories(${APACHE_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})

message("SSL: ${OPENSSL_LIBRARIES}")
#target_link_libraries(mod_newrelic_telemetry ${SSL_LIB})
#add_library(mod_newrelic_telemetry SHARED mod_newrelic_telemetry.cpp mod_newrelic_telemetry.h Trace.cpp Trace.h Telemetry.cpp Telemetry.h Span.cpp Span.h ModuleMonitor.cpp ModuleMonitor.h SpanCommon.cpp SpanCommon.h TraceWriter.cpp TraceWriter.h)
add_library(mod_newrelic_telemetry SHARED mod_newrelic_telemetry.cpp ModuleMonitor.cpp Span.cpp SpanCommon.cpp Telemetry.cpp Trace.cpp TraceWriter.cpp Util.cpp Util.h root_certificates.h ${OPENSSL_LIBRARIES})
