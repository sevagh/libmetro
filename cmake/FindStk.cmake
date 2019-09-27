# STK_FOUND
# STK_INCLUDE_DIR
# STK_LIBRARY

find_path(STK_INCLUDE_DIR NAMES stk)

find_library(STK_LIBRARY NAMES stk)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(STK DEFAULT_MSG STK_LIBRARY STK_INCLUDE_DIR)

mark_as_advanced(STK_INCLUDE_DIR STK_LIBRARY)
