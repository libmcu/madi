add_subdirectory(external/libmcu)
add_subdirectory(external/cbor)
add_subdirectory(ports/mcumgr)

if(APPLE)
	set(LIBMCU_NOINIT __attribute__\(\(section\(\"__DATA,.noinit.libmcu\"\)\)\))
else()
	set(LIBMCU_NOINIT __attribute__\(\(section\(\".noinit.libmcu\"\)\)\))
endif()

# Platform-specific actor priority configuration
if(TARGET_PLATFORM STREQUAL "madi_esp32s3")
	set(ACTOR_PRIORITY_BASE 16)
elseif(TARGET_PLATFORM STREQUAL "madi_nrf52840")
	set(ACTOR_PRIORITY_BASE 5)
else()
	set(ACTOR_PRIORITY_BASE 10) # Default fallback
endif()

target_compile_definitions(libmcu PUBLIC
	_POSIX_THREADS
	_POSIX_C_SOURCE=200809L
	LIBMCU_NOINIT=${LIBMCU_NOINIT}
	METRICS_USER_DEFINES=\"${PROJECT_SOURCE_DIR}/include/metrics.def\"
	WDT_STACK_SIZE_BYTES=4096
	LOGGING_MAX_BACKENDS=2
	LOGGING_TAGS_MAXNUM=32
	ACTOR_PRIORITY_BASE=${ACTOR_PRIORITY_BASE}
	ACTOR_PRIORITY_DESCENDING

	${APP_DEFS}
)
