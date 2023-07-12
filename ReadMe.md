# vst3utils

a c++ header only library with little helper classes for writing vst3 plug-ins.

## Requirements

- c++17 compatible compiler
- vst3 sdk

## Usage

Eiher add a header include directory pointing to the 'include' repository path or
if you use cmake then just add this directoy in cmake with `add_subdirectory` and add a
target dependency to your own target:

```cmake
add_subdirectory("path/to/vst3utils" ${PROJECT_BINARY_DIR}/vst3utils)

add_target_dependency(myTarget
  PRIVATE
    vst3utils
)
```

## History

- **Version 1.0.0** [07/16/2023]
	- initial release

## Headers

### `#include "vst3utils/enum_array.h`

- `vst3utils::enum_array`
	- a std::array using an enum for accessing its elements

### `#include "vst3utils/event_iterator.h"`

- `vst3utils::event_iterator`
	- a c++ compatible forward iterator for `Steinberg::Vst::Event`

### `#include "vst3utils/norm_plain_conversion.h`

contains functions to convert from normalized to plain and back

- `vst3utils::normalized_to_plain`
- `vst3utils::normalized_to_steps`
- `vst3utils::normalized_to_exp`
- `vst3utils::plain_to_normalized`
- `vst3utils::steps_to_normalized`
- `vst3utils::exp_to_normalized`

### `#include "vst3utils/parameter_description.h`

contains structs and functions to declare parameters at compile time

example:
```c++
static constexpr std::array<description, 4> param_desc = {{
  {range_description (u"volume", -12, linear_functions<-60, 0> (), 1, u"dB")},
  {range_description (u"cutoff", 2000, exponent_functions<80, 22050> (), 0, u"Hz")},
  {range_description (u"resonance", 50, linear_functions<0, 100> (), 0, u"%")},
  {list_description  (u"filter type", 0, strings_filter_types)},
}};
```

### `#include "vst3utils/parameter_updater.h`

- `vst3utils::throttled_parameter_updater`
	- a throttling realtime parameter updater

### `#include "vst3utils/parameter.h`

- `vst3utils::parameter`
	- extension to the parameter class of the vst3 sdk which uses a parameter description

### `#include "vst3utils/smooth_value.h`

- `vst3utils::smooth_value`
	- a value object that smoothly changes from one value to another

### `#include "vst3utils/string_conversion.h`

- `vst3utils::copy_utf16_to_ascii`
- `vst3utils::create_utf16_from_ascii`
- `vst3utils::copy_ascii_to_utf16`

## License

```
//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------
```
