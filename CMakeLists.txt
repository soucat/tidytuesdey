######## Project settings
cmake_minimum_required(VERSION 2.8.5)
set (PACKAGE_NAME uchardet)
project (${PACKAGE_NAME} CXX C)
enable_testing()

######## Package information
set (PACKAGE_URL https://www.freedesktop.org/wiki/Software/uchardet/)
set (PACKAGE_BUGREPORT https://bugs.freedesktop.org/enter_bug.cgi?product=uchardet)
set (UCHARDET_VERSION_MAJOR 0)
set (UCHARDET_VERSION_MINOR 0)
set (UCHARDET_VERSION_REVISION 6)

if (CMAKE_BUILD_TYPE MATCHES Debug)
	set (versi