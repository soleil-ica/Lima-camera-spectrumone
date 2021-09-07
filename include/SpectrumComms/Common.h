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

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <yat/Exception.h>
#include <yat/file/FileName.h>

#define RETHROW_EXCEPTION(ex) \
throw yat::Exception(ex.errors[0].reason, ex.errors[0].desc, \
ex.errors[0].origin, ex.errors[0].code);

#define THROW_EXCEPTION(reason, desc, origin) \
throw yat::Exception(reason, desc, origin);

#define IS_EQUAL(a, b)  \
yat::StringUtil::is_equal_no_case(a, b)

#define IS_NOT_EQUAL(a, b)  \
!yat::StringUtil::is_equal_no_case(a, b)

#define TO_STRING(arg)  \
yat::StringUtil::to_string(arg)

#define DLLIMPORT   __declspec( dllimport )
#define DLLEXPORT   __declspec( dllexport )

namespace SpectrumComms
{

enum EventType {
    Info, Warning, Error, Fatal,
};

class Listener
{
public:
    virtual void on_buffer_filled() = 0;
    virtual void on_new_event(const std::string & str, EventType evt) = 0;
    virtual void temperature_callback(const double & temperature) = 0;
    virtual void gain_callback(const long & gain) = 0;
    virtual void num_flushes_callback(const int & num_flushes) = 0;
    virtual void shutter_callback(const bool & shutter) = 0;
    ~Listener(){}
protected:
    Listener(){}
};

class Utils
{
    public:
    static std::string string_to_hex_digits(const std::string & input);
    static std::string hex_digits_to_string(const std::string & input);
    static std::string make_string_readable(const std::string & input);
    static std::string make_string_readable(const char* dta, size_t nb);
    static unsigned char hex_digit_to_char(const unsigned char & c);
    static void char_to_hex_digits(const unsigned char & c, std::string & str);

    private:
    Utils();
    ~Utils();
};

}   // namespace SpectrumComms

#endif  // GPIBPROLOGIX_H