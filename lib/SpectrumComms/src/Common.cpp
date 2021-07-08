//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2021
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################

#include "Common.h"

using namespace SpectrumComms;


void Utils::char_to_hex_digits(const unsigned char & c, std::string & str)
{
    static const unsigned char hex_digits[] = "0123456789ABCDEF";

    str.push_back(hex_digits[c >> 4]);
    str.push_back(hex_digits[c & 15]);
}

unsigned char Utils::hex_digit_to_char(const unsigned char & c)
{
    if ('0' <= c && c <= '9') { return c      - '0'; }
    if ('a' <= c && c <= 'f') { return c + 10 - 'a'; }
    if ('A' <= c && c <= 'F') { return c + 10 - 'A'; }
    THROW_EXCEPTION(
        "INVALID CHAR",
        "Couldn't convert symbol to HEX",
        "Utils::hex_digit_to_char");
}

std::string Utils::string_to_hex_digits(const std::string& input)
{
    std::string output;
    unsigned char c;

    output.reserve(input.size() * 3 - 1);

    for (size_t i; i < input.size()-1; i++)
    {
        char_to_hex_digits(input[i], output);
        output.push_back(' ');
    }

    char_to_hex_digits(input[input.size()-1], output);
    return output;
}

std::string Utils::hex_digits_to_string(const std::string & input)
{
    std::string result;
    unsigned char n;
    result.reserve(input.size() / 2);

    for (std::size_t i = 0; i < input.size() / 2; ++i)
    {
        n =   hex_digit_to_char(input[2 * i]) * 16 +
                            hex_digit_to_char(input[2 * i + 1]);
        result.push_back(n);
    }

    return result;
}

std::string Utils::make_string_readable(const std::string & input)
{
    return make_string_readable(input.c_str(), input.size());
}

std::string Utils::make_string_readable(const char* dta, size_t nb)
{
    std::string output;
    char c;

    output.reserve(nb + 5);

    for (size_t i; i < nb; i++)
    {
        c = dta[i];
        if(c >= 32 && c <= 126)
        {
            output.push_back(c);
        }
        else
        {
            switch(c)
            {
                case '\n':
                output.push_back('\\');
                output.push_back('n');
                break;

                case '\t':
                output.push_back('\\');
                output.push_back('t');
                break;

                case '\v':
                output.push_back('\\');
                output.push_back('v');
                break;

                case '\b':
                output.push_back('\\');
                output.push_back('b');
                break;

                case '\r':
                output.push_back('\\');
                output.push_back('r');
                break;

                case '\f':
                output.push_back('\\');
                output.push_back('f');
                break;

                case '\a':
                output.push_back('\\');
                output.push_back('a');
                break;

                case '\0':
                output.push_back('\\');
                output.push_back('0');
                break;

                default:
                output.push_back('\\');
                output.push_back('x');
                char_to_hex_digits(c, output);
            }
        }
    }
    return output;
}


