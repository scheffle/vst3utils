//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#include "vst3utils/norm_plain_conversion.h"
#include <gtest/gtest.h>

//------------------------------------------------------------------------
namespace vst3utils {

//------------------------------------------------------------------------
TEST (norm_plain_conversion_test, normalized_to_plain)
{
	auto plain = normalized_to_plain (0., 100., 0.5);
	EXPECT_DOUBLE_EQ (plain, 50.);
	plain = normalized_to_plain (0., 100., 0.);
	EXPECT_DOUBLE_EQ (plain, 0.);
	plain = normalized_to_plain (0., 100., 1.);
	EXPECT_DOUBLE_EQ (plain, 100.);
	plain = normalized_to_plain (-200., 200., 0.5);
	EXPECT_DOUBLE_EQ (plain, 0.);
	plain = normalized_to_plain (-200., 200., 0.);
	EXPECT_DOUBLE_EQ (plain, -200.);
	plain = normalized_to_plain (-200., 200., 1.);
	EXPECT_DOUBLE_EQ (plain, 200.);
}

//------------------------------------------------------------------------
TEST (norm_plain_conversion_test, plain_to_normalized)
{
	auto norm = plain_to_normalized (0., 100., 50.);
	EXPECT_DOUBLE_EQ (norm, 0.5);
	norm = plain_to_normalized (0., 100., 0.);
	EXPECT_DOUBLE_EQ (norm, 0.);
	norm = plain_to_normalized (0., 100., 100.);
	EXPECT_DOUBLE_EQ (norm, 1.);
	norm = plain_to_normalized (-200., 200., 0.);
	EXPECT_DOUBLE_EQ (norm, 0.5);
	norm = plain_to_normalized (-200., 200., -200.);
	EXPECT_DOUBLE_EQ (norm, 0.);
	norm = plain_to_normalized (-200., 200., 200.);
	EXPECT_DOUBLE_EQ (norm, 1.);
}

//------------------------------------------------------------------------
TEST (norm_plain_conversion_test, normalized_to_steps)
{
	auto step = normalized_to_steps (9, 0, 0.);
	EXPECT_EQ (step, 0);
	step = normalized_to_steps (9, 0, 1.);
	EXPECT_EQ (step, 9);
	step = normalized_to_steps (4, 0, 0.25);
	EXPECT_EQ (step, 1);
	step = normalized_to_steps (4, 0, 0.5);
	EXPECT_EQ (step, 2);
	step = normalized_to_steps (4, 0, 0.75);
	EXPECT_EQ (step, 3);
	step = normalized_to_steps (4, 0, 1.);
	EXPECT_EQ (step, 4);
	step = normalized_to_steps (4, 10, 1.);
	EXPECT_EQ (step, 14);
}

//------------------------------------------------------------------------
TEST (norm_plain_conversion_test, steps_to_normalized)
{
	auto norm = steps_to_normalized (4, 0, 0);
	EXPECT_DOUBLE_EQ (norm, 0.);
	norm = steps_to_normalized (4, 0, 1);
	EXPECT_DOUBLE_EQ (norm, 0.25);
	norm = steps_to_normalized (4, 0, 2);
	EXPECT_DOUBLE_EQ (norm, 0.5);
	norm = steps_to_normalized (4, 0, 3);
	EXPECT_DOUBLE_EQ (norm, 0.75);
	norm = steps_to_normalized (4, 0, 4);
	EXPECT_DOUBLE_EQ (norm, 1.);
	norm = steps_to_normalized (4, 10, 14);
	EXPECT_DOUBLE_EQ (norm, 1.);
}

//------------------------------------------------------------------------
TEST (norm_plain_conversion_test, normalized_to_exp)
{
	auto exp = normalized_to_exp (0., 100., 0.);
	EXPECT_DOUBLE_EQ (exp, 0.);
	exp = normalized_to_exp (0., 1., 1.);
	EXPECT_DOUBLE_EQ (exp, 1.);
	exp = normalized_to_exp (80., 22050., 0.);
	EXPECT_DOUBLE_EQ (exp, 80.);
	exp = normalized_to_exp (80., 22050., 1.);
	EXPECT_DOUBLE_EQ (exp, 22050.);
	exp = normalized_to_exp (80., 22050., 0.1);
	EXPECT_DOUBLE_EQ (std::round (exp), 140.);
	exp = normalized_to_exp (80., 22050., 0.2);
	EXPECT_DOUBLE_EQ (std::round (exp), 246.);
	exp = normalized_to_exp (80., 22050., 0.5);
	EXPECT_DOUBLE_EQ (std::round (exp), 1328.);
	exp = normalized_to_exp (80., 22050., 0.7);
	EXPECT_DOUBLE_EQ (std::round (exp), 4086.);
	exp = normalized_to_exp (80., 22050., 0.8);
	EXPECT_DOUBLE_EQ (std::round (exp), 7167.);
	exp = normalized_to_exp (80., 22050., 0.9);
	EXPECT_DOUBLE_EQ (std::round (exp), 12571.);
}

//------------------------------------------------------------------------
TEST (norm_plain_conversion_test, exp_to_normalized)
{
	auto norm = exp_to_normalized (0., 100., 0.);
	EXPECT_DOUBLE_EQ (norm, 0.);
	norm = exp_to_normalized (0., 100., 100.);
	EXPECT_DOUBLE_EQ (norm, 1.);
	norm = exp_to_normalized (80., 22050., 80.);
	EXPECT_DOUBLE_EQ (norm, 0.);
	norm = exp_to_normalized (80., 22050., 22050.);
	EXPECT_DOUBLE_EQ (norm, 1.);
	norm = exp_to_normalized (80., 22050., 140.);
	EXPECT_DOUBLE_EQ (std::round (norm * 100.) / 100., 0.1);
	norm = exp_to_normalized (80., 22050., 246.);
	EXPECT_DOUBLE_EQ (std::round (norm * 100.) / 100., 0.2);
	norm = exp_to_normalized (80., 22050., 1328.);
	EXPECT_DOUBLE_EQ (std::round (norm * 100.) / 100., 0.5);
	norm = exp_to_normalized (80., 22050., 7167.);
	EXPECT_DOUBLE_EQ (std::round (norm * 100.) / 100., 0.8);
	norm = exp_to_normalized (80., 22050., 12571.);
	EXPECT_DOUBLE_EQ (std::round (norm * 100.) / 100., 0.9);
}

//------------------------------------------------------------------------
} // vst3utils
