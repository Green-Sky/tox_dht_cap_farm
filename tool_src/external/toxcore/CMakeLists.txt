cmake_minimum_required(VERSION 3.14...3.24 FATAL_ERROR)

include(FetchContent)

if (NOT TARGET toxcore)
	set(EXPERIMENTAL_API ON CACHE BOOL "" FORCE)
	set(UNITTEST OFF CACHE BOOL "" FORCE)
	set(DHT_BOOTSTRAP OFF CACHE BOOL "" FORCE)
	set(BOOTSTRAP_DAEMON OFF CACHE BOOL "" FORCE)
	set(BUILD_TOXAV OFF CACHE BOOL "" FORCE)
	set(ENABLE_SHARED OFF CACHE BOOL "" FORCE)
	set(TOXCORE_COMMIT_HASH "102a1fa")

	FetchContent_Declare(c-toxcore
		GIT_REPOSITORY https://github.com/TokTok/c-toxcore
		GIT_TAG ${TOXCORE_COMMIT_HASH}

		FIND_PACKAGE_ARGS # for the future
	)
	FetchContent_MakeAvailable(c-toxcore)

	# the sad case
	add_library(toxcore INTERFACE)

	if (TARGET toxcore_static)
		target_link_libraries(toxcore INTERFACE toxcore_static)
	else()
		target_link_libraries(toxcore INTERFACE toxcore_shared)
	endif()

	target_compile_definitions(toxcore INTERFACE TOXCORE_COMMIT_HASH=${TOXCORE_COMMIT_HASH})

	# HACK: "install" api headers into binary dir
	configure_file(
		${c-toxcore_SOURCE_DIR}/toxcore/tox.h
		${CMAKE_CURRENT_BINARY_DIR}/include/tox/tox.h
		@ONLY
	)
	configure_file(
		${c-toxcore_SOURCE_DIR}/toxcore/tox_private.h
		${CMAKE_CURRENT_BINARY_DIR}/include/tox/tox_private.h
		@ONLY
	)

	target_include_directories(toxcore INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/include/)
endif()

