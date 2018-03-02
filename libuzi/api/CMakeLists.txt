
set(API_FILES uzi.c)

uzi_module_add(${API_FILES})

if(BUILD_TESTING)
	add_subdirectory(test)
endif()

