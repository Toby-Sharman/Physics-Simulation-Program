//
// Physics-Simulation-program
// File: unit_utilities.h
// Created by Tobias Sharman on 11/09/2025
//
// Description:
//   - Contains functions to parse units in the form of strings into their base dimensions and a factor to multiply by
//     to get to SI
//         -> parseUnits is the only function meant to be used in other files with the others just meant as helpers
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_UNIT_UTILITIES_H
#define PHYSICS_SIMULATION_PROGRAM_UNIT_UTILITIES_H

#include <string_view>
#include <utility>

#include "core/maths/utilities/units.h"

// decodeSuperscript
//
// Splits a unit string (can include the prefix) into its base unit (can include prefix) and exponent
//
// Notes on algorithms:
//   - Moves a pointer along the string until the character at that index is either a '^', '-', or numeric (0-9) then
//     breaks the loop
//         -> TODO: Add support for unicode style superscripts nicely (was in before but taken away as not needed and made logic less legible)
//         -> In the case of a '^' the contents of the string before it are returned as the base unit and after it are
//            converted to an int and returned as the exponent
//         -> For '-' and numeric characters the contents of the string before it are returned as the bae unit and
//            from that characters index the rest of string after it are returned as the exponent
//   - Returning exponent as an int as converting to int8_t or some other smaller data structure would require recasting
//     or a custom string to int8_t function //TODO
//
// Notes on output:
//   - If the input is empty returns the empty input and an exponent of 0
//   - If no exponent is found in the input the original input is returned as the base unit and the exponent is set to 1
//   - If there is a present exponent it is returned with the sign as part of the number
//
// Parameters:
//   unit - an immutable string of characters
//
// Returns:
//   - pair<string_view,int> - first = base unit, second = exponent
//
// Example usage:
//   std::string_view unit = "mm^3"
//   auto [baseUnit, exponent] = extractSuperscript(unit);
[[nodiscard]] std::pair<std::string_view, int> extractSuperscript(std::string_view unit) noexcept;

// extractPrefix
//
// Attempts to match an input string against standard SI prefixes (see prefixes in units.h) followed by a valid unit
// symbol (see unitTable in units.h)
//
// Notes on input:
//   - The input must only contain the relevant part of the unit with a prefix and base unit otherwise it won't be able
//     to make a match, i.e. no exponent (so run extractSuperscript first) or even just whitespace
//   - Supports multicharacter prefixes and units, whether that be multiple ASCII characters or Unicode
//
// Notes on algorithms:
//   - Iterates through every prefix looking for a valid prefix, base unit pair and if one is found returns that
//     combination as a net factor/scale of the prefix and factor to SI (e.g. eV = 1.602...) and an SI dimensional
//     representation
//         -> This means that care has to be taken when altering either table such that there is not the possibility for
//            multiple valid returns for a given input
//   - If there is no valid combination of prefix and base unit the unit table is searched for the unit in a prefixless
//     form and then mapped to an SI dimensional representation
//
// Notes on output:
//   - The unit symbol is not preserved in the return of the function, instead being directly mapped to its factor to SI
//     and its SI dimensions
//
// Parameters:
//   unit - an immutable string of characters
//
// Returns:
//   - UnitInfo - a factor relative to SI and a representation of the 7 base SI dimensions as an array
//
// Throws:
//   - If there is neither a combination of prefix and base unit or standalone base unit for the tables an invalid
//     argument error is thrown
//
// Example usage:
//   std::string_view baseUnit = "keV"
//   auto [scale, siUnit] = extractPrefix(baseUnit);
[[nodiscard]] UnitInfo extractPrefix(std::string_view unit);

// parseUnit
//
// Takes an input string, described by start and end pointers and turns it into relative factor to SI and an SI
// dimensional representation of unit
//
// Notes on input:
//   - Since not meant to be an entry point into the parsing functions by others input is kept in the most efficient
//     pass through from parseUnits
//         -> As an added bonus it is also a different data entry type so IDEs can help flag a usage of the wrong
//            function
//
// Notes on algorithms:
//   - First cleans the input of any trailing or leading spaces then copies anything that isn't a space, those being
//     internal, to a new string
//         -> parseUnits cleans units of spaces pretty well as is but this gives option for direct use of parseUnit in
//            testing and makes the unit parsing a bit more robust
//         -> Does only one pass over the entire string
//         -> Could have added a check for if there is any internal spaces and if so just write straight to a
//            string_view rather the copying each character to a string, but implementation of this would either require
//            tracking of at least two additional variables and the inputs are meant to be very small so there would be
//            no reasonable gain in proper usage
//
// Notes on output:
//   - Not in the form that units will be propagated through the program as the factor/scale will be merged into the
//     unit value
//
// Parameters:
//   - tokenStart - pointer to the start of a string
//   - tokenEnd - pointer to the end of a string
//
// Returns:
//   - UnitInfo - a factor relative to SI and a representation of the 7 base SI dimensions as an array
//
// Example usage:
//   const char* p = units.data();
//   ...
//   const char* tokenStart = p;
//   ...
//   const char* tokenEnd = p;
//   auto [factor, siUnit] = parseUnit(tokenStart, tokenEnd);
[[nodiscard]] UnitInfo parseUnit(const char* tokenStart, const char* tokenEnd);

// parseUnits
//
// Takes a string of units and parses it into a factor/scale relative to SI and its base SI dimension
//
// Notes on input:
//   - Units with prefixes must have the prefix attached to the alphabetic part of the string else it would be
//     impossible to identify that it is reasonably attached to the base unit (sans prefix)
//   - Spaces between units are taken as implicit multiplication and '*' and '/' are treated as their standard
//     operations
//   - Spaces between exponent start operators like '^' or directly starting one with a '-' or numeric type have
//     handling, but are not recommended
//   - There is no handling for units that have no place being in a unit string like a '.', ',', '(', or ')'
//
// Notes on algorithms:
//   - Moves along the input string searching for the start of a unit then splits that off into a token
//   - If it comes across a '/' it marks the next token for division
//   - There is no handling for any brackets in the units and no error will be thrown so if it is wanted to add this
//     functionality it would be necessary to implement a recursive loop, furthermore for brackets with an exponent the
//     extractSuperscript has no handling for a non string input so a new function would be required to identify
//     shared exponents for a series of units and update them accordingly
//
// Notes on output:
//   - Not in the form that units will be propagated through the program as the factor/scale will be merged into the
//     unit value
//
// Parameters:
//   - unit - an immutable string of characters
//
// Returns:
//   - UnitInfo - a factor relative to SI and a representation of the 7 base SI dimensions as an array
//
// Example usage:
//   std::string_view unit = "Mg km/ms^-2"
//   auto [factor, siUnit] = parseUnit(tokenStart, tokenEnd); // factor = 1 siUnit = {1, 1, -2, 0, 0, 0, 0}
[[nodiscard]] UnitInfo parseUnits(std::string_view units);

#endif // PHYSICS_SIMULATION_PROGRAM_UNIT_UTILITIES_H