CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT("index")

SET(CMAKE_CXX_FLAGS "-std=c++2a -O2 -Wpedantic -Wall -fopenmp")
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
ADD_COMPILE_DEFINITIONS(WITH_TRUECOLOR)

FILE(GLOB SOURCES
	"src/*.cpp"
	"src/*.tcc"
	"src/TermboxWidgets/*.cpp"
	"src/TermboxWidgets/*.tcc"
	"src/Actions/*.cpp"
	"src/Actions/*.tcc"
	"src/UI/*.cpp"
)

ADD_SUBDIRECTORY(libs/fmt)
ADD_SUBDIRECTORY(src/TermboxWidgets/termbox)

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ${SOURCES})
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} libtermbox-static fmt)
