# Install script for directory: /home/fallen/Desktop/CSE-322/ns-3-dev/src/core

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-core-debug.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-core-debug.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-core-debug.so"
         RPATH "/usr/local/lib:$ORIGIN/:$ORIGIN/../lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/fallen/Desktop/CSE-322/ns-3-dev/build/lib/libns3-dev-core-debug.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-core-debug.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-core-debug.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-core-debug.so"
         OLD_RPATH "/home/fallen/Desktop/CSE-322/ns-3-dev/build/lib:"
         NEW_RPATH "/usr/local/lib:$ORIGIN/:$ORIGIN/../lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-core-debug.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/int64x64-128.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/helper/csv-reader.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/helper/event-garbage-collector.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/helper/random-variable-stream-helper.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/abort.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/ascii-file.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/ascii-test.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/assert.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/attribute-accessor-helper.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/attribute-construction-list.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/attribute-container.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/attribute-helper.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/attribute.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/boolean.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/breakpoint.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/build-profile.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/calendar-scheduler.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/callback.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/command-line.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/config.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/default-deleter.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/default-simulator-impl.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/deprecated.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/des-metrics.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/double.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/enum.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/event-id.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/event-impl.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/fatal-error.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/fatal-impl.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/fd-reader.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/environment-variable.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/global-value.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/hash-fnv.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/hash-function.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/hash-murmur3.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/hash.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/heap-scheduler.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/int-to-type.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/int64x64-double.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/int64x64.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/integer.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/length.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/list-scheduler.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/log-macros-disabled.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/log-macros-enabled.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/log.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/make-event.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/map-scheduler.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/math.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/names.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/node-printer.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/nstime.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/object-base.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/object-factory.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/object-map.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/object-ptr-container.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/object-vector.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/object.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/pair.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/pointer.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/priority-queue-scheduler.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/ptr.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/random-variable-stream.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/rng-seed-manager.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/rng-stream.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/scheduler.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/show-progress.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/simple-ref-count.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/simulation-singleton.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/simulator-impl.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/simulator.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/singleton.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/string.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/synchronizer.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/system-path.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/system-wall-clock-ms.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/system-wall-clock-timestamp.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/test.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/time-printer.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/timer-impl.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/timer.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/trace-source-accessor.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/traced-callback.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/traced-value.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/trickle-timer.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/tuple.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/type-id.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/type-name.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/type-traits.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/uinteger.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/unused.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/valgrind.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/vector.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/warnings.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/watchdog.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/realtime-simulator-impl.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/wall-clock-synchronizer.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/val-array.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/src/core/model/matrix-array.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/build/include/ns3/config-store-config.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/build/include/ns3/core-config.h"
    "/home/fallen/Desktop/CSE-322/ns-3-dev/build/include/ns3/core-module.h"
    )
endif()

