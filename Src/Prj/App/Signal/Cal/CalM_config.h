/**
 * @file Ut_Signal/CalM_config.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Define calibration datasets
 *
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * See <https://www.gnu.org/licenses/>.
 */

using classifier_type = cal::input_classifier_type::classifier_type;

constexpr uint8  kAdDebounce = 5; ///< [10 ms] Debounce time
constexpr uint16 kGreenLo = (605 - 10);
constexpr uint16 kGreenHi = (605 + 10);
constexpr uint16 kYellowLo = (399 - 10);
constexpr uint16 kYellowHi = (399 + 10);
constexpr uint16 kRedLo = (192 - 10);
constexpr uint16 kRedHi = (192 + 10);
constexpr uint16 kWhiteLo = (147 - 10);
constexpr uint16 kWhiteHi = (147 + 10);
constexpr uint16 kAdMin = 0;
constexpr uint16 kAdMax = 1023;

constexpr uint8 kClassified = cal::input_type::kClassified;
constexpr uint8 kOnboard = cal::target_type::kOnboard;
constexpr uint8 kNone = cal::target_type::kNone;

#define INPUT0 { kClassified, 0 }
#define INPUT1 { kClassified, 1 }
#define INPUT2 { kClassified, 2 }
#define INPUT3 { kClassified, 3 }
#define INPUT4 { kClassified, 4 }
#define INPUT5 { kNone      , 0 }

#define ASPECTS0 { { 0b00011000, 0b00000000 }, { 0b00000100, 0b00000000 }, { 0b00000110, 0b00000000 }, { 0b00011001, 0b00000000 }, { 0b00011111, 0b00000000 } }
#define ASPECTS1 { { 0b00011000, 0b00000000 }, { 0b00000100, 0b00000000 }, { 0b00000110, 0b00000000 }, { 0b00011001, 0b00000000 }, { 0b00011111, 0b00000000 } }
#define ASPECTS2 { { 0b00011000, 0b00000000 }, { 0b00000100, 0b00000000 }, { 0b00000110, 0b00000000 }, { 0b00011001, 0b00000000 }, { 0b00011111, 0b00000000 } }
#define ASPECTS3 { { 0b00011000, 0b00000000 }, { 0b00000100, 0b00000000 }, { 0b00000110, 0b00000000 }, { 0b00011001, 0b00000000 }, { 0b00011111, 0b00000000 } }
#define ASPECTS4 { { 0b00000010, 0b00000000 }, { 0b00000001, 0b00000000 }, { 0b00000000, 0b00000000 }, { 0b00000000, 0b00000000 }, { 0b00011111, 0b00000000 } }
#define ASPECTS5 { { 0b00000000, 0b00000000 }, { 0b00000000, 0b00000000 }, { 0b00000000, 0b00000000 }, { 0b00000000, 0b00000000 }, { 0b00011111, 0b00000000 } }

#define TARGET0 { { kOnboard, 13 }, { kOnboard, 12 }, { kOnboard, 11 }, { kOnboard, 10 }, { kOnboard,  9 } }
#define TARGET1 { { kOnboard,  8 }, { kOnboard,  7 }, { kOnboard,  6 }, { kOnboard,  5 }, { kOnboard,  4 } }
#define TARGET2 { { kOnboard, 18 }, { kOnboard, 17 }, { kOnboard, 16 }, { kOnboard, 15 }, { kOnboard, 14 } }
#define TARGET3 { { kOnboard, 23 }, { kOnboard, 22 }, { kOnboard, 21 }, { kOnboard, 20 }, { kOnboard, 19 } }
#define TARGET4 { { kNone   ,  0 }, { kNone   ,  0 }, { kNone   ,  0 }, { kOnboard, 28 }, { kOnboard, 27 } }
#define TARGET5 { { kNone   ,  0 }, { kNone   ,  0 }, { kNone   ,  0 }, { kNone   ,  0 }, { kNone   ,  0 } }

#define CHANGEOVERTIME       10
#define CHANGEOVERBLINKTIME  0

#define CAL_SIGNAL_ARRAY \
{ \
  { INPUT0, { ASPECTS0 }, { TARGET0 }, CHANGEOVERTIME, CHANGEOVERBLINKTIME }, \
  { INPUT1, { ASPECTS1 }, { TARGET1 }, CHANGEOVERTIME, CHANGEOVERBLINKTIME }, \
  { INPUT2, { ASPECTS2 }, { TARGET2 }, CHANGEOVERTIME, CHANGEOVERBLINKTIME }, \
  { INPUT3, { ASPECTS3 }, { TARGET3 }, CHANGEOVERTIME, CHANGEOVERBLINKTIME }, \
  { INPUT4, { ASPECTS4 }, { TARGET4 }, CHANGEOVERTIME, CHANGEOVERBLINKTIME }, \
  { INPUT5, { ASPECTS5 }, { TARGET5 }, CHANGEOVERTIME, CHANGEOVERBLINKTIME }, \
}

