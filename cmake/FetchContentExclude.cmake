include(FetchContent)

function(FetchContent_MakeAvailable_One dep exclude)
  message(STATUS "Fetching ${dep} (exclude: ${exclude})")
  FetchContent_GetProperties(${dep})
  if(NOT ${dep}_POPULATED)
    FetchContent_Populate(${dep})
    if(${dep} STREQUAL "spdlog" AND NOT DEFINED SPDLOG_PATCHED)
      # hack spdlog
      execute_process(COMMAND sed -i "s/^\\([^p].*mutex_;.*\\)$/public:\\1 private:/" ${${dep}_SOURCE_DIR}/include/spdlog/sinks/ansicolor_sink.h)
      message(STATUS "Patched spdlog")
      set(SPDLOG_PATCHED ON CACHE INTERNAL "")
    endif()
    if(exclude)
      add_subdirectory(${${dep}_SOURCE_DIR} ${${dep}_BINARY_DIR} EXCLUDE_FROM_ALL)
    else()
      add_subdirectory(${${dep}_SOURCE_DIR} ${${dep}_BINARY_DIR})
    endif()
  endif()
endfunction()

function (FetchContent_MakeAvailable_Exclude)
  foreach(dep IN LISTS ARGV)
    FetchContent_MakeAvailable_One(${dep} 1)
  endforeach()
endfunction()

function (FetchContent_MakeAvailable_Include)
  foreach(dep IN LISTS ARGV)
    FetchContent_MakeAvailable_One(${dep} 0)
  endforeach()
endfunction()