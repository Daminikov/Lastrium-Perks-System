

function(add_project_folder path)

    file(GLOB_RECURSE header_files
        "${path}include/*.h"
    )

    file(GLOB_RECURSE src_files
        "${path}src/*.cpp"
    )

    set(sources ${sources} ${src_files} PARENT_SCOPE)
    set(headers ${headers} ${header_files} PARENT_SCOPE)

endfunction()

