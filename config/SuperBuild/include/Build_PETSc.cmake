#  -*- mode: cmake -*-

#
# Build TPL:  PETSc 
#    
# --- Define all the directories and common external project flags
define_external_project_args(PETSc TARGET petsc DEPENDS ${MPI_PROJECT} METIS ParMetis HYPRE BUILD_IN_SOURCE)

# add PETSc version to the autogenerated tpl_versions.h file
amanzi_tpl_version_write(FILENAME ${TPL_VERSIONS_INCLUDE_FILE}
                         PREFIX PETSc
                         VERSION ${PETSc_VERSION_MAJOR} ${PETSc_VERSION_MINOR} ${PETSc_VERSION_PATCH})
  
# --- Download packages PETSc needs
set(petsc_packages SuperLU SuperLUDist)
get_filename_component(real_download_path ${TPL_DOWNLOAD_DIR} REALPATH)

message(STATUS ">>> Build_PETSc -- Checking PETSc required packages: ${petsc_packages}")
foreach (_pack ${petsc_packages})

  message(STATUS ">>> Build_PETSc -- package: ${_pack}")

  set(_url     "${${_pack}_URL_STRING}")
  set(_archive "${${_pack}_ARCHIVE_FILE}")
  set(_saveas  "${${_pack}_SAVEAS_FILE}")
  set(_md5sum  "${${_pack}_MD5_SUM}")

  message(STATUS ">>> Build_PETSc -- URL: ${_url}")
  message(STATUS ">>> Build_PETSc -- ARCHIVE_FILE: ${_archive}")
  message(STATUS ">>> Build_PETSc -- SAVEAS_FILE: ${_saveas}")
  message(STATUS ">>> Build_PETSc -- MD5SUM: ${_md5sum}")

  if (EXISTS "${real_download_path}/${_archive}")
    message(STATUS "\tFound ${_archive}")
  else()
    if (DISABLE_EXTERNAL_DOWNLOAD)
      message(FATAL_ERROR "You have disabled external downloads, however"
                          " ${real_download_path}/${_archive} does not exist")
    else()
      message(STATUS ">>> Build_PETSc -- Downloading ${_archive} for PETSc")
      file(DOWNLOAD "${_url}/${_archive}" "${real_download_path}/${_saveas}"
                    INACTIVITY_TIMEOUT 180
                    EXPECTED_MD5 ${_md5sum}
                    STATUS ${_pack}_DOWNLOAD_STATUS)
      list(GET ${_pack}_DOWNLOAD_STATUS 0 _status)
      if (${_status} EQUAL 0)
        message(STATUS ">>> Build_PETSc -- DOWNLOAD: successful!")
      else()
        message(FATAL_ERROR ">>> Build_PETSc -- DOWNLOAD: failed -- ${${_pack}_DOWNLOAD_STATUS}[1]")
      endif()
    endif()
  endif()  
endforeach()


# --- Define configure parameters

# Use the common cflags, cxxflags
include(BuildWhitespaceString)
build_whitespace_string(petsc_cflags
                       ${Amanzi_COMMON_CFLAGS})

build_whitespace_string(petsc_cxxflags
                       ${Amanzi_COMMON_CXXFLAGS})
set(cpp_flag_list 
    ${Amanzi_COMMON_CFLAGS}
    ${Amanzi_COMMON_CXXFLAGS})
list(REMOVE_DUPLICATES cpp_flag_list)
build_whitespace_string(petsc_cppflags ${cpp_flags_list})

build_whitespace_string(petsc_fcflags
                       ${Amanzi_COMMON_FCFLAGS})

# Set PETSc debug flag
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" )
  set(petsc_debug_flag 0)
else()
  set(petsc_debug_flag 1)
endif()

# Point PETSc to the MPI build
if ( "${${MPI_PROJECT}_BUILD_TARGET}" STREQUAL "" )
  set(petsc_mpi_flags --with-mpi=1)
else()
  set(petsc_mpi_flags --with-mpi=1 --with-mpi-dir=${TPL_INSTALL_PREFIX})
endif()

# BLAS options
if (BLAS_LIBRARIES) 
  if (NOT APPLE) # Macs are different.
    build_whitespace_string(petsc_blas_libs ${BLAS_LIBRARIES})
    set(petsc_blas_option --with-blas-libs='${petsc_blas_libs}')
  endif()
else()
  set(petsc_blas_option)
endif()

# LAPACK options
if ( LAPACK_LIBRARIES ) 
  if (NOT APPLE) # Macs are different.
    build_whitespace_string(petsc_lapack_libs ${LAPACK_LIBRARIES})
    set(petsc_lapack_option --with-lapack-libs='${petsc_lapack_libs}')
  endif()
else()
  set(petsc_lapack_option)
endif()

# Point PETSc to the metis build