#define V2P(v) classifier_type::convertInput(v)

#define CLASSIFIER_LO0 { V2P(kRedLo), V2P(kGreenLo), V2P(kYellowLo), V2P(kWhiteLo), V2P(kAdMax) }
#define CLASSIFIER_LO1 { V2P(kRedLo), V2P(kGreenLo), V2P(kYellowLo), V2P(kWhiteLo), V2P(kAdMax) }
#define CLASSIFIER_LO2 { V2P(kRedLo), V2P(kGreenLo), V2P(kYellowLo), V2P(kWhiteLo), V2P(kAdMax) }
#define CLASSIFIER_LO3 { V2P(kRedLo), V2P(kGreenLo), V2P(kYellowLo), V2P(kWhiteLo), V2P(kAdMax) }
#define CLASSIFIER_LO4 { V2P(kRedLo), V2P(kGreenLo), V2P(kAdMax   ), V2P(kAdMax  ), V2P(kAdMax) }
#define CLASSIFIER_LO5 { V2P(kAdMax), V2P(kAdMax  ), V2P(kAdMax   ), V2P(kAdMax  ), V2P(kAdMax) }

#define CLASSIFIER_HI0 { V2P(kRedHi), V2P(kGreenHi), V2P(kYellowHi), V2P(kWhiteHi), V2P(kAdMin) }
#define CLASSIFIER_HI1 { V2P(kRedHi), V2P(kGreenHi), V2P(kYellowHi), V2P(kWhiteHi), V2P(kAdMin) }
#define CLASSIFIER_HI2 { V2P(kRedHi), V2P(kGreenHi), V2P(kYellowHi), V2P(kWhiteHi), V2P(kAdMin) }
#define CLASSIFIER_HI3 { V2P(kRedHi), V2P(kGreenHi), V2P(kYellowHi), V2P(kWhiteHi), V2P(kAdMin) }
#define CLASSIFIER_HI4 { V2P(kRedHi), V2P(kGreenHi), V2P(kAdMin   ), V2P(kAdMin  ), V2P(kAdMin) }
#define CLASSIFIER_HI5 { V2P(kAdMin), V2P(kAdMin  ), V2P(kAdMin   ), V2P(kAdMin  ), V2P(kAdMin) }

#define CLASSIFIER_DEBOUNCE kAdDebounce

#define LIMITS0 { CLASSIFIER_DEBOUNCE, { CLASSIFIER_LO0 }, {CLASSIFIER_HI0 } }
#define LIMITS1 { CLASSIFIER_DEBOUNCE, { CLASSIFIER_LO1 }, {CLASSIFIER_HI1 } }
#define LIMITS2 { CLASSIFIER_DEBOUNCE, { CLASSIFIER_LO2 }, {CLASSIFIER_HI2 } }
#define LIMITS3 { CLASSIFIER_DEBOUNCE, { CLASSIFIER_LO3 }, {CLASSIFIER_HI3 } }
#define LIMITS4 { CLASSIFIER_DEBOUNCE, { CLASSIFIER_LO4 }, {CLASSIFIER_HI4 } }
#define LIMITS5 { CLASSIFIER_DEBOUNCE, { CLASSIFIER_LO5 }, {CLASSIFIER_HI5 } }

#define CLASSIFIER_PIN0 10
#define CLASSIFIER_PIN1 11
#define CLASSIFIER_PIN2 12
#define CLASSIFIER_PIN3 13
#define CLASSIFIER_PIN4 14
#define CLASSIFIER_PIN5 10

#define CLASSIFIER_SINGLE0 { CLASSIFIER_PIN0, LIMITS0 }
#define CLASSIFIER_SINGLE1 { CLASSIFIER_PIN1, LIMITS1 }
#define CLASSIFIER_SINGLE2 { CLASSIFIER_PIN2, LIMITS2 }
#define CLASSIFIER_SINGLE3 { CLASSIFIER_PIN3, LIMITS3 }
#define CLASSIFIER_SINGLE4 { CLASSIFIER_PIN4, LIMITS4 }
#define CLASSIFIER_SINGLE5 { CLASSIFIER_PIN5, LIMITS5 }

#define CAL_INPUT_CLASSIFIER_CFG { { CLASSIFIER_SINGLE0 , CLASSIFIER_SINGLE1, CLASSIFIER_SINGLE2, CLASSIFIER_SINGLE3, CLASSIFIER_SINGLE4, CLASSIFIER_SINGLE5 } }

#define CAL_LEDS_CFG { }