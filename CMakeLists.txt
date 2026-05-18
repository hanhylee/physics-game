cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)
include(FetchContent)

project(game
	VERSION 1.0.0
	DESCRIPTION "Shows how to integrate Box2D with raylib"
	HOMEPAGE_URL "https://box2d.org"
	LANGUAGES C CXX
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

FetchContent_Declare(
	box2d
	GIT_REPOSITORY https://github.com/erincatto/box2d.git
	GIT_TAG main
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(box2d)

FetchContent_Declare(
	raylib
	GIT_REPOSITORY https://github.com/raysan5/raylib.git
	GIT_TAG master
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(raylib)

add_executable(game src/main.cpp
		src/level.cpp
		src/level.h
		src/entity.cpp
		src/entity.h
		src/environment.cpp
		src/environment.h
		src/player.cpp
		src/player.h
		src/enemy.cpp
		src/enemy.h
		src/turret.cpp
		src/turret.h)
target_link_libraries(game PRIVATE box2d raylib)

add_custom_command(TARGET game POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/assets
		$<TARGET_FILE_DIR:game>/assets
)

if (MSVC)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT game)
	set_property(TARGET game PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()

if (EMSCRIPTEN)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")
	# Since WASM is used, ALLOW_MEMORY_GROWTH has no extra overheads
	#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sWASM=1 -sASYNCIFY -sALLOW_MEMORY_GROWTH=1 --shell-file ${CMAKE_SOURCE_DIR}/minshell.html")
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
	target_link_options(game PROPERTIES LINK_FLAGS "--preload-file images@images -sWASM=1 -sASYNCIFY -sALLOW_MEMORY_GROWTH=1 --shell-file ${CMAKE_SOURCE_DIR}/minshell.html")
endif()
