#include <GpibComms.h>

using namespace SFAXCommunications;

GpibComms::GpibComms(GpibConfig config):
    m_config(config),
    m_is_initialized(false)
{
}

void GpibComms::disconnect()
{
    YAT_TRACE("GpibComms::disconnect");
    try
    {
        m_is_initialized = false;
        bool bl = m_sock->connection_status() ==  yat::ClientSocket::CONNECTED_YES;
        if(bl && m_sock)
        {
            m_sock->disconnect();
            yat::Socket::terminate();
            YAT_INFO << "GpibComms: Socket disconnected!" << std::endl;
        }
        else
        {
            YAT_INFO << "GpibComms: Socket already disconnected!" << std::endl;
        }
    }
    catch(const yat::SocketException & se)
    {
        RETHROW_SOCKET_EXCEPTION(se);
    }
    catch(...)
    {
        THROW_EXCEPTION(
            "UNKNOWN ERROR!",
            "Unknown error during socket disconnection!",
            "GpibComms::disconnect");
    }
}


void GpibComms::connect()
{
    YAT_TRACE("GpibComms::connect");
    try
    {
        yat::Socket::init();
        m_sock.reset(new yat::ClientSocket(yat::Socket::TCP_PROTOCOL));

        m_sock->set_option(yat::Socket::SOCK_OPT_IBUFFER_SIZE, DATA_SIZE);
        m_sock->set_option(yat::Socket::SOCK_OPT_OBUFFER_SIZE, DATA_SIZE);
        m_sock->set_option(yat::Socket::SOCK_OPT_NO_DELAY, 1);
        m_sock->set_option(yat::Socket::SOCK_OPT_OTIMEOUT, 20);
        m_sock->set_option(yat::Socket::SOCK_OPT_ITIMEOUT, 20);
        m_sock->set_option(yat::Socket::SOCK_OPT_REUSE_ADDRESS, 0);

        yat::Address addr(m_config.host, m_config.port);
        m_sock->connect(addr);
        m_sock->set_blocking_mode();
        m_is_initialized = true;
    }
    catch(const yat::SocketException & se)
    {
        RETHROW_SOCKET_EXCEPTION(se);
    }
    catch(...)
    {
        THROW_EXCEPTION(
            "UNKNOWN ERROR!",
            "Unknown error during socket connection!",
            "GpibComms::connect");
    }
}

void GpibComms::gpib_init()
{
    YAT_TRACE("GpibComms::gpib_init");
    try
    {
        std::vector<std::string> params;

        params.push_back("++mode 1\n");
        params.push_back("++auto 0\n");
        params.push_back("++eos 3\n");
        params.push_back("++eoi 1\n");
        params.push_back("++read_tmo_ms " + TO_STRING(m_config.timeout>3000 ? 3000 : m_config.timeout) + "\n");
        params.push_back("++addr " + TO_STRING(m_config.gpib_address) + "\n");
        params.push_back("++ver\n");

        for(size_t i=0; i<params.size(); i++)
        {
            write(params[i]);
        }

        YAT_INFO << read(); // Get version
    }
    catch(const yat::Exception & ex)
    {
        RETHROW_EXCEPTION(ex);
    }

}

std::string GpibComms::escape_chars(const std::string& input)
{
    YAT_TRACE("GpibComms::escape_chars");
    std::string output;
    unsigned char c;

    output.reserve(input.size() + 5);

    for (size_t i; i < input.size(); i++)
    {
        c = input[i];
        switch(c)
        {
            case '\n':
            case '\r':
            case '+':
            case 27:
            output.push_back(27);

            default:
            output.push_back(c);
        }
    }

    output.push_back('\n');

    return output;
}

void GpibComms::gpib_write(const std::string & argin, bool ask_talk)
{
    YAT_TRACE("GpibComms::gpib_write");
    write(escape_chars(argin));
    if(ask_talk) write(ASK_TALK_STR);
}

std::string GpibComms::gpib_read(bool ask_talk)
{
    YAT_TRACE("GpibComms::gpib_read");
    if(ask_talk) write(ASK_TALK_STR);
    wait_data(m_config.timeout, false);
    return read();
}

std::string GpibComms::gpib_blocking_read(size_t timeout, bool ask_talk,
    bool throw_exception)
{
    YAT_TRACE("GpibComms::gpib_blocking_read");
    if(ask_talk) write(ASK_TALK_STR);
    if(wait_data(timeout, throw_exception))
    {
        return read();
    }
    return "";
}

std::string GpibComms::gpib_flush(bool ask_talk)
{
    YAT_TRACE("GpibComms::gpib_flush");
    if(ask_talk)
    {
        write(ASK_TALK_STR);
    }
    wait_data(m_config.timeout, false);
    if(is_to_read())
    {
        return read();
    }
    return "";
}

