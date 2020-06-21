#  -*- mode: cmake -*-

#
# TPLVersions
#
#    Define the versions, approved download locations for each TPL
#

#
# TPL: Amanzi Collection of TPLs
#
#   Define a "version number" for the collection of TPLs listed here.
#   It's not clear this is the best way to include this information, 
#   but it's a reasonable place to start.
#   
#   Upgrade History:
#
#   0.90.6       - first version reference used in installations
#   0.90.7       - updated MSTK to version 2.01
#                - added NETCDF - fortran version 4.2 (interface library)
#   0.90.8       - added Xerces-C++ version 3.1.1 (XML support)
#   0.90.9       - update MSTK to version 2.02
#
#   0.91.0       - added PFLOTRAN and Alquimia (updates from Jeff Johnson's work on state-branch)
#   0.91.1       - updated CCSE to version 1.1.7 (and added patch)
#   0.91.2       - turned on patch for IfPack support of noncontiguous global ids with HYPRE
#   0.91.3       - updated Xerces-C++ build/find to use OSX CoreServices framework
#   0.91.4       - updated CCSE to version 1.1.8
#   0.91.5       - updated CCSE to version 1.2.1
#   0.91.6       - updated CCSE to version 1.2.3
#   0.91.7       - updated MSTK to version 2.10rc3
#   0.91.8       - updated Alquimia to licensed version 0.1
#   0.91.9       - updated CCSE to version 1.2.4
#   0.91.10      - updated MSTK to version 2.10rc5
#   0.91.11      - updated PFlotran to commit 1afe88d.
#   0.91.12      - updated MSTK to version 2.10
#   0.91.12a     - updated METIS to 5.1.0, ParMetis to 4.0.3a, SuperLU to 4.3, SuperLUDist to 3.3 and PETSc to 3.4.3
#   0.91.12b     - updated Trilinos to 10.6.1 and MSTK to 2.11rc2
#   0.91.12c     - updated CCSE to version 1.2.5
#   0.91.13      - updated MSTK to version 2.11rc3
#   0.91.14      - updated MSTK to version 2.11rc4 (fixes memory leaks)
#   0.91.15      - updated MSTK to version 2.11rc5 (fixes memory leaks)
#
#   0.92.0       - Merge lib updates through 0.91.15
#   0.92.1       - update MSTK to version 2.12 (fixes debug version linking)
#   0.92.2       - update CCSE to version 1.2.7 (adds f90 utility for plotting)
#   0.92.3       - update CURL to version 7.37.0 (builds correctly on Mac OS X 10.9)
#   0.92.4       - update NetCDF to version 4.3.2 (builds correctly on Mac OS X 10.9)
#   0.92.5       - Patched Alquimia to build properly with GFortran 4.9.x
#   0.92.6       - update Boost to version 1.56.0 
#   0.92.7       - update CCSE to version 1.2.8 
#   0.92.8       - update ExodussII 5.22 -> 6.06
#   0.92.9       - update MSTK to v 2.21 (incompatible -DWITH_MSTK_2_21rc1_OR_NEWER=TRUE)
#   0.92.10      - update MSTK to v 2.22rc1
#   0.92.11      - update MSTK to v 2.22rc3 (fixed parallel mesh partitioning bug)
#   0.92.12      - update PETSc to 3.5.2, Alquimia to 0.2, and PFlotran to commit 611092f80ddb.
#   0.92.13      - update MSTK to v2.22, includes installation of mesh utilities
#   0.92.14      - update Hypre to v2.10.0b (and added patch for to ensure tol>0)
#   0.92.15      - updated Alquimia to v0.2 (backward compatible)
#   0.92.16      - update CCSE to version 1.3.0 
#   0.92.17      - update MSTK to version 2.23 (adds element set capabilities)
#   0.92.18      - update Boost to version 1.58.0 
#   0.92.19      - update CCSE to version 1.3.2
#   0.92.20      - update CCSE to version 1.3.4 (fix issue with fsnapshot)
#   0.92.21      - update Alquimia to version 0.3.1 (CrunchFlow integration)
#   0.92.22      - added optional Silo package
#   0.92.23      - Patched ASCEM-IO to allocate space for sprintf() correctly.
#   0.92.24      - update MSTK to version 2.25 (updates to meshconvert, exoatt)
#   0.92.25      - update MSTK to version 2.26rc2 (adds fixes for pinchouts)
#
#   0.93.0       - defaulted to C++11, update Trilinos to 12.6.1
#   0.93.1       - update Boost to version 1.61.0
#   0.93.2       - update Alquimia to version 1.0.3
#   0.93.3       - update Alquimia to version 1.0.4

#   0.94.1       - updates several TPLs, new versions are:
#                - Trilinos 12.10.1
#                - zlib 1.2.11
#                - hdf5 1.8.12
#                - netcdf 4.4.1.1
#                - netcdf-fortran 4.4.4
#                - boost 1.6.3
#   0.94.2       - update MTSK to version 2.28rc1
#   0.94.3       - update MSTK to version 3.00 (incompatible - need to update #defines)
#   0.94.4       - update MSTK to version 3.01
#   0.94.5       - restored Alquimia to version 1.0.4
#   0.94.6       - Added CrunchTope package, hash version c31ecb9
#   0.94.7       - update UnitTest++ to version 2.0.0
#                - update Hypre to version 2.11.2    
#   0.94.8       - removed ExodusII as independent TPL  
#   0.94.9       - update PFloTran to version dev-c8df814cb6fa
#                - update PETSc to xsdk-0.2.0 (native 3.7.5)
#                - update SuperLU to 5.2.1
#                - update SuperLU_dist to xsdk-0.2.0 (native 5.1.3)
#                - update Alquimia to xsdk-0.2.0 (native 1.0.4)
#                - update Hypre to xsdk-0.2.0 (native 2.11.2)
#   0.94.10      - update Alquimia to 1.0.5 (critical bug fixes)
#   0.94.11      - update MSTK to 3.0.3 (critical bug fixes)
#                - update NetCDF to 4.5.0
#	         - update NetCDF-Fortran to 4.4.4
#	         - update curl to 7.56.1
#   0.94.12      - update xerces-c to 3.2.0 (CMake build)

#   0.95.0       - update Trilinos 12.12.1
#                - update Hypre 2.12.1  
#                - note alquimia 1.0.5 == xsdk-0.3.0
#                - pflotran release/xsdk-0.3.0
#                - update PETSc 3.8.2
#   0.95.1       - update OpenMPI to 3.0.1
#   0.95.2       - added Amesos2 to list of built Trilinos packages
#                - update MSTK to 3.0.4
#   0.95.3       - removed CURL
#                - update HYPRE to 2.14.0
#                - update OpenMPI to 3.0.2
#   0.95.4       - update Boost to version 1.67.0
#   0.95.5       - update NetCDF to version 4.6.1
#                - update OpenMPI to 3.1.1
#   0.95.6       - added new package nanoflann 1.3.0
#   0.95.7       - update MSTK to 3.1.0
#   0.95.8       - update NetCDF-Fortran to 4.4.5
#   0.95.9       - update NetCDF-C to 4.7.0
#   0.95.10      - update OpenMPI to 3.1.4
#
#   0.96.0       - update Trilinos to 12.14.0bf557717e-Jun17
#   0.96.1       - update NetCDF-C to 4.7.1 
#                - update MOAB to 5.1.0
#   0.96.2       - update SuperLU_Dist to 3.4.0
#                - update PETSc to 3.11.3
#                - update PFloTran to b681066 (Sep 20, 2019)
#                - update Alquimia to 1.0.6-rc
#   0.96.3       - update HDF5 to 1.10.5
#   0.96.4       - update MSTK to 3.2.1
#   0.97.0       - update HDF5 to 1.10.6
#                - update Nanoflann to 1.3.1
#                - update Trilinos to 55a7599733 (Nov11, 2019, also 12.18.1)
#                - update NetCDF-C to 4.7.3
#                - update NetCDF-Fortran to 4.5.2
#   0.97.1       - update MSTK to 3.3.1
#   0.97.2       - update MSTK to 3.3.5
#   0.97.3       - update CrunchTope to hash version 906e164 (Apr 2, 2020)
#   0.97.4       - added CLM
#   0.97.5       - added exprtk
#   0.97.6       - update BoxLib (CCSE) to version 1.3.5
#   0.97.7       - update BoxLib (CCSE) to version 16.09

include(CMakeParseArguments)

MACRO(LIST_LENGTH var)
  SET(entries)
  FOREACH(e ${ARGN})
    SET(entries "${entries}.")
  ENDFOREACH(e)
  STRING(LENGTH "${entries}" ${var})
ENDMACRO(LIST_LENGTH)

# this macro appends version number defines to the tpl_versions.h include file
macro(amanzi_tpl_version_write)
  set(singleValueArgs FILENAME PREFIX)
  set(multiValueArgs VERSION)
  set(options "")

  cmake_parse_arguments(LOCAL "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

  list_length(length ${LOCAL_VERSION})

  if (length GREATER 0) 
    list(GET LOCAL_VERSION 0 MAJOR)
    file(APPEND ${LOCAL_FILENAME} "#define ${LOCAL_PREFIX}_MAJOR ${MAJOR}\n")
  else()
    file(APPEND ${LOCAL_FILENAME} "#define ${LOCAL_PREFIX}_MAJOR\n")
  endif()

  if (length GREATER 1)
    list(GET LOCAL_VERSION 1 MINOR)
    file(APPEND ${LOCAL_FILENAME} "#define ${LOCAL_PREFIX}_MINOR ${MINOR}\n")
  else()
    file(APPEND ${LOCAL_FILENAME} "#define ${LOCAL_PREFIX}_MINOR\n")
  endif()

  if (length GREATER 2)
    list(GET LOCAL_VERSION 2 PATCH)
    file(APPEND ${LOCAL_FILENAME} "#define ${LOCAL_PREFIX}_PATCH ${PATCH}\n")
  else()
    file(APPEND ${LOCAL_FILENAME} "#define ${LOCAL_PREFIX}_PATCH\n")
  endif()

  file(APPEND ${LOCAL_FILENAME} "\n")

endmacro(amanzi_tpl_version_write)


#
# TPLs and XSDK versions 
#
set(AMANZI_TPLS_VERSION_MAJOR 0)
set(AMANZI_TPLS_VERSION_MINOR 97)
set(AMANZI_TPLS_VERSION_PATCH 7)
set(AMANZI_TPLS_VERSION ${AMANZI_TPLS_VERSION_MAJOR}.${AMANZI_TPLS_VERSION_MINOR}.${AMANZI_TPLS_VERSION_PATCH})
# Not sure how to create a meaningful hash key for the collection

set(XSDK_VERSION "0.3.0")

#
# Default location on GitHub
#
set (AMANZI_TPLS_DOWNLOAD_URL "https://raw.githubusercontent.com/amanzi/amanzi-tpls/master/src")

#
# TPL: Xerces
#
set(XERCES_VERSION_MAJOR 3)
set(XERCES_VERSION_MINOR 2)
set(XERCES_VERSION_PATCH 0)
set(XERCES_VERSION ${XERCES_VERSION_MAJOR}.${XERCES_VERSION_MINOR}.${XERCES_VERSION_PATCH})
set(XERCES_URL_STRING     ${AMANZI_TPLS_DOWNLOAD_URL})
set(XERCES_ARCHIVE_FILE   xerces-c-${XERCES_VERSION}.tar.bz2)
set(XERCES_SAVEAS_FILE    ${XERCES_ARCHIVE_FILE})
set(XERCES_MD5_SUM        7b1394c32abbdd87841d94a8404c2ac6) 

#
# TPL: OpenMPI
#
set(OpenMPI_VERSION_MAJOR 3)
set(OpenMPI_VERSION_MINOR 1)
set(OpenMPI_VERSION_PATCH 4)
set(OpenMPI_VERSION ${OpenMPI_VERSION_MAJOR}.${OpenMPI_VERSION_MINOR}.${OpenMPI_VERSION_PATCH})
set(OpenMPI_URL_STRING     "https://www.open-mpi.org/software/ompi/v3.1/downloads/")
set(OpenMPI_ARCHIVE_FILE   openmpi-${OpenMPI_VERSION}.tar.bz2)
set(OpenMPI_SAVEAS_FILE    ${OpenMPI_ARCHIVE_FILE})
set(OpenMPI_MD5_SUM        29770c6efcbb4c41d820cb6b8eae90b8)

#
# TPL: MPICH
#
set(MPICH_VERSION_MAJOR 3)
set(MPICH_VERSION_MINOR 2)
set(MPICH_VERSION_PATCH 1)
set(MPICH_VERSION ${MPICH_VERSION_MAJOR}.${MPICH_VERSION_MINOR}.${MPICH_VERSION_PATCH})
set(MPICH_URL_STRING     "https://www.mpich.org/static/downloads/${MPICH_VERSION}/")
set(MPICH_ARCHIVE_FILE   mpich-${MPICH_VERSION}.tar.gz)
set(MPICH_SAVEAS_FILE    ${MPICH_ARCHIVE_FILE})
set(MPICH_MD5_SUM        e175452f4d61646a52c73031683fc375)

#
# TPL: zlib
#
set(ZLIB_VERSION_MAJOR 1)
set(ZLIB_VERSION_MINOR 2)
set(ZLIB_VERSION_PATCH 11)
set(ZLIB_VERSION ${ZLIB_VERSION_MAJOR}.${ZLIB_VERSION_MINOR}.${ZLIB_VERSION_PATCH})
set(ZLIB_URL_STRING     ${AMANZI_TPLS_DOWNLOAD_URL})
set(ZLIB_ARCHIVE_FILE   zlib-${ZLIB_VERSION}.tar.gz)
set(ZLIB_SAVEAS_FILE    ${ZLIB_ARCHIVE_FILE})
set(ZLIB_MD5_SUM        1c9f62f0778697a09d36121ead88e08e) 

#
# TPL: METIS
#
set(METIS_VERSION_MAJOR 5)
set(METIS_VERSION_MINOR 1)
set(METIS_VERSION_PATCH 0)
set(METIS_VERSION ${METIS_VERSION_MAJOR}.${METIS_VERSION_MINOR}.${METIS_VERSION_PATCH})
set(METIS_URL_STRING     ${AMANZI_TPLS_DOWNLOAD_URL})
set(METIS_ARCHIVE_FILE   metis-${METIS_VERSION}.tar.gz)
set(METIS_SAVEAS_FILE    ${METIS_ARCHIVE_FILE})
set(METIS_MD5_SUM        5465e67079419a69e0116de24fce58fe)

#
# TPL: CCSE
#
set(CCSE_VERSION_MAJOR 16)
set(CCSE_VERSION_MINOR 09)
set(CCSE_VERSION_PATCH 0)
set(CCSE_VERSION ${CCSE_VERSION_MAJOR}.${CCSE_VERSION_MINOR})
set(AMANZI_DIR $ENV{AMANZI_DIR})
set(CCSE_URL_STRING     "https://github.com/BoxLib-Codes/BoxLib/archive/")
set(CCSE_ARCHIVE_FILE   ${CCSE_VERSION}.tar.gz) 
set(CCSE_SAVEAS_FILE    ccse-${CCSE_VERSION}.tar.gz)
set(CCSE_MD5_SUM        3c26c38bfd06d2e37b22e31a9eacbf2f)
set(CCSE_GIT_REPOSITORY "https://github.com/BoxLib-Codes/BoxLib")
# set(CCSE_GIT_TAG        "3c29d664ae1b9d23d2d570c18428b3e16b54a136") 16.09
# set(CCSE_GIT_TAG        "89acb8e008f1e833efb7fb4638972fae238afd91") Ok
# set(CCSE_GIT_TAG        "81128c082a995cdbb380c97a192818f5caf708d5") No
set(CCSE_GIT_TAG        "b1dbcdd79c5bad0b80329e9d1a937823e36ae9bd")

#
# TPL: UnitTest
#
set(UnitTest_VERSION_MAJOR 2)
set(UnitTest_VERSION_MINOR 0)
set(UnitTest_VERSION_PATCH 0)
set(UnitTest_VERSION ${UnitTest_VERSION_MAJOR}.${UnitTest_VERSION_MINOR}.${UnitTest_VERSION_PATCH})
set(UnitTest_URL_STRING     ${AMANZI_TPLS_DOWNLOAD_URL})
set(UnitTest_ARCHIVE_FILE   unittest-cpp-${UnitTest_VERSION}.tgz)
set(UnitTest_SAVEAS_FILE    ${UnitTest_ARCHIVE_FILE})
set(UnitTest_MD5_SUM        29f958e355e516e7ab016b467974728d) 

#
# TPL: Boost
#
set(Boost_VERSION_MAJOR 1)
set(Boost_VERSION_MINOR 67)

set(Boost_VERSION_PATCH 0)
set(Boost_VERSION        ${Boost_VERSION_MAJOR}.${Boost_VERSION_MINOR}.${Boost_VERSION_PATCH})
set(Boost_VERSION_STRING ${Boost_VERSION_MAJOR}_${Boost_VERSION_MINOR}_${Boost_VERSION_PATCH})
set(Boost_URL_STRING     ${AMANZI_TPLS_DOWNLOAD_URL})
set(Boost_ARCHIVE_FILE   boost_${Boost_VERSION_STRING}.tar.bz2)
set(Boost_SAVEAS_FILE    ${Boost_ARCHIVE_FILE})
set(Boost_MD5_SUM        ced776cb19428ab8488774e1415535ab)

#
# TPL: HDF5
#
set(HDF5_VERSION_MAJOR 1)
set(HDF5_VERSION_MINOR 10)
set(HDF5_VERSION_PATCH 6)
set(HDF5_VERSION ${HDF5_VERSION_MAJOR}.${HDF5_VERSION_MINOR}.${HDF5_VERSION_PATCH})
set(HDF5_URL_STRING     ${AMANZI_TPLS_DOWNLOAD_URL})
set(HDF5_ARCHIVE_FILE   hdf5-${HDF5_VERSION}.tar.gz)
set(HDF5_SAVEAS_FILE    ${HDF5_ARCHIVE_FILE})
set(HDF5_MD5_SUM        37f3089e7487daf0890baf3d3328e54a)


#
# TPL: NetCDF
#
set(NetCDF_VERSION_MAJOR 4)
set(NetCDF_VERSION_MINOR 7)
set(NetCDF_VERSION_PATCH 3)
set(NetCDF_VERSION ${NetCDF_VERSION_MAJOR}.${NetCDF_VERSION_MINOR}.${NetCDF_VERSION_PATCH})
set(NetCDF_URL_STRING     "https://github.com/Unidata/netcdf-c/archive/")
set(NetCDF_ARCHIVE_FILE   v${NetCDF_VERSION}.tar.gz)
set(NetCDF_SAVEAS_FILE    netcdf-${NetCDF_VERSION}.tar.gz)
set(NetCDF_MD5_SUM        5ebcafd3fee1d48559687b5ace414fe8)


#
# TPL: NetCDF Fortran
#
set(NetCDF_Fortran_VERSION_MAJOR 4)
set(NetCDF_Fortran_VERSION_MINOR 5)
set(NetCDF_Fortran_VERSION_PATCH 2)
set(NetCDF_Fortran_VERSION ${NetCDF_Fortran_VERSION_MAJOR}.${NetCDF_Fortran_VERSION_MINOR}.${NetCDF_Fortran_VERSION_PATCH})
set(NetCDF_Fortran_URL_STRING     "https://github.com/Unidata/netcdf-fortran/archive/")
set(NetCDF_Fortran_ARCHIVE_FILE   v${NetCDF_Fortran_VERSION}.tar.gz)
set(NetCDF_Fortran_SAVEAS_FILE    netcdf-fortran-${NetCDF_Fortran_VERSION}.tar.gz)
set(NetCDF_Fortran_MD5_SUM        759402703585d12f84667716887c42df) 

#
# ASCEM-IO
#
set(ASCEMIO_VERSION_MAJOR 2)
set(ASCEMIO_VERSION_MINOR 2)
set(ASCEMIO_VERSION ${ASCEMIO_VERSION_MAJOR}.${ASCEMIO_VERSION_MINOR})
set(ASCEMIO_URL_STRING     ${AMANZI_TPLS_DOWNLOAD_URL})
set(ASCEMIO_ARCHIVE_FILE   ascem-io-${ASCEMIO_VERSION}.tar.gz)
set(ASCEMIO_SAVEAS_FILE    ${ASCEMIO_ARCHIVE_FILE})
set(ASCEMIO_MD5_SUM        869820bacd4c289c8f320be58c1449a7)

#
# TPL: MSTK
#
set(MSTK_VERSION_MAJOR 3)
set(MSTK_VERSION_MINOR 3)
set(MSTK_VERSION_PATCH 5)

set(MSTK_VERSION ${MSTK_VERSION_MAJOR}.${MSTK_VERSION_MINOR}.${MSTK_VERSION_PATCH})
set(MSTK_URL_STRING     "https://github.com/MeshToolkit/MSTK/archive")
set(MSTK_ARCHIVE_FILE   ${MSTK_VERSION_MAJOR}.${MSTK_VERSION_MINOR}.${MSTK_VERSION_PATCH}.tar.gz)
set(MSTK_SAVEAS_FILE    mstk-${MSTK_VERSION}.tar.gz)
set(MSTK_MD5_SUM        814e2d7202ac0dbc4d735d94e5548fc8)

#
# TPL: MOAB
#
set(MOAB_VERSION_MAJOR  5)
set(MOAB_VERSION_MINOR  1)
set(MOAB_VERSION_PATCH  0)
set(MOAB_VERSION ${MOAB_VERSION_MAJOR}.${MOAB_VERSION_MINOR}.${MOAB_VERSION_PATCH})
set(MOAB_URL_STRING     ftp://ftp.mcs.anl.gov/pub/fathom)
set(MOAB_ARCHIVE_FILE   moab-${MOAB_VERSION}.tar.gz)
set(MOAB_SAVEAS_FILE    ${MOAB_ARCHIVE_FILE})
set(MOAB_MD5_SUM        875435654b2bc81b97e85894c2a45aaa)

#
# TPL: HYPRE
#
set(HYPRE_VERSION_MAJOR  2)
set(HYPRE_VERSION_MINOR  14)
set(HYPRE_VERSION_PATCH  0)
set(HYPRE_VERSION  ${HYPRE_VERSION_MAJOR}.${HYPRE_VERSION_MINOR}.${HYPRE_VERSION_PATCH})
set(HYPRE_URL_STRING     "https://github.com/LLNL/hypre/archive/")
set(HYPRE_ARCHIVE_FILE   v${HYPRE_VERSION}.tar.gz)
set(HYPRE_SAVEAS_FILE    hypre-${HYPRE_VERSION}.tar.gz)
set(HYPRE_MD5_SUM        ecde5cc807ec45bfb647e9f28d2eaea1)

#
# TPL: ParMetis
#
set(ParMetis_VERSION_MAJOR  4)
set(ParMetis_VERSION_MINOR  0)
set(ParMetis_VERSION_PATCH  3a)
set(ParMetis_VERSION  ${ParMetis_VERSION_MAJOR}.${ParMetis_VERSION_MINOR}.${ParMetis_VERSION_PATCH})
set(ParMetis_URL_STRING     ${AMANZI_TPLS_DOWNLOAD_URL})
set(ParMetis_ARCHIVE_FILE   parmetis-${ParMetis_VERSION}.tar.gz)
set(ParMetis_SAVEAS_FILE    ${ParMetis_ARCHIVE_FILE})
set(ParMetis_MD5_SUM        56ac6ebf6e7e8a522fa053c799dc7a92)

#
# TPL: SuperLU
#
set(SuperLU_VERSION_MAJOR  5)
set(SuperLU_VERSION_MINOR  2)
set(SuperLU_VERSION_PATCH  1)
set(SuperLU_VERSION  ${SuperLU_VERSION_MAJOR}.${SuperLU_VERSION_MINOR}.${SuperLU_VERSION_PATCH})
set(SuperLU_URL_STRING     "http://crd-legacy.lbl.gov/~xiaoye/SuperLU")
set(SuperLU_ARCHIVE_FILE   superlu_${SuperLU_VERSION}.tar.gz)
set(SuperLU_SAVEAS_FILE    superlu_${SuperLU_VERSION}.tar.gz)
set(SuperLU_MD5_SUM        3a1a9bff20cb06b7d97c46d337504447)

#
# TPL: SuperLU_Dist
#
set(SuperLUDist_VERSION_MAJOR  5)
set(SuperLUDist_VERSION_MINOR  4)
set(SuperLUDist_VERSION_PATCH  0)
set(SuperLUDist_VERSION  ${SuperLUDist_VERSION_MAJOR}.${SuperLUDist_VERSION_MINOR}.${SuperLUDist_VERSION_PATCH})
set(SuperLUDist_URL_STRING     "https://github.com/xiaoyeli/superlu_dist/archive")
set(SuperLUDist_ARCHIVE_FILE   v${SuperLUDist_VERSION}.tar.gz)
set(SuperLUDist_SAVEAS_FILE    superlu_dist_${SuperLUDist_VERSION}.tar.gz)
set(SuperLUDist_MD5_SUM        334cd350ae375be63ccf4d62900be3e4)

#
# TPL: Sowing (Built by PETSc!)
#
set(Sowing_VERSION_MAJOR  1)
set(Sowing_VERSION_MINOR  1)
set(Sowing_VERSION_PATCH  25-p1)
set(Sowing_VERSION  ${Sowing_VERSION_MAJOR}.${Sowing_VERSION_MINOR}.${Sowing_VERSION_PATCH})
set(Sowing_URL_STRING     "https://bitbucket.org/petsc/pkg-sowing/get")
set(Sowing_ARCHIVE_FILE   v${Sowing_VERSION}.tar.gz)
set(Sowing_SAVEAS_FILE    sowing-${Sowing_VERSION}.tar.gz)
set(Sowing_MD5_SUM        30ae83576038f00def33619a31f6a14c)

#
# TPL: PETSc
#
set(PETSc_VERSION_MAJOR  3)
set(PETSc_VERSION_MINOR  11)
set(PETSc_VERSION_PATCH  3)
set(PETSc_VERSION  ${PETSc_VERSION_MAJOR}.${PETSc_VERSION_MINOR}.${PETSc_VERSION_PATCH})
set(PETSc_ARCHIVE_VERSION ${PETSc_VERSION_MAJOR}.${PETSc_VERSION_MINOR}.${PETSc_VERSION_PATCH})
set(PETSc_URL_STRING     "https://gitlab.com/petsc/petsc/-/archive/v${PETSc_VERSION}")
set(PETSc_ARCHIVE_FILE   petsc-v${PETSc_VERSION}.tar.gz)
set(PETSc_SAVEAS_FILE    petsc-${PETSc_VERSION}.tar.gz)
set(PETSc_MD5_SUM        c5f8e3aa0bed20a8ab685fb809950fce)


#
# TPL: Trilinos
#
set(Trilinos_VERSION_MAJOR 12)
set(Trilinos_VERSION_MINOR 18)
set(Trilinos_VERSION_PATCH 55a7599733-Nov11)
set(Trilinos_VERSION ${Trilinos_VERSION_MAJOR}-${Trilinos_VERSION_MINOR}-${Trilinos_VERSION_PATCH})
set(Trilinos_URL_STRING     "https://github.com/trilinos/Trilinos/archive")
set(Trilinos_ARCHIVE_FILE   trilinos-release-${Trilinos_VERSION}.tar.gz)
set(Trilinos_SAVEAS_FILE    ${Trilinos_ARCHIVE_FILE})
set(Trilinos_GIT_REPOSITORY "https://github.com/trilinos/Trilinos")
set(Trilinos_GIT_TAG        "55a75997332636a28afc9db1aee4ae46fe8d93e7")

#
# TPL: SEACAS
#
set(SEACAS_VERSION_MAJOR 173a1e6)
set(SEACAS_VERSION_MINOR 0)
set(SEACAS_VERSION_PATCH 0)
set(SEACAS_VERSION ${SEACAS_VERSION_MAJOR})
set(SEACAS_URL_STRING     ${AMANZI_TPLS_DOWNLOAD_URL})
set(SEACAS_ARCHIVE_FILE   seacas-${SEACAS_VERSION}.tgz)
set(SEACAS_SAVEAS_FILE    ${SEACAS_ARCHIVE_FILE})
set(SEACAS_MD5_SUM        3235d1b885ee8e1a04408382f50bd0f0)

#
# TPL: PFlotran
#
set(PFLOTRAN_VERSION_MAJOR 0)
set(PFLOTRAN_VERSION_MINOR 0)
set(PFLOTRAN_VERSION_PATCH b681066)
set(PFLOTRAN_VERSION ${PFLOTRAN_VERSION_MAJOR}.${PFLOTRAN_VERSION_MINOR}.${PFLOTRAN_VERSION_PATCH})
set(PFLOTRAN_URL_STRING     "https://bitbucket.org/pflotran/pflotran/get/release")
set(PFLOTRAN_ARCHIVE_FILE   pflotran-b681066-20Sep2019.tar.gz)
set(PFLOTRAN_SAVEAS_FILE    pflotran-b681066-20Sep2019.tar.gz)
set(PFLOTRAN_MD5_SUM        486b0981a90aa662644b907f0a42e7fe)
set(PFLOTRAN_GIT_REPOSITORY "https://bitbucket.org/pflotran/pflotran/src/master")
set(PFLOTRAN_GIT_TAG        "b68106687fd4ea421468fda130ea053d44e0262e")

#
# TPL: Alquimia
#
set(ALQUIMIA_VERSION_MAJOR 1)
set(ALQUIMIA_VERSION_MINOR 0)
set(ALQUIMIA_VERSION_PATCH 6-rc)
set(ALQUIMIA_VERSION ${ALQUIMIA_VERSION_MAJOR}.${ALQUIMIA_VERSION_MINOR}.${ALQUIMIA_VERSION_PATCH})
set(ALQUIMIA_URL_STRING     https://github.com/LBL-EESA/alquimia-dev/archive)
set(ALQUIMIA_ARCHIVE_FILE   v${ALQUIMIA_VERSION}.tar.gz)
set(ALQUIMIA_SAVEAS_FILE    alquimia-dev-${ALQUIMIA_VERSION}.tar.gz)
set(ALQUIMIA_MD5_SUM        5bcf12c69c63cbd7912facc67b859af1)

#
# TPL: Silo
#
set(Silo_VERSION_MAJOR 4)
set(Silo_VERSION_MINOR 10)
set(Silo_VERSION_PATCH 2)
set(Silo_VERSION  ${Silo_VERSION_MAJOR}.${Silo_VERSION_MINOR}.${Silo_VERSION_PATCH})
set(Silo_URL_STRING    "https://wci.llnl.gov/content/assets/docs/simulation/computer-codes/silo/silo-4.10.2")
set(Silo_ARCHIVE_FILE  silo-4.10.2.tar.gz)
set(Silo_SAVEAS_FILE   ${Silo_ARCHIVE_FILE})
set(Silo_MD5_SUM 9ceac777a2f2469ac8cef40f4fab49c8)

#
# TPL: CrunchTope
#
set(CRUNCHTOPE_VERSION_MAJOR 020420)
set(CRUNCHTOPE_VERSION_MINOR 906e164)
set(CRUNCHTOPE_VERSION_PATCH 0)
set(CRUNCHTOPE_VERSION  ${CRUNCHTOPE_VERSION_MAJOR}.${CRUNCHTOPE_VERSION_MINOR}.${CRUNCHTOPE_VERSION_PATCH})
set(CRUNCHTOPE_URL_STRING    ${AMANZI_TPLS_DOWNLOAD_URL})
set(CRUNCHTOPE_ARCHIVE_FILE  CrunchTope_020420-906e164.0.tgz)
set(CRUNCHTOPE_SAVEAS_FILE   ${CRUNCHTOPE_ARCHIVE_FILE})
set(CRUNCHTOPE_MD5_SUM       059766e149e2a47c754ecf9815641d71)

#
# TPL: Nanoflann
#
set(Nanoflann_VERSION_MAJOR 1)
set(Nanoflann_VERSION_MINOR 3)
set(Nanoflann_VERSION_PATCH 1)
set(Nanoflann_VERSION  ${Nanoflann_VERSION_MAJOR}.${Nanoflann_VERSION_MINOR}.${Nanoflann_VERSION_PATCH})
set(Nanoflann_URL_STRING    "https://github.com/jlblancoc/nanoflann/archive")
set(Nanoflann_ARCHIVE_FILE  v${Nanoflann_VERSION}.tar.gz)
set(Nanoflann_SAVEAS_FILE   nanoflann-${Nanoflann_VERSION}.tar.gz)
set(Nanoflann_MD5_SUM       230edd6cb3843a5377743fe87959ffc5)

#
# TPL: CLM
#
set(CLM_VERSION_MAJOR 0)
set(CLM_VERSION_MINOR 1)
set(CLM_VERSION_PATCH 0)
set(CLM_VERSION  ${CLM_VERSION_MAJOR}.${CLM_VERSION_MINOR}.${CLM_VERSION_PATCH})
set(CLM_URL_STRING    "https://github.com/amanzi/pf-clm/archive")
set(CLM_ARCHIVE_FILE  v${CLM_VERSION}.tar.gz)
set(CLM_SAVEAS_FILE   clm-${CLM_ARCHIVE_FILE})
set(CLM_MD5_SUM       e404e9a24691c81e410f1a553b47ebbe)

#
# TPL: exprtk
#
set(EXPRTK_VERSION_MAJOR 0)
set(EXPRTK_VERSION_MINOR 0)
set(EXPRTK_VERSION_PATCH d81ac1a)
set(EXPRTK_VERSION  ${EXPRTK_VERSION_MAJOR}.${EXPRTK_VERSION_MINOR}.${EXPRTK_VERSION_PATCH})
set(EXPRTK_URL_STRING    ${AMANZI_TPLS_DOWNLOAD_URL})
set(EXPRTK_ARCHIVE_FILE  exprtk-${EXPRTK_VERSION}.zip)
set(EXPRTK_SAVEAS_FILE   ${EXPRTK_ARCHIVE_FILE})
set(EXPRTK_MD5_SUM       3cd40d9d5b2cb913f1eb4fe07c7d04eb)

