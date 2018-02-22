
include(CheckCSourceCompiles)

macro(check_c_linker_flag _FLAG _RESULT)
    set(PREV_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
    set(CMAKE_REQUIRED_FLAGS "${_FLAG}")
    check_c_source_compiles("int main() { return 0; }" ${_RESULT})
    set(CMAKE_REQUIRED_FLAGS "${PREV_CMAKE_REQUIRED_FLAGS}")
endmacro(check_c_linker_flag)