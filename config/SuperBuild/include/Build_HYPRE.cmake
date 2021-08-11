#  -*- mode: cmake -*-

#
# Build TPL:  HYPRE 
#    

# --- Define all the directories and common external project flags
define_external_project_args(HYPRE 
                             TARGET hypre
                             DEPENDS SuperLU SuperLUDist)

# add version version to the autogenerated tpl_versions.h file
amanzi_tpl_version_write(FILENAME ${TPL_VERSIONS_INCLUDE_FILE}
  PREFIX HYPRE
  VERSION ${HYPRE_VERSION_MAJOR} ${HYPRE_VERSION_MINOR} ${HYPRE_VERSION_PATCH})


# --- Define configure parameters

# Disable OpenMP with HYPRE for now
# Is OpenMP available
# if (ENABLE_OpenMP)
#   find_package(OpenMP)

if (ENABLE_KOKKOS_OPENMP)
  set(hypre_openmp_opt "--with-openmp" "--with-LDFLAGS=${OpenMP_C_LIBRARIES}")
else()
  set(hypre_openmp_opt)
endif()

# Locate LAPACK and BLAS
if (BLAS_FOUND)
  if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    set(hypre_blas_opt "--with-blas-lib=-framework Accelerate")
  else()
    set(hypre_blas_opt "--with-blas-lib=${BLAS_LIBRARIES}")
  endif()
else()
  set(hypre_blas_opt)
endif()

if (LAPACK_FOUND)
  if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    set(hypre_lapack_opt "--with-lapack-lib=-framework Accelerate")
  else()
    set(hypre_lapack_opt "--with-lapack-lib=${LAPACK_LIBRARIES}")
  endif()
else()
  set(hypre_lapack_opt)
endif()

if(ENABLE_KOKKOS_CUDA)
  find_package(CUDA REQUIRED)
  set(CUDA_HOME ${CUDA_TOOLKIT_ROOT_DIR})
  message(STATUS "CUDA_HOME: ${CUDA_HOME}")
  set(Hypre_CUDA_SM 70)
  set(hypre_kokkos_cuda "--with-cuda" "--enable-cusparse" "--enable-unified-memory")
else()
  set(CUDA_HOME)
  set(Hypre_CUDA_SM)
  set(hypre_kokkos_cuda)
endif()

# set(hypre_fortran_opt -"--disable-fortran)

set(Hypre_CC_FLAGS ${CMAKE_C_FLAGS})
set(Hypre_CXX_FLAGS ${CMAKE_CXX_FLAGS})
set(Hypre_LINK_FLAGS ${CMAKE_LINK_FLAGS})

# Locate SuperLU and SuperLUDist
set(hypre_superlu_opt "--with-superlu" 
                      "--with-superlu-include=${TPL_INSTALL_PREFIX}/include"
                      "--with-superlu-lib=${SuperLU_LIBRARY}"
                      "--with-dsuperlu"
                      "--with-dsuperlu-include=${TPL_INSTALL_PREFIX}/include"
                      "--with-dsuperlu-lib=${SuperLUDist_LIBRARY}")

# shared/static libraries (shared FEI is broken in HYPRE)
if (BUILD_SHARED_LIBS)
  set(hypre_shared_opt "--enable-shared")
else()
  set(hypre_shared_opt "--disable-shared")
endif()

# --- Set the name of the patch
set(HYPRE_patch_file hypre-superlu.patch)
# --- Configure the bash patch script
set(HYPRE_sh_patch ${HYPRE_prefix_dir}/hypre-patch-step.sh)
configure_file(${SuperBuild_TEMPLATE_FILES_DIR}/hypre-patch-step.sh.in
               ${HYPRE_sh_patch}
               @ONLY)
# --- Configure the CMake patch step
set(HYPRE_cmake_patch ${HYPRE_prefix_dir}/hypre-patch-step.cmake)
configure_file(${SuperBuild_TEMPLATE_FILES_DIR}/hypre-patch-step.cmake.in
               ${HYPRE_cmake_patch}
               @ONLY)
# --- Set the patch command
set(HYPRE_PATCH_COMMAND ${CMAKE_COMMAND} -P ${HYPRE_cmake_patch})     

# --- If downloads are disabled point to local repository
if ( DISABLE_EXTERNAL_DOWNLOAD )
  STRING(REGEX REPLACE ".*\/" "" HYPRE_GIT_REPOSITORY_LOCAL_DIR ${HYPRE_GIT_REPOSITORY})
  set (HYPRE_GIT_REPOSITORY_TEMP ${TPL_DOWNLOAD_DIR}/${HYPRE_GIT_REPOSITORY_LOCAL_DIR})
else()
  set (HYPRE_GIT_REPOSITORY_TEMP ${HYPRE_GIT_REPOSITORY})
endif()
message(STATUS "HYPRE git repository = ${HYPRE_GIT_REPOSITORY_TEMP}")

# --- Add external project build and tie to the ZLIB build target
ExternalProject_Add(${HYPRE_BUILD_TARGET}
                    DEPENDS   ${HYPRE_PACKAGE_DEPENDS}         # Package dependency target
                    TMP_DIR   ${HYPRE_tmp_dir}                 # Temporary files directory
                    STAMP_DIR ${HYPRE_stamp_dir}               # Timestamp and log directory
                    # -- Download and GIT definition
                    GIT_REPOSITORY ${HYPRE_GIT_REPOSITORY_TEMP}              
                    GIT_TAG        ${HYPRE_GIT_TAG}   
                    # -- Update (one way to skip this step is use null command)
                    UPDATE_COMMAND ""
                    # -- Patch 
                    PATCH_COMMAND  ${HYPRE_PATCH_COMMAND}
                    # -- Configure
                    SOURCE_DIR    ${HYPRE_source_dir}
                    SOURCE_SUBDIR src                          # cmake 3.7+ feature 
                    CONFIGURE_COMMAND
                       ${HYPRE_source_dir}/src/configure
                         --prefix=${TPL_INSTALL_PREFIX}
                         --with-MPI
                         ${hypre_shared_opt}
                         ${hypre_openmp_opt}
                         ${hypre_lapack_opt}
                         ${hypre_blas_opt}
                         ${hypre_superlu_opt}
                         ${hypre_kokkos_cuda}
                         CC=${CMAKE_C_COMPILER}
                         CFLAGS=${Hypre_CC_FLAGS}
                         CXX=${CMAKE_CXX_COMPILER}
                         CXXFLAGS=${Hypre_CXX_FLAGS}
                         LDFLAGS=${Hypre_LINK_FLAGS}
                         CUDA_HOME=${CUDA_HOME}
                         HYPRE_CUDA_SM=${Hypre_CUDA_SM}               
                    # -- Build
                    BINARY_DIR       ${HYPRE_source_dir}/src        # Build directory 
                    BUILD_COMMAND    $(MAKE)  
                    # -- Install
                    INSTALL_DIR      ${TPL_INSTALL_PREFIX}     # Install directory
                    INSTALL_COMMAND  $(MAKE) install
                    # -- Output control
                    ${HYPRE_logging_args})


# --- Useful variables that depend on HYPRE
include(BuildLibraryName)
build_library_name(HYPRE HYPRE_LIBRARY APPEND_PATH ${TPL_INSTALL_PREFIX}/lib)
set(HYPRE_INSTALL_PREFIX ${TPL_INSTALL_PREFIX})
