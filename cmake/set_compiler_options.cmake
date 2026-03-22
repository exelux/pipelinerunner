macro(set_compiler_options)
    plt_set_default_complier_options()

    if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCC OR MINGW)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -Wall")
    endif()

    add_compile_options(
        -Wno-class-memaccess
        -Wno-reorder
        -Wno-unused-function
        -Wno-unused-local-typedefs
    )

    set(CMAKE_SHARED_COMPILER_FLAGS "-Wl,--no-undefined")

    cmake_policy(SET CMP0010 NEW)
endmacro()
