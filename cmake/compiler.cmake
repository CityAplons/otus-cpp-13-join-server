# #
# Function to set compilation flags.
# #

get_property(L_CXX_STANDARD GLOBAL PROPERTY G_CXX_STANDARD)
get_property(L_CXX_STANDARD_REQUIRED GLOBAL PROPERTY G_CXX_STANDARD_REQUIRED)

function(set_compile_options target)
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD ${L_CXX_STANDARD}
        CXX_STANDARD_REQUIRED ${L_CXX_STANDARD_REQUIRED}
    )

    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W4
        )
    else()
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -pedantic -Werror
        )
    endif()
endfunction()