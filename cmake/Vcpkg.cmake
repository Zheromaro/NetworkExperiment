# ==========================================================
# 📦 Vcpkg Integration (Project-local, inside build dir)
# ==========================================================

if(DEFINED PROJECT_NAME)
  message(FATAL_ERROR "Vcpkg.cmake must be included BEFORE the first project() call.")
endif()

option(ENABLE_VCPKG "Auto-download and enable local Vcpkg toolchain." OFF)

# If the user already passed a valid toolchain file on the command line, respect it.
if(ENABLE_VCPKG AND (NOT DEFINED CMAKE_TOOLCHAIN_FILE OR NOT EXISTS "${CMAKE_TOOLCHAIN_FILE}"))

  # vcpkg lives inside the build directory
  set(VCPKG_ROOT "${CMAKE_BINARY_DIR}/vcpkg" CACHE PATH "Local vcpkg directory")
  set(VCPKG_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "Vcpkg toolchain file")

  if(NOT EXISTS "${VCPKG_ROOT}")
    # --- 1. Clone vcpkg if missing ---
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}")

    message(STATUS "🌐 vcpkg not found. Cloning into ${VCPKG_ROOT}...")
    find_package(Git QUIET REQUIRED)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} clone "https://github.com/microsoft/vcpkg.git" "${VCPKG_ROOT}"
      RESULT_VARIABLE GIT_CLONE_RESULT
      OUTPUT_QUIET
    )
    if(NOT GIT_CLONE_RESULT EQUAL 0)
      message(FATAL_ERROR "❌ Failed to clone vcpkg. Please install Git or clone manually.")
    endif()
    message(STATUS "✅ vcpkg repository cloned.")
  endif()

  # --- 2. Bootstrap vcpkg executable if missing ---
  if(CMAKE_HOST_WIN32)
    set(VCPKG_EXEC "${VCPKG_ROOT}/vcpkg.exe")
    set(BOOTSTRAP_SCRIPT "${VCPKG_ROOT}/bootstrap-vcpkg.bat")
  else()
    set(VCPKG_EXEC "${VCPKG_ROOT}/vcpkg")
    set(BOOTSTRAP_SCRIPT "${VCPKG_ROOT}/bootstrap-vcpkg.sh")
  endif()

  if(NOT EXISTS "${VCPKG_EXEC}")
    message(STATUS "🔧 Bootstrapping vcpkg (this may take a minute)...")
    if(NOT EXISTS "${BOOTSTRAP_SCRIPT}")
      message(FATAL_ERROR "❌ Bootstrap script not found: ${BOOTSTRAP_SCRIPT}")
    endif()
    execute_process(
      COMMAND ${BOOTSTRAP_SCRIPT}
      WORKING_DIRECTORY "${VCPKG_ROOT}"
      RESULT_VARIABLE BOOTSTRAP_RESULT
    )
    if(NOT BOOTSTRAP_RESULT EQUAL 0)
      message(FATAL_ERROR "❌ vcpkg bootstrap failed.")
    endif()
    message(STATUS "✅ vcpkg bootstrapped successfully.")
  endif()

  # --- 3. Activate the local toolchain & manifest mode ---
  if(EXISTS "${VCPKG_TOOLCHAIN_FILE}")
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_TOOLCHAIN_FILE}" CACHE FILEPATH "Vcpkg toolchain" FORCE)
    set(VCPKG_MANIFEST_MODE ON CACHE BOOL "Vcpkg manifest mode" FORCE)
    message(STATUS "🔗 Local Vcpkg toolchain active: ${CMAKE_TOOLCHAIN_FILE}")
  else()
    message(FATAL_ERROR "❌ Vcpkg toolchain file missing: ${VCPKG_TOOLCHAIN_FILE}")
  endif()

endif()
