#
# Build TPL:  SuperLUDist
#   
# --- Define all the directories and common external project flags
# NOTEs: Conflict in header names with SuperLU is resolved by installing it first.
#        Only PETSc uses one enum from this header, so looks safe to override.
#        Besides that there is no real dependency on SuperLU here.
define_external_project_args(SuperLUDist
                             TARGET superlu_dist
                             DEPENDS METIS ParMetis SuperLU)

# Add version version to the autogenerated tpl_versions.h file
include(${SuperBuild_SOURCE_DIR}/TPLVersions.cmake)
amanzi_tpl_version_write(FILENAME ${TPL_VERSIONS_INCLUDE_FILE}
  PREFIX SuperLUDist
  VERSION ${SuperLUDist_VERSION_MAJOR} ${SuperLUDist_VERSION_MINOR} ${SuperLUDist_VERSION_PATCH})
  
# --- Patch the original code
#set(SuperLUDist_patch_file superludist-missingpatch)
#set(SuperLUDist_sh_patch ${SuperLUDist_prefix_dir}/superluidist-patch-step.sh)
#configure_file(${SuperBuild_TEMPLATE_FILES_DIR}/superludist-patch-step.sh.in
#               ${SuperLUDist_sh_patch}
#               @ONLY)
# configure the CMake patch step
#set(SuperLUDist_cmake_patch ${SuperLUDist_prefix_dir}/superluidist-patch-step.cmake)
#configure_file(${SuperBuild_TEMPLATE_FILES_DIR}/superludist-patch-step.cmake.in
#               ${SuperLUDist_cmake_patch}
#               @ONLY)
# set the patch command
#set(SuperLUDist_PATCH_COMMAND ${CMAKE_COMMAND} -P ${SuperLUDist_cmake_patch})

if(BUILD_SHARED_LIBS)
  set(SLU_BUILD_STATIC_LIBS FALSE)
else()
  set(SLU_BUILD_STATIC_LIBS TRUE)
endif()  

# --- Define the arguments passed to CMake.
set(SuperLUDist_CMAKE_ARGS 
      "-DCMAKE_INSTALL_PREFIX:FILEPATH=${TPL_INSTALL_PREFIX}"
      "-DCMAKE_INSTALL_LIBDIR:FILEPATH=${TPL_INSTALL_PREFIX}/lib"
      "-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}"
      "-DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}"
      "-DBUILD_STATIC_LIBS:BOOL=${SLU_BUILD_STATIC_LIBS}"
      "-DTPL_ENABLE_BLASLIB:BOOL=FALSE")

# --- Add external project build and tie to the SuperLU build target
ExternalProject_Add(${SuperLUDist_BUILD_TARGET}
                    DEPENDS   ${SuperLUDist_PACKAGE_DEPENDS}   # Package dependency target
                    TMP_DIR   ${SuperLUDist_tmp_dir}           # Temporary files directory
                    STAMP_DIR ${SuperLUDist_stamp_dir}         # Timestamp and log directory
                    # -- Download and URL definitions
                    DOWNLOAD_DIR  ${TPL_DOWNLOAD_DIR}          # Download directory
                    URL           ${SuperLUDist_URL}           # URL may be a web site OR a local file
                    URL_MD5       ${SuperLUDist_MD5_SUM}       # md5sum of the archive file
                    DOWNLOAD_NAME ${SuperLUDist_SAVEAS_FILE}   # file name to store (if not end of URL)
                    # -- Patch
                    # PATCH_COMMAND ${SuperLUDist_PATCH_COMMAND}  # Mods to source
                    # -- Configure
                    LIST_SEPARATOR |                             # Use the alternate list separator
                    SOURCE_DIR      ${SuperLUDist_source_dir}    # Source directory
                    CMAKE_ARGS      ${AMANZI_CMAKE_CACHE_ARGS}   # Ensure uniform build
                                    ${SuperLUDist_CMAKE_ARGS}
                                    -DCMAKE_C_FLAGS:STRING=${Amanzi_COMMON_CFLAGS}  # Ensure uniform build
                                    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
                                    -DCMAKE_CXX_FLAGS:STRING=${Amanzi_COMMON_CXXFLAGS}
                                    -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
                                    -DCMAKE_Fortran_COMPILER:FILEPATH=${CMAKE_Fortran_COMPILER}
                                    -DMPI_CXX_COMPILER:FILEPATH=${MPI_CXX_COMPILER}
                                    -DMPI_C_COMPILER:FILEPATH=${MPI_C_COMPILER}
                                    -DMPI_Fortran_COMPILER:FILEPATH=${MPI_Fortran_COMPILER}
                                    -DOpenMP_C_FLAGS:STRING=                        # Workaround to avoid OpenMP
                                    -DOpenMP_Fortran_FLAGS:STRING=
                    # -- CMake Cache
                    CMAKE_CACHE_ARGS -DCMAKE_MODULE_PATH:STRING=${superlu_module_opt}
                                     -DTPL_PARMETIS_INCLUDE_DIRS:STRING=${TPL_INSTALL_PREFIX}/include
                                     -DTPL_PARMETIS_LIBRARIES:STRING=${ParMetis_LIBRARIES}
                                     -DTPL_BLAS_LIBRARIES:STRING=${BLAS_LIBRARIES}
                                     -DTPL_LAPACK_LIBRARIES:STRING=${LAPACK_LIBRARIES}
                    # -- Build
                    BINARY_DIR      ${SuperLUDist_build_dir}   # Build directory 
                    BUILD_COMMAND   $(MAKE)
                    # -- Install
                    INSTALL_DIR     ${TPL_INSTALL_PREFIX}      # Install directory
                    INSTALL_COMMAND $(MAKE) install
                    # -- Output control
                    ${SuperLUDist_logging_args})

include(BuildLibraryName)
build_library_name(superlu_dist SuperLUDist_LIB APPEND_PATH ${TPL_INSTALL_PREFIX}/lib)

# --- set cache (global) variables
global_set(SuperLUDist_LIBRARY "${SuperLUDist_LIB}")
global_set(SuperLUDist_DIR ${TPL_INSTALL_PREFIX})
