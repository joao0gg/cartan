file(READ ${INPUT} content)
get_filename_component(name ${INPUT} NAME)
string(REPLACE "." "_" name ${name})
file(WRITE ${OUTPUT} "#pragma once\n\ninline constexpr char ${name}[] = R\"glsl(${content})glsl\";\n")
