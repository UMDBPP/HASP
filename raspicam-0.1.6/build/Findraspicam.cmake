# ===================================================================================
#  raspicam CMake configuration file
#
#             ** File generated automatically, do not modify **
#
#  Usage from an external project:
#    In your CMakeLists.txt, add these lines:
#
#    FIND_PACKAGE(raspicam REQUIRED )
#    TARGET_LINK_LIBRARIES(MY_TARGET_NAME )
#
#    This file will define the following variables:
#      - raspicam_LIBS          : The list of libraries to links against.
#      - raspicam_LIB_DIR       : The directory where lib files are. Calling LINK_DIRECTORIES
#                                with this path is NOT needed.
#      - raspicam_VERSION       : The  version of this PROJECT_NAME build. Example: "1.2.0"
#      - raspicam_VERSION_MAJOR : Major version part of VERSION. Example: "1"
#      - raspicam_VERSION_MINOR : Minor version part of VERSION. Example: "2"
#      - raspicam_VERSION_PATCH : Patch version part of VERSION. Example: "0"
#
# ===================================================================================
INCLUDE_DIRECTORIES(;/usr/local/include)
LINK_DIRECTORIES("/usr/local/lib")

SET(raspicam_LIBS /opt/vc/lib/libmmal_core.so;/opt/vc/lib/libmmal_util.so;/opt/vc/lib/libmmal.so raspicam) 
SET(raspicam_FOUND "YES") 

SET(raspicam_CV_FOUND  "")
SET(raspicam_CV_LIBS /opt/vc/lib/libmmal_core.so;/opt/vc/lib/libmmal_util.so;/opt/vc/lib/libmmal.so raspicam  raspicam_cv)

SET(raspicam_VERSION        0.1.6)
SET(raspicam_VERSION_MAJOR  0)
SET(raspicam_VERSION_MINOR  1)
SET(raspicam_VERSION_PATCH  6)
