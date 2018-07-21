cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_COMPILER "clang++-3.8")
project(icfpc2018)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
enable_testing()


set(CMAKE_CXX_FLAGS "-Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O2 -pg")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -s -DNDEBUG -march=native")

file(GLOB AI_MAINS AI/*/*.cpp)
file(GLOB L_MODELS problemsL/*.mdl)

# AIのMAINのビルド
foreach (AI_MAIN ${AI_MAINS})
    get_filename_component(AI_MAIN_NAME ${AI_MAIN} NAME_WE)
    add_executable(${AI_MAIN_NAME} ${AI_MAIN})
endforeach (AI_MAIN)

# テスト
foreach (AI_MAIN ${AI_MAINS})
    get_filename_component(AI_MAIN_NAME ${AI_MAIN} NAME_WE)
    # テスト: lightningのモデルを対象とする
    if("${AI_MAIN}" MATCHES "SampleAI.cpp")
        continue()
    endif()
    # テスト: lightningのモデルを対象とする
    if("${AI_MAIN}" MATCHES "SampleAI.cpp")
        continue()
    endif()
    foreach(MODEL ${L_MODELS})
        get_filename_component(MODEL_NAME ${MODEL} NAME_WE)
        set(TEST_NAME "${AI_MAIN_NAME}_${MODEL_NAME}")
        set(BIN_FILE "./bin/${AI_MAIN_NAME}")
        set(TRACE_FILE "out/${TEST_NAME}.out")
        add_test(NAME ${TEST_NAME}
                COMMAND "./bin/${AI_MAIN_NAME}" ${MODEL} ${TRACE_FILE}
                )
        set_tests_properties(${TEST_NAME} PROPERTIES DEPENDS ${AI_MAIN_NAME})
        set_tests_properties(${TEST_NAME} PROPERTIES LABELS ${AI_MAIN_NAME})


        set(ASCII_TEST_NAME "${AI_MAIN_NAME}_${MODEL_NAME}_ASCII")
        add_test(NAME "${ASCII_TEST_NAME}"
                COMMAND tracer3 ${MODEL} ${TRACE_FILE} out/${TEST_NAME}.ascii
                )
        set_tests_properties(${ASCII_TEST_NAME} PROPERTIES DEPENDS ${TEST_NAME})
        set_tests_properties(${ASCII_TEST_NAME} PROPERTIES LABELS ${AI_MAIN_NAME})
    endforeach(MODEL)
endforeach (AI_MAIN)

# tracerのビルド
add_executable(tracer "tracer/tracer.cpp")
add_executable(tracer2 "tracer/tracer2.cpp")
add_executable(tracer3 "tracer/tracer3.cpp")

# modelerのビルド
add_executable(modeler "modeler/main.cpp")
