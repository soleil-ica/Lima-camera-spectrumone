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