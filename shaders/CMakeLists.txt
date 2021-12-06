cmake_minimum_required(VERSION 3.17)

add_executable(contract contract.cpp)
set_target_properties(contract PROPERTIES COMPILE_FLAGS "-O3 -std=c++17 -Wreturn-type -flto -fno-exceptions -fno-rtti -Wno-inline-new-delete -fno-exceptions -nostartfiles -Wno-invalid-noreturn -Wl,--no-entry,--allow-undefined,--export=Ctor,--export=Dtor,--export=Method_0,--export=Method_1,--export=Method_2,--export=Method_3,--export=Method_4,--export=Method_5 ${BEAM_SHADER_CLANG_OPTIONS_STR}")
set_target_properties(contract PROPERTIES LINK_FLAGS "-O3 -std=c++17 -Wreturn-type -flto -fno-exceptions -fno-rtti -Wno-inline-new-delete -fno-exceptions -nostartfiles -Wno-invalid-noreturn -Wl,--no-entry,--allow-undefined,--export=Ctor,--export=Dtor,--export=Method_0,--export=Method_1,--export=Method_2,--export=Method_3,--export=Method_4,--export=Method_5 ${BEAM_SHADER_CLANG_OPTIONS_STR}")

add_executable(app app.cpp)
set_target_properties(app PROPERTIES COMPILE_FLAGS "-O3 -std=c++17 -Wreturn-type -flto -fno-exceptions -fno-rtti -Wno-inline-new-delete -fno-exceptions -nostartfiles -Wno-invalid-noreturn -Wl,--no-entry,--allow-undefined,--export=Method_0,--export=Method_1 ${BEAM_SHADER_CLANG_OPTIONS_STR}")
set_target_properties(app PROPERTIES LINK_FLAGS "-O3 -std=c++17 -Wreturn-type -flto -fno-exceptions -fno-rtti -Wno-inline-new-delete -fno-exceptions -nostartfiles -Wno-invalid-noreturn -Wl,--no-entry,--allow-undefined,--export=Method_0,--export=Method_1 ${BEAM_SHADER_CLANG_OPTIONS_STR}")

target_link_libraries(contract PRIVATE Beam::shader-lib)
target_link_libraries(app PRIVATE Beam::shader-lib)

generate_sid(contract)
