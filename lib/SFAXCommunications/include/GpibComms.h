#ifndef GPIBPROLOGIX_H
#define GPIBPROLOGIX_H

#include <yat/network/ClientSocket.h>
#include <yat/memory/UniquePtr.h>

#include <yat/utils/String.h>
#include <yat/utils/Logging.h>

#include <iomanip>

#include "Utils.h"

#define DATA_SIZE 65536
#define ASK_TALK_STR "++read eoi\n"

#define RETHROW_SOCKET_EXCEPTION(ex)                            \
throw yat::Exception(ex.errors[0].reason, ex.errors[0].desc +   \
sock_status(ex.errors[0].code),                                 \
ex.errors[0].origin, ex.errors[0].code);


namespace SFAXCommunications
{

class GpibComms
{

public:

    struct GpibConfig
    {
        std::string     host;
        unsigned long   port;
        unsigned long   gpib_address;
        unsigned long   timeout;
    };
    
    GpibComms(GpibConfig config);
    virtual ~GpibComms() {};

    void disconnect();
    void connect();

    void gpib_init();
    void gpib_write(const std::string & argin, bool ask_talk=true);
    std::string gpib_read(bool ask_talk=false);
    std::string gpib_blocking_read(size_t timeout, bool ask_talk=false,
        bool throw_exception=true);
    std::string gpib_flush(bool ask_talk=false);
    

private:

    bool m_is_initialized;
    GpibConfig m_config;
    yat::UniquePtr<yat::ClientSocket> m_sock;

    bool wait_data(size_t timeout, bool throw_exception);
    bool is_to_read();

    std::string read();
    void write(const std::string & dta);

    static std::string sock_status(int val);
    static std::string escape_chars(const std::string& input);

    
};  // Class

}   // namespace SFAXCommunications

#endif  // GPIBPROLOGIX_H