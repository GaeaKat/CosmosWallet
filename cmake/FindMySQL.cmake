INCLUDE (FindPackageHandleStandardArgs)


SET(MYSQL_ROOT_DIR  /usr/local/mysql/connector-c++-/)
# Re-use the previous path:
FIND_PATH (MYSQL_INCLUDE_DIR
        NAMES mysql/jdbc.h
        HINTS ${MYSQL_ROOT_DIR}
        PATH_SUFFIXES include
        DOC "MySQL include directory")
FIND_LIBRARY (MYSQL_LIBRARY
        NAMES libmysqlcppconn8.so
        HINTS ${MYSQL_ROOT_DIR}
        PATH_SUFFIXES lib64/debug
        DOC "MySQL debug library")

SET( MYSQL_OTHER_LIBRARY ${MYSQL_ROOT_DIR}lib64/debug/libmysqlcppconn.so)

#FIND_LIBRARY (MYSQL_LIBRARY_RELEASE
#        NAMES libmysqlcppconn8.so
#        HINTS ${MYSQL_ROOT_DIR}
#        PATH_SUFFIXES lib64
#        DOC "MySQL release library")

message("${MYSQL_OTHER_LIBRARY} - other one")
SET (MYSQL_INCLUDE_DIRS ${MYSQL_INCLUDE_DIR})
SET (MYSQL_LIBRARIES ${MYSQL_LIBRARY} ${MYSQL_OTHER_LIBRARY})

MARK_AS_ADVANCED (MYSQL_INCLUDE_DIR MYSQL_LIBRARY)

FIND_PACKAGE_HANDLE_STANDARD_ARGS (MySQL REQUIRED_VARS MYSQL_ROOT_DIR
        MYSQL_INCLUDE_DIR MYSQL_LIBRARY)
