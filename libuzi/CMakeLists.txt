
set(UZI_SOURCES
	alignment.c
	handle.c
	handle.h
	dictionary.c
	error.c
	file.c
	buffer.c
	memory.c
	string.c
	unicode.c
	interlocked.c
	barrier.c
	critical.c
	event.c
	init.c
	mutex.c
	semaphore.c
	sleep.c
	synch.h
	sysinfo.c
	thread.c
	thread.h
	tls.c
	timer.c
	wait.c
	uzi.c)

if(ANDROID)
	list(APPEND UZI_SOURCES cpu-features.c cpu-features.h)
endif()

add_library(uzi
	${UZI_SOURCES})

if(WIN32)
	target_link_libraries(ws2_32)
endif()

if(UNIX AND NOT APPLE)
	target_link_libraries(uzi rt)
endif()

target_link_libraries(uzi ${CMAKE_THREAD_LIBS_INIT})

if(BUILD_TESTING)
	add_subdirectory(test)
endif()

