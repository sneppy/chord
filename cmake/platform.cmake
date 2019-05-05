# Platform setup --------------------------------
if(UNIX AND (NOT APPLE))
	add_compile_definitions(PLATFORM_UNIX=1)
elseif(WIN32)
	add_compile_definitions(PLATFORM_WINDOWS=1)
elseif(APPLE)
	add_compile_definitions(PLATFORM_APPLE=1)
endif()