#!/bin/bash

function try_glslc {
    if ! glslc --version > /dev/null;
    then
        echo "encountered error with finding glslc SPIR-V compiler installation."
        return 1
    fi
    echo "found glslc installation."
    return 0
}

function compile_shader {
    last_error=0

    if [[ $# -ne 2 ]];
    then 
        echo "compile_shader() expects 2 arguments: <path_to_shader> <output_name>"
        return 1
    fi

    echo "compiling shader at $1"
    if ! glslc $1 -o $2;
    then
        echo "failed to compile shader to $2. glslc exited with code $?"
        return 1
    fi
    echo "successfully compiled shader to $2"
    return 0
}

if ! try_glslc;
then
    exit 1
fi

echo "compiling vertex and fragment shaders..."

if ! compile_shader "default.vert" "vertex.spv" ||
   ! compile_shader "default.frag" "fragment.spv";
then 
    echo "compilation failed."
    exit 1
fi
echo "successfully compiled all shaders."
exit 0