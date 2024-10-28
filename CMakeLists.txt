cmake_minimum_required(VERSION 3.19.0)

project(vst3utils VERSION 1.0.0)

add_library(vst3utils INTERFACE
	"include/vst3utils/buffer.h"
	"include/vst3utils/byteorder_stream.h"
	"include/vst3utils/enum_array.h"
	"include/vst3utils/event_iterator.h"
	"include/vst3utils/events.h"
	"include/vst3utils/message.h"
	"include/vst3utils/norm_plain_conversion.h"
	"include/vst3utils/observable.h"
	"include/vst3utils/parameter_changes_iterator.h"
	"include/vst3utils/parameter_description.h"
	"include/vst3utils/parameter_updater.h"
	"include/vst3utils/parameter.h"
	"include/vst3utils/smooth_value.h"
	"include/vst3utils/string_conversion.h"
	"include/vst3utils/transport_state_observer.h"
	"ReadMe.md"
)

target_include_directories(vst3utils INTERFACE "include")

target_compile_features(vst3utils
    INTERFACE
        cxx_std_17
)

option(VST3UTILS_TESTS "Enable unit test target" OFF)
set(VST3UTILS_TESTS_SDK_PATH "" CACHE PATH "Path to the VST SDK for unit testing")
if(VST3UTILS_TESTS)

	enable_testing()

	set(BUILD_GMOCK 0)
	set(INSTALL_GTEST 0)
	add_subdirectory(tests/googletest)

	add_executable(vst3utils_test
		"tests/buffer_test.cpp"
		"tests/norm_plain_conversion_test.cpp"
		"tests/observable_test.cpp"
		"tests/string_conversion_test.cpp"
		"tests/transport_state_observer_test.cpp"
	)

	target_link_libraries(vst3utils_test
		PRIVATE
			vst3utils
			gtest_main
	)

	target_compile_features(vst3utils_test
		INTERFACE
			cxx_std_17
	)

	if(CMAKE_XCODE_BUILD_SYSTEM EQUAL 12)
		add_custom_command(
			TARGET vst3utils_test
			POST_BUILD
			COMMAND 
				codesign --force -s - $<TARGET_FILE:vst3utils_test>
		)
	endif()

	if(SMTG_WIN)
		target_compile_options(vst3utils_test PRIVATE "/utf-8" "/Zc:__cplusplus")
	endif()

	if(VST3UTILS_TESTS_SDK_PATH)
		set(SMTG_ENABLE_VSTGUI_SUPPORT 0)
		set(SMTG_ENABLE_VST3_HOSTING_EXAMPLES 0)
		set(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES 0)
		set(SMTG_ADD_VST3_UTILITIES 0)
		set(SMTG_RUN_VST_VALIDATOR 0)
		set(SMTG_COREAUDIO_SDK_PATH "")
		add_subdirectory("${VST3UTILS_TESTS_SDK_PATH}" "${PROJECT_BINARY_DIR}/vst3sdk")
		smtg_enable_vst3_sdk()

		target_sources(vst3utils_test PRIVATE
			"tests/attribute_list_test.cpp"
			"tests/events_test.cpp"
			"tests/message_test.cpp"
		)

		target_link_libraries(vst3utils_test
			PRIVATE
				sdk_hosting
		)

		if(SMTG_MAC)
			target_link_libraries(vst3utils_test
				PRIVATE
					"-framework CoreFoundation"
			)
		endif()

	endif()

	include(GoogleTest)
	gtest_discover_tests(vst3utils_test)
	
endif(VST3UTILS_TESTS)