bool GpibComms::wait_data(size_t timeout, bool throw_exception)
{
    try
    {
        return m_sock->wait_input_data(timeout, throw_exception);
    }
    catch(const yat::SocketException & se)
    {
        RETHROW_SOCKET_EXCEPTION(se);
    }
    catch(...)
    {
        THROW_EXCEPTION(
            "UNKNOWN ERROR",
            "Unknown error in socket",
            "GpibComms::wait_data");
    }
}

bool GpibComms::is_to_read()
{
    try
    {
        return m_sock->can_read_without_blocking();
    }
    catch(const yat::SocketException & se)
    {
        RETHROW_SOCKET_EXCEPTION(se);
    }
    catch(...)
    {
        THROW_EXCEPTION(
            "UNKNOWN ERROR",
            "Unknown error in socket",
            "GpibComms::is_to_read");
    }
}

void GpibComms::write(const std::string & argin)
{
    YAT_TRACE("GpibComms::write");
    if(!m_is_initialized)
    {
        THROW_EXCEPTION(
            "NO CONNEXION",
            "Socket is not connected!",
            "GpibComms::write");
    }
    if(argin.size() == 0)
    {
        THROW_EXCEPTION(
            "OUT OF RANGE",
            "Input argument is empty!",
            "GpibComms::write");
    }
    try
    {
        m_sock->send(argin);
        YAT_INFO << "GpibComms::write string = ["
            << Utils::make_string_readable(argin)
            << "], hex = "
            << Utils::string_to_hex_digits(argin)
            << std::endl;
    }
    catch(const yat::SocketException & se)
    {
        RETHROW_SOCKET_EXCEPTION(se);
    }
    catch(...)
    {
        THROW_EXCEPTION(
            "UNKNOWN ERROR",
            "Unknown error during socket send!",
            "GpibComms::write");
    }
}

std::string GpibComms::read()
{
    YAT_TRACE("GpibComms::read");
    if(!m_is_initialized)
    {
        THROW_EXCEPTION(
            "NO CONNEXION",
            "Socket is not connected!",
            "GpibComms::read");
    }

    std::string answer;

    yat::Socket::Data data(DATA_SIZE, true);

    try
    {
        long number = m_sock->receive(data);
        answer = data.base();
        YAT_INFO << "GpibComms::read answer = ["
            << Utils::make_string_readable(answer)
            << "], answer size = "
            << answer.size()
            << std::endl;
    }
    catch(const yat::SocketException & se)
    {
        RETHROW_SOCKET_EXCEPTION(se);
    }
    catch(...)
    {
        THROW_EXCEPTION(
            "UNKNOWN ERROR",
            "Unknown error during socket receive!",
            "GpibComms::read");
    }
    return answer;
}


std::string GpibComms::sock_status(int val)
{
    switch (val)
    {
    case yat::SoErr_NoError:
        return ", No Error";
        
    case yat::SoErr_BadMemAddress:
        return ", The receive buffer pointer(s) point outside the processes address space";
        
    case yat::SoErr_AddressInUse:
        return ", Address is already in use (bind & connect)";
        
    case yat::SoErr_AddressNotAvailable:
        return ", Address not available on machine (bind & connect)";
        
    case yat::SoErr_BadDescriptor:
        return ", Invalid socket descriptor (socket)";
        
    case yat::SoErr_BadMessage:
        return ", Message signature is invalid";
        
    case yat::SoErr_ConnectionClosed:
        return ", Connection was closed (or broken) by other party";
        
    case yat::SoErr_ConnectionRefused:
        return ", Connection refused by server";
        
    case yat::SoErr_DatagramTooLong:
        return ", Datagram too long to send atomically";
        
    case yat::SoErr_InvalidOption:
        return ", Invalid option for socket protocol";
        
    case yat::SoErr_IsConnected:
        return ", Socket is already connected";
        
    case yat::SoErr_NotConnected:
        return ", Socket is not connected";
        
    case yat::SoErr_OpNotSupported:
        return ", Operation is not supported for this socket";
        
    case yat::SoErr_PrivilegedPort:
        return ", User does not have acces to privileged ports (bind)";
        
    case yat::SoErr_TimeOut:
        return ", Time out was reached for operation (receive & send)";
        
    case yat::SoErr_WouldBlock:
        return ", Current operation is blocking (non-blocking socket)";
        
    case yat::SoErr_InProgress:
        return ", Op. in progress";
        
    case yat::SoErr_OSInterrupt:
        return ", Op. interrupted by OS event (signal)";
        
    case yat::SoErr_OutOfMemory:
        return ", Memory allocation failed.";
        
    case yat::SoErr_Other:
        return ", Any other OS specific error";
        
    default:
        return "";

    }
}