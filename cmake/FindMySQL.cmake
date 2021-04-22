INCLUDE (FindPackageHandleStandardArgs)


SET(MYSQL_ROOT_DIR  /usr/local/mysql/connector-c++-/)
# Re-use the previous path:
FIND_PATH (MYSQL_INCLUDE_DIR
        NAMES mysqlx/xapi.h
        HINTS ${MYSQL_ROOT_DIR}
        PATH_SUFFIXES include
        DOC "MySQL include directory")
FIND_LIBRARY (MYSQL_LIBRARY
        NAMES libmysqlcppconn8.so
        HINTS ${MYSQL_ROOT_DIR}
        PATH_SUFFIXES lib64/debug
        DOC "MySQL debug library")
#FIND_LIBRARY (MYSQL_LIBRARY_RELEASE
#        NAMES libmysqlcppconn8.so
#        HINTS ${MYSQL_ROOT_DIR}
#        PATH_SUFFIXES lib64
#        DOC "MySQL release library")


SET (MYSQL_INCLUDE_DIRS ${MYSQL_INCLUDE_DIR})
SET (MYSQL_LIBRARIES ${MYSQL_LIBRARY})

MARK_AS_ADVANCED (MYSQL_INCLUDE_DIR MYSQL_LIBRARY)

FIND_PACKAGE_HANDLE_STANDARD_ARGS (MySQL REQUIRED_VARS MYSQL_ROOT_DIR
        MYSQL_INCLUDE_DIR MYSQL_LIBRARY)