# PETSc SuperLU, Metis, ParMetis and SuperLU_dist flags
# For now we allow PETSc to download and build SuperLU and SuperLUDist
# Perhaps they should be a separate TPLs. Error with the download or
# building of these packages will appear to be an error in the
# petsc-configure target. See the log files for more detailed
# information.
set(petsc_superlu_flags --download-superlu=${real_download_path}/${SuperLU_SAVEAS_FILE})
set(petsc_superlu_dist_flags --download-superlu_dist=${real_download_path}/${SuperLUDist_SAVEAS_FILE})
set(petsc_metis_flags --with-metis=1 --with-metis-dir=${TPL_INSTALL_PREFIX})
set(petsc_parmetis_flags --with-parmetis=1 --with-parmetis-dir=${TPL_INSTALL_PREFIX})
set(petsc_hypre_flags --with-hypre=1 --with-hypre-dir=${TPL_INSTALL_PREFIX})

set(petsc_package_flags ${petsc_superlu_flags} ${petsc_parmetis_flags} ${petsc_superlu_dist_flags} ${petsc_metis_flags} ${petsc_hypre_flags})

# PETSc install directory
set(petsc_install_dir ${TPL_INSTALL_PREFIX}/${PETSc_BUILD_TARGET}-${PETSc_VERSION})

if (BUILD_SHARED_LIBS)
  set(CONFIG_PETSC_SHARED --with-shared-libraries=1)
else()
  set(CONFIG_PETSC_SHARED --with-shared-libraries=0)
endif()

# --- Set the name of the patch
#set(PETSC_patch_file petsc-3.5.2-mat-m0.patch petsc-3.5.2-install.patch)
# --- Configure the bash patch script
#set(PETSC_sh_patch ${PETSc_prefix_dir}/petsc-patch-step.sh)
#message(STATUS "PETSC_sh_patch: ${PETSC_sh_patch}")
#configure_file(${SuperBuild_TEMPLATE_FILES_DIR}/petsc-patch-step.sh.in
#               ${PETSC_sh_patch}
#               @ONLY)
# --- Configure the CMake patch step
#set(PETSC_cmake_patch ${PETSc_prefix_dir}/petsc-patch-step.cmake)
#configure_file(${SuperBuild_TEMPLATE_FILES_DIR}/petsc-patch-step.cmake.in
#               ${PETSC_cmake_patch}
#               @ONLY)
# --- Set the patch command
#set(PETSC_PATCH_COMMAND ${CMAKE_COMMAND} -P ${PETSC_cmake_patch})     

# --- Add external project build 
ExternalProject_Add(${PETSc_BUILD_TARGET}
                    DEPENDS   ${PETSc_PACKAGE_DEPENDS}             # Package dependency target
                    TMP_DIR   ${PETSc_tmp_dir}                     # Temporary files directory
                    STAMP_DIR ${PETSc_stamp_dir}                   # Timestamp and log directory
                    # -- Download and URL definitions
                    DOWNLOAD_DIR ${TPL_DOWNLOAD_DIR}               # Download directory
                    URL          ${PETSc_URL}                      # URL may be a web site OR a local file
                    URL_MD5      ${PETSc_MD5_SUM}                  # md5sum of the archive file
                    DOWNLOAD_NAME ${PETSc_SAVEAS_FILE}             # file name to store (if not end of URL)
                    # -- Patch 
                    # PATCH_COMMAND ${PETSC_PATCH_COMMAND}
                    # -- Configure
                    SOURCE_DIR        ${PETSc_source_dir}          # Source directory
                    CONFIGURE_COMMAND
                              <SOURCE_DIR>/configure
                                          ${CONFIG_PETSC_SHARED}
                                          --prefix=<INSTALL_DIR>
                                          --with-cc=${CMAKE_C_COMPILER_USE}
                                          --with-cxx=${CMAKE_CXX_COMPILER_USE}
                                          --with-fc=${CMAKE_Fortran_COMPILER_USE}
                                          --CFLAGS=${petsc_cflags}
                                          --CXXFLAGS=${petsc_cxxflags}
                                          --without-x
                                          --with-debugging=${petsc_debug_flag}
                                          --without-valgring
                                          ${petsc_mpi_flags}
                                          ${petsc_lapack_option}
                                          ${petsc_blas_option}
                                          ${petsc_package_flags}
                    # -- Build
                    BINARY_DIR       ${PETSc_build_dir}           # Build directory 
                    BUILD_COMMAND    $(MAKE) -j 1 PETSC_DIR=${PETSc_source_dir} # Run the CMake script to build
                    BUILD_IN_SOURCE  ${PETSc_BUILD_IN_SOURCE}     # Flag for in source builds
                    # -- Install
                    INSTALL_DIR      ${petsc_install_dir}         # Install directory, NOT in the usual directory
                    INSTALL_COMMAND  $(MAKE) install PETSC_DIR=${PETSc_source_dir}
                    # -- Output control
                    ${PETSc_logging_args})

# --- Useful variables for other packages that depend on PETSc
set(PETSC_DIR ${petsc_install_dir})
