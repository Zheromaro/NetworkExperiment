# ==========================================================
# 📁 Source & Header Files
# ==========================================================

set(SRC_DIR     "${PROJECT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(TEST_DIR    "${PROJECT_SOURCE_DIR}/test")
set(BIN_DIR     "${SRC_DIR}/bin")

# ---- All source files (including main & bin/) ----
file(GLOB_RECURSE all_sources
    CONFIGURE_DEPENDS
    "${SRC_DIR}/*.c"
)

# ---- Binary entry points for multiple executables (src/bin/*.c) ----
file(GLOB_RECURSE bin_sources
    CONFIGURE_DEPENDS
    "${BIN_DIR}/*.c"
)

# Remove bin/ sources from all_sources to get shared implementation
set(sources ${all_sources})
if(bin_sources)
  list(REMOVE_ITEM sources ${bin_sources})
endif()

# exe_sources = everything (when no src/bin/ exists)
set(exe_sources ${all_sources})

# Exclude main.c AND all src/bin/*.c files
list(FILTER sources EXCLUDE REGEX ".*/main\\.c$")

# ---- Headers ----
file(GLOB_RECURSE headers
    CONFIGURE_DEPENDS
    "${INCLUDE_DIR}/*.h"
)

# ---- Test directories ----
file(GLOB TEST_ENTRIES
    LIST_DIRECTORIES TRUE
    RELATIVE "${TEST_DIR}"
    "${TEST_DIR}/*"
)

set(test_dirs "")
foreach(entry IN LISTS TEST_ENTRIES)
  if(IS_DIRECTORY "${TEST_DIR}/${entry}")
    list(APPEND test_dirs "${TEST_DIR}/${entry}")
  endif()
endforeach()

# ---- Test helper sources (C utilities in test/) ----
file(GLOB_RECURSE test_helper_sources
    CONFIGURE_DEPENDS
    "${TEST_DIR}/*.c"
)
