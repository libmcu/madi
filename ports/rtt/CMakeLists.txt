cmake_minimum_required(VERSION 3.16)

project(rtt LANGUAGES C CXX)

if (NOT DEFINED RTT_ROOT)
	set(RTT_ROOT ${CMAKE_SOURCE_DIR}/external/SEGGER_RTT_V784e)
endif()

add_library(${PROJECT_NAME} STATIC
	${RTT_ROOT}/RTT/SEGGER_RTT.c
	${RTT_ROOT}/RTT/SEGGER_RTT_printf.c
)

target_compile_features(${PROJECT_NAME} PRIVATE c_std_99)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		${RTT_ROOT}/RTT
		${RTT_ROOT}/Config
)
