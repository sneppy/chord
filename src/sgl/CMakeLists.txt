# Setup library ---------------------------------
project(sgl)

## Gather source and headers files
file(GLOB_RECURSE SOURCES

	private/*.c
	private/*.cpp
	private/*.cxx
)
file(GLOB_RECURSE HEADERS public

	public/*.h
	public/*.hpp
	public/*.tpp
)

## Create library
add_library(${PROJECT_NAME}

	${SOURCES}
	${HEADERS}
)

## Include directories
target_include_directories(${PROJECT_NAME}

	PUBLIC
		public
)