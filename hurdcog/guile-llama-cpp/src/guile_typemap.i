//     Copyright 2024 Li-Cheng (Andy) Tai
//                      atai@atai.org
//
/*
    This file is part of guile_llama_cpp.

    guile_llama_cpp is free software: you can redistribute it and/or modify it
    under the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    guile_llama_cpp is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with guile_llama_cpp. If not, see <https://www.gnu.org/licenses/>.

*/

%include typemaps.i

// in arguments

%typemap(in) int64_t {
    $1 = scm_to_int64($input);
}

%typemap(in) uint64_t {
    $1 = scm_to_uint64($input);
}

%typemap(in) int32_t {
    $1 = scm_to_int32($input);
}

%typemap(in) uint32_t {
    $1 = scm_to_uint32($input);
}

%typemap(in) int16_t {
    $1 = scm_to_int16($input);
}

%typemap(in) uint16_t {
    $1 = scm_to_uint16($input);
}

%typemap(in) int8_t {
    $1 = scm_to_int8($input);
}

%typemap(in) uint8_t {
    $1 = scm_to_uint8($input);
}

%typemap(in) int {
    $1 = scm_to_int($input);
}

%typemap(in) uint {
    $1 = scm_to_uint($input);
}

%typemap(in) size_t {
    $1 = scm_to_size_t($input);
}

%typemap(in) bool {
    $1 = scm_to_bool($input);
}
/*
%typemap(in) float {
    $1 = scm_to_float($input);
}
*/
%typemap(in) double {
    $1 = scm_to_double($input);
}

// out arguments

%typemap(out) int64_t {
    $result = scm_from_int64($1);
}

%typemap(out) uint64_t {
    $result = scm_from_uint64($1);
}

%typemap(out) int32_t {
    $result = scm_from_int32($1);
}

%typemap(out) uint32_t {
    $result = scm_from_uint32($1);
}

%typemap(out) int16_t {
    $result = scm_from_int16($1);
}

%typemap(out) uint16_t {
    $result = scm_from_uint16($1);
}

%typemap(out) int8_t {
    $result = scm_from_int8($1);
}

%typemap(out) uint8_t {
    $result = scm_from_uint8($1);
}

%typemap(out) int {
    $result = scm_from_int($1);
}

%typemap(out) uint {
    $result = scm_from_uint($1);
}

%typemap(out) size_t {
    $result = scm_from_size_t($1);
}

%typemap(out) bool {
    $result = scm_from_bool($1);
}
/*
%typemap(out) float {
    $result = scm_from_float($1);
}
*/
%typemap(out) double {
    $result = scm_from_double($1);
}

// arrays for standard C plain types
%include "carrays.i" // Include SWIG's carrays.i library
%array_class(int8_t, int8_array);
%array_class(uint8_t, uint8_array);
%array_class(int16_t, int16_array);
%array_class(uint16_t, uint16_array);
%array_class(int32_t, int32_array);
%array_class(uint32_t, uint32_array);
%array_class(int64_t, int64_array);
%array_class(uint64_t, uint64_array);
%array_class(size_t, size_t_array);
%array_class(bool, bool_array);
%array_class(float, float_array);
%array_class(double, double_t_array);

%include "std_vector.i"
// Instantiate templates used by example
namespace std {
   %template(IntVector) vector<int>;
   %template(DoubleVector) vector<double>;
}
