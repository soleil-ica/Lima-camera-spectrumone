#include "SpectrumComms/GpibComms.h"
#include "GpibInterface/GpibInterface.h"

using namespace SpectrumComms;

GpibComms::GpibComms(GpibConfig config):
    m_config(config),
    m_is_initialized(false)
{
    ask_str = ASK_EOI_STR;
}

void GpibComms::disconnect()
{
    YAT_TRACE("GpibComms::disconnect");
    try
    {
        m_is_initialized = false;
        if(m_sock && m_sock->connection_status() ==  yat::ClientSocket::CONNECTED_YES)
        {
            m_sock->disconnect();
            YAT_INFO << "GpibComms: Socket disconnected!" << std::endl;
        }
        else
        {
            YAT_INFO << "GpibComms: Socket already disconnected!" << std::endl;
        }
        yat::Socket::terminate();
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

void GpibComms::make_ack(bool bl)
{
    if(bl)
    {
        write("++read_tmo_ms 100\n");
        ask_str = ASK_RAW_STR;
        //write(ASK_ACK_STR);
    } 
    else
    {
        write("++read_tmo_ms 3000\n");
        ask_str = ASK_EOI_STR;
    }
}

void GpibComms::connect()
{
    YAT_TRACE("GpibComms::connect");

    // OUT_OF_MEMORY Thrown if memory allocation fails.
    m_buffer.capacity(DATA_SIZE);

    try
    {
        m_is_initialized = false;
        m_gpib_interface.reset(new GpibInterfaceLib::GpibInterface(m_config.gpib_address));

        m_is_initialized = true;
    }
    catch(const GpibInterfaceLib::GpibInterfaceException & exception)
    {
        RETHROW_GPIB_EXCEPTION(exception);
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
        //params.push_back("++eot_enable 0\n");
        //params.push_back("++eot_char 13\n");
        params.push_back("++auto 0\n");
        params.push_back("++eos 3\n");
        params.push_back("++eoi 1\n");
        params.push_back("++addr " + TO_STRING(m_config.gpib_address) + "\n");
        params.push_back("++read_tmo_ms 3000\n");
        params.push_back("++ver\n");
        //params.push_back("++read_tmo_ms " + TO_STRING(TIMEOUT>3000 ? 3000 : TIMEOUT) + "\n");

        for(size_t i=0; i<params.size(); i++)
        {
            write(params[i]);
        }

        std::string ver;
        wait_data(TIMEOUT, false);
        read();
        ver.assign(m_buffer.base(), m_buffer.length());

        YAT_INFO << ver; // Get version

        // write("++rst\n");
        // yat::Thread::sleep(8000);


    }
    catch(const yat::Exception & ex)
    {
        RETHROW_EXCEPTION(ex);
    }

}

std::string GpibComms::escape_chars(const std::string& input)
{
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
    if(ask_talk) write(ask_str);
}

void GpibComms::gpib_read(std::string & result, bool ask_talk)
{
    YAT_TRACE("GpibComms::gpib_read");
    if(ask_talk) write(ask_str);
    wait_data(TIMEOUT, false);
    read();
    result.assign(m_buffer.base(), m_buffer.length());
}

void GpibComms::gpib_blocking_read(std::string & result, size_t timeout,
    bool ask_talk)
{
    YAT_TRACE("GpibComms::gpib_blocking_read");
    if(ask_talk) write(ask_str);
    if(wait_data(timeout, true))
    {
        read();
        result.assign(m_buffer.base(), m_buffer.length());
    }
}

void GpibComms::gpib_flush(std::string & result, bool ask_talk)
{
    YAT_TRACE("GpibComms::gpib_flush");
    if(ask_talk)
    {
        write(ask_str);
    }
    wait_data(TIMEOUT, false);
    if(is_to_read())
    {
        read();
        result.assign(m_buffer.base(), m_buffer.length());
    }
}

bool GpibComms::gpib_read_raw(bool ask_talk)
{
    if(ask_talk)
    {
        write(ASK_RAW_STR);
    }
    read();
    return true;
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
        YAT_RESULT << "GpibComms::write string = ["
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

void GpibComms::read()
{
    if(!m_is_initialized)
    {
        THROW_EXCEPTION(
            "NO CONNEXION",
            "Socket is not connected!",
            "GpibComms::read");
    }

    try
    {
        size_t nb_received = m_sock->receive(m_buffer.base(), m_buffer.capacity());
        m_buffer.force_length(nb_received);

        if(nb_received < 50)
        {
            YAT_RESULT << "GpibComms::read answer = ["
            << Utils::make_string_readable(m_buffer.base(), m_buffer.length())
            << "], answer size = "
            << nb_received
            << std::endl;
        }
        else
        {
            // YAT_RESULT << "GpibComms::read answer of size = "
            // << nb_received
            // << std::endl;
        }
        
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