cmake_minimum_required(VERSION 3.14.0)

project(vst3utils VERSION 1.0.0)

add_library(vst3utils INTERFACE
	"include/vst3utils/buffer.h"
	"include/vst3utils/byteorder_stream.h"
	"include/vst3utils/enum_array.h"
	"include/vst3utils/event_iterator.h"
	"include/vst3utils/message.h"
	"include/vst3utils/norm_plain_conversion.h"
	"include/vst3utils/parameter_changes_iterator.h"
	"include/vst3utils/parameter_description.h"
	"include/vst3utils/parameter_updater.h"
	"include/vst3utils/parameter.h"
	"include/vst3utils/smooth_value.h"
	"include/vst3utils/string_conversion.h"
	"ReadMe.md"
)

target_include_directories(vst3utils INTERFACE "include")

target_compile_features(vst3utils
    INTERFACE
        cxx_std_17
)

