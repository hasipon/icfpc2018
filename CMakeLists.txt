cmake_minimum_required(VERSION 3.5)
project(icfpc2018)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER clang++-3.8)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY "./nbts")
enable_testing()


set(CMAKE_CXX_FLAGS "-Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O2 -pg")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -s -DNDEBUG -march=native")

file(GLOB AI_MAINS AI/*/*.cpp)

# AIのMAINのビルド
foreach (AI_MAIN ${AI_MAINS})
    get_filename_component(AI_MAIN_NAME ${AI_MAIN} NAME_WE)
    add_executable(${AI_MAIN_NAME} ${AI_MAIN})
endforeach (AI_MAIN)

# tracerのビルド
add_executable(tracer4 "tracer/tracer4.cpp")

file(GLOB R_FILES problemsF/*.r)

add_test(NAME MAKE
        COMMAND make)

#問題のループ
foreach(R_FILE ${R_FILES})
    # 問題名
    get_filename_component(PROBLEM ${R_FILE} NAME_WE)
    foreach (AI_MAIN ${AI_MAINS})
        # AI名
        get_filename_component(AI ${AI_MAIN} NAME_WE)
        file(MAKE_DIRECTORY "./out/${AI}")

        set(TEST_NAME "${AI}_${PROBLEM}")
        set(MODEL_SRC "./problemsF/${PROBLEM}_src.mdl")
        set(MODEL_TGT "./problemsF/${PROBLEM}_tgt.mdl")
        set(OUTPUT_FILE "./nbts/${TEST_NAME}.nbt")
        add_test(NAME ${TEST_NAME}
                COMMAND ./bin/${AI} ${MODEL_SRC} ${MODEL_TGT} ${OUTPUT_FILE})
        set_tests_properties(${TEST_NAME} PROPERTIES DEPENDS MAKE)

        set(GZIP_TEST_NAME "GZIP_${TEST_NAME}")
        set(GZIPCOMM "gzip -9 -k -f ${OUTPUT_FILE} && cp ${OUTPUT_FILE}.gz ./out/${AI}/")
        add_test(NAME ${GZIP_TEST_NAME}
                COMMAND sh -c "${GZIPCOMM}")
        set_tests_properties(${GZIP_TEST_NAME} PROPERTIES DEPENDS ${TEST_NAME})
    endforeach()
endforeach()


