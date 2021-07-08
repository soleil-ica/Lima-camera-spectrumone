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

#ifndef GPIBPROLOGIX_H
#define GPIBPROLOGIX_H



#include <yat/network/ClientSocket.h>
#include <yat/memory/UniquePtr.h>
#include <yat/memory/DataBuffer.h>

#include <yat/utils/String.h>
#include <yat/utils/Logging.h>

#include <iomanip>

#include "Common.h"

#define DATA_SIZE 2048
#define ASK_EOI_STR "++read eoi\n"
#define ASK_ACK_STR "++read 111\n"
#define ASK_RAW_STR "++read\n"
#define TIMEOUT 2000 // Default timeout

#define RETHROW_SOCKET_EXCEPTION(ex)                            \
throw yat::Exception(ex.errors[0].reason, ex.errors[0].desc +   \
sock_status(ex.errors[0].code),                                 \
ex.errors[0].origin, ex.errors[0].code);


namespace SpectrumComms
{

// ============================================================================
//! \class GpibComms
//! \brief Gpib Communication through Prologix Ethernet Controller.
// ============================================================================
class GpibComms
{

public:

    //! \brief Config struct, to give to the constructor.
    struct GpibConfig
    {
        std::string host; // Gpib controler Host (Ip address or hostname)
        size_t      port; // Gpib controler Port
        size_t      gpib_address; // Adress of the device to reach on the Gpib Bus
    };

    //! \brief Constructor
    GpibComms(GpibConfig config);

    //! \brief Destructor
    virtual ~GpibComms() {YAT_LOG("~GpibComms()");}

    //! \brief Disconnect the socket.
    void disconnect();

    //! \brief Connect the ethernet socket and initialize it.
    void connect();

    //! \brief Initialize the Gpib controller.
    void gpib_init();

    //! \brief Gpib write command.
    //! \param argin String to write.
    //! \param ask_talk If the controller should ask the Gpib device to talk afterwards.
    //! \throw yat::Exception if the command fails
    void gpib_write(const std::string & argin, bool ask_talk=true);

    //! \brief Gpib read command.
    //! \param[out] result String for the result.
    //! \param ask_talk If the controller should ask the Gpib device to talk before reading.
    //! \throw yat::Exception if the command fails, or nothing to read.
    void gpib_read(std::string & result, bool ask_talk=false);

    //! \brief Gpib blocking read command.
    //! Wait for something to be read for timeout.
    //! \param[out] result String for the result.
    //! \param timeout Timeout to wait for (ms).
    //! \param ask_talk If the controller should ask the Gpib device to talk before reading.
    //! \throw yat::Exception if the command fails or timeout expire.
    void gpib_blocking_read(std::string & result, size_t timeout,
        bool ask_talk=false);

    //! \brief Gpib flush command. Try to read, if nothing to be read, ignores.
    //! \param[out] result String for the result.
    //! \param ask_talk If the controller should ask the Gpib device to talk before reading.
    void gpib_flush(std::string & result, bool ask_talk=false);

    //! \brief Gpib read raw. Use this to read raw data, stores the data in the internal buffer.
    //! \param ask_talk If the controller should ask the Gpib device to talk before reading.
    //! \throw yat::Exception if the command fails or nothing to read.
    bool gpib_read_raw(bool ask_talk=false);

    //! \brief Change the read request string.
    void make_ack(bool bl);

    //! \brief Returns the internal buffer object.
    yat::Buffer<char>& getBuffer() {return m_buffer;}

private:

    bool m_is_initialized;
    std::string ask_str;
    GpibConfig m_config;
    yat::UniquePtr<yat::ClientSocket> m_sock;
    yat::Buffer<char> m_buffer;

    bool wait_data(size_t timeout, bool throw_exception);
    bool is_to_read();

    void read();
    void write(const std::string & dta);

    static std::string sock_status(int val);
    static std::string escape_chars(const std::string& input);

    
};  // Class

}   // namespace SpectrumComms

#endif  // GPIBPROLOGIX_H