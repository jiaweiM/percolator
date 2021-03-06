# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. The list of available CPACK_xxx variables and their associated
# documentation may be obtained using
#  cpack --help-variable-list
#
# Some variables are common to all generators (e.g. CPACK_PACKAGE_NAME)
# and some are specific to a generator
# (e.g. CPACK_NSIS_EXTRA_INSTALL_COMMANDS). The generator specific variables
# usually begin with CPACK_<GENNAME>_xxxx.


SET(CPACK_BINARY_7Z "")
SET(CPACK_BINARY_BUNDLE "")
SET(CPACK_BINARY_CYGWIN "")
SET(CPACK_BINARY_DEB "")
SET(CPACK_BINARY_DRAGNDROP "")
SET(CPACK_BINARY_IFW "")
SET(CPACK_BINARY_NSIS "")
SET(CPACK_BINARY_OSXX11 "")
SET(CPACK_BINARY_PACKAGEMAKER "")
SET(CPACK_BINARY_PRODUCTBUILD "")
SET(CPACK_BINARY_RPM "")
SET(CPACK_BINARY_STGZ "")
SET(CPACK_BINARY_TBZ2 "")
SET(CPACK_BINARY_TGZ "")
SET(CPACK_BINARY_TXZ "")
SET(CPACK_BINARY_TZ "")
SET(CPACK_BINARY_WIX "")
SET(CPACK_BINARY_ZIP "")
SET(CPACK_BUILD_SOURCE_DIRS "D:/code/library/percolator;D:/code/library/percolator")
SET(CPACK_CMAKE_GENERATOR "Visual Studio 15 2017")
SET(CPACK_COMPONENTS_ALL "System;Unspecified")
SET(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
SET(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Lukas Kall <lukas.kall@scilifelab.se>")
SET(CPACK_GENERATOR "NSIS")
SET(CPACK_INSTALL_CMAKE_PROJECTS "D:/code/library/percolator;PERCOLATOR;ALL;/")
SET(CPACK_INSTALL_PREFIX "C:/Program Files (x86)/PERCOLATOR")
SET(CPACK_MODULE_PATH "D:/code/library/percolator/cmake")
SET(CPACK_NSIS_CONTACT "Lukas Kall <lukas.kall@scilifelab.se>")
SET(CPACK_NSIS_DISPLAY_NAME "percolator-noxml-v3-02")
SET(CPACK_NSIS_INSTALLER_ICON_CODE "")
SET(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
SET(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
SET(CPACK_NSIS_MODIFY_PATH "ON")
SET(CPACK_NSIS_MUI_ICON "D:/code/library/percolator/admin/win/nsi\\percolator.ico")
SET(CPACK_NSIS_PACKAGE_NAME "percolator-noxml-v3-02")
SET(CPACK_OUTPUT_CONFIG_FILE "D:/code/library/percolator/CPackConfig.cmake")
SET(CPACK_PACKAGE_DEFAULT_LOCATION "/")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "D:/code/library/percolator/ReadMe.txt")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Percolator and qvality, two post processors for shotgun proteomics data.")
SET(CPACK_PACKAGE_FILE_NAME "percolator-noxml-v3-02")
SET(CPACK_PACKAGE_ICON "D:/code/library/percolator/admin/win/nsi\\page_header.bmp")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "percolator-noxml-v3-02")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "percolator-noxml-v3-02")
SET(CPACK_PACKAGE_NAME "percolator-noxml")
SET(CPACK_PACKAGE_RELOCATABLE "true")
SET(CPACK_PACKAGE_VENDOR "Percolator")
SET(CPACK_PACKAGE_VERSION "3.02.0")
SET(CPACK_PACKAGE_VERSION_MAJOR "3")
SET(CPACK_PACKAGE_VERSION_MINOR "02")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_RESOURCE_FILE_LICENSE "D:/code/library/percolator/license.txt")
SET(CPACK_RESOURCE_FILE_README "C:/Program Files/CMake/share/cmake-3.9/Templates/CPack.GenericDescription.txt")
SET(CPACK_RESOURCE_FILE_WELCOME "C:/Program Files/CMake/share/cmake-3.9/Templates/CPack.GenericWelcome.txt")
SET(CPACK_RPM_PACKAGE_DEPENDS " libc6, libgcc1")
SET(CPACK_RPM_PACKAGE_GROUP "Applications/Engineering")
SET(CPACK_RPM_PACKAGE_LICENSE "Apache 2.0 license")
SET(CPACK_RPM_PACKAGE_RELOCATABLE "FALSE")
SET(CPACK_RPM_PACKAGE_VENDOR "Percolator")
SET(CPACK_SET_DESTDIR "FALSE")
SET(CPACK_SOURCE_7Z "")
SET(CPACK_SOURCE_CYGWIN "")
SET(CPACK_SOURCE_GENERATOR "TGZ")
SET(CPACK_SOURCE_IGNORE_FILES "/CVS/;/.svn/;/.swp$/;cscope.*;/.git/;D:/code/library/percolator/;/.bzr/;/.settings/;D:/code/library/percolator/.travis.yml;")
SET(CPACK_SOURCE_OUTPUT_CONFIG_FILE "D:/code/library/percolator/CPackSourceConfig.cmake")
SET(CPACK_SOURCE_RPM "")
SET(CPACK_SOURCE_TBZ2 "")
SET(CPACK_SOURCE_TGZ "")
SET(CPACK_SOURCE_TXZ "")
SET(CPACK_SOURCE_TZ "")
SET(CPACK_SOURCE_ZIP "")
SET(CPACK_STRIP_FILES "TRUE")
SET(CPACK_SYSTEM_NAME "win32")
SET(CPACK_TOPLEVEL_TAG "win32")
SET(CPACK_WIX_SIZEOF_VOID_P "4")

if(NOT CPACK_PROPERTIES_FILE)
  set(CPACK_PROPERTIES_FILE "D:/code/library/percolator/CPackProperties.cmake")
endif()

if(EXISTS ${CPACK_PROPERTIES_FILE})
  include(${CPACK_PROPERTIES_FILE})
endif()
