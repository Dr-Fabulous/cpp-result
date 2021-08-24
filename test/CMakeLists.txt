include(CTest)

if (BUILD_TESTING)
	add_executable(${PROJECT_NAME}-tests main.cpp)
	target_compile_features(${PROJECT_NAME}-tests PUBLIC cxx_std_17)
	target_link_libraries(${PROJECT_NAME}-tests PUBLIC ${PROJECT_NAME})

	add_test(NAME ${PROJECT_NAME}-tests
		 COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-tests)
endif()
