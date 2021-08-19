cmake_minimum_required(VERSION 3.18.0)

project(cpp-result
	LANGUAGES CXX
	VERSION 0.0.0)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE inc)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

include(CTest)

if(BUILD_TESTING)
	add_executable(${PROJECT_NAME}-test test/main.cpp)
	target_compile_features(${PROJECT_NAME}-test PUBLIC cxx_std_17)
	target_link_libraries(${PROJECT_NAME}-test INTERFACE ${PROJECT_NAME})

	add_test(NAME ${PROJECT_NAME}-test
		 COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-test)

	add_custom_target(check
			  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-error
			  DEPENDS ${PROJECT_NAME}-test)
endif()
