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

#ifndef COMMANDINTERFACE_H
#define COMMANDINTERFACE_H

#include <yat/threading/Task.h>
#include <yat/threading/Utilities.h>
#include <yat/utils/String.h>
#include <yat/memory/UniquePtr.h>
#include <yat/utils/Callback.h>
#include <yat/utils/Logging.h>

#include "GpibComms.h"
#include "Common.h"
#include "Commands.hpp"

namespace SpectrumComms
{
class GpibComms;

class CommandInterface
{
public:
    CommandInterface(GpibComms::GpibConfig gpib_config, Listener* listener);
    virtual ~CommandInterface();

    void report_error(const std::string & str)
        {report_event(str, EventType::Error);}
    void report_info(const std::string & str)
        {report_event(str, EventType::Info);}
    void report_fatal(const std::string & str)
        {report_event(str, EventType::Fatal);}

    std::string command_and_wait(const Command & cmd, size_t timeout,
        std::vector<std::string> * args, bool ack = false);
    std::string command_and_wait(const Command & cmd, size_t timeout, bool ack = false)
        {return command_and_wait(cmd, timeout, 0, ack);}
    std::string command_and_read(const Command & cmd,
        std::vector<std::string> * args, bool ack = false);
    std::string command_and_read(const Command & cmd, bool ack = false)
        {return command_and_read(cmd, 0, ack);}
    std::string command_and_flush(const Command & cmd,
        std::vector<std::string> * args, bool ack = false);
    std::string command_and_flush(const Command & cmd, bool ack = false)
        {return command_and_flush(cmd, 0, ack);}

    GpibComms m_comms;

private:
    Listener* m_listener;

    static std::string error_code(int val);

    // ReportEvent
    inline void report_event(const std::string & str, EventType evt)
    {
        if(m_listener) m_listener->on_new_event(str, evt);
    }
}; // Class


}
#endif  // COMMANDINTERFACE_H