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

#include "SpectrumComms/CommandInterface.h"

using namespace SpectrumComms;


CommandInterface::CommandInterface(GpibComms::GpibConfig gpib_config, Listener* listener) :
    m_comms(gpib_config),
    m_listener(listener)
{
    YAT_TRACE("CommandInterface::CommandInterface");
}

CommandInterface::~CommandInterface()
{
    YAT_TRACE("CommandInterface::~CommandInterface");
}

std::string CommandInterface::command_and_read(const Command & cmd,
    std::vector<std::string> * args, bool ack)
{
    YAT_INFO << "CommandInterface::command_and_read: " << cmd.name() << std::endl;
    try
    {
        std::string result;
        m_comms.write_and_read(cmd.str(args), result, cmd.expected_reply());
        if(ack)
        {
            if(!cmd.is_ack(result))
            {
                THROW_EXCEPTION(
                    "NOT ACKNOWLEDGED",
                    "Received: " + Utils::make_string_readable(result) +
                    " for command " + cmd.name(),
                    "CommandInterface::command_and_read");
            }
        }
        return result;
    }
    catch(const yat::Exception & ex)
    {
        report_error("CommandInterface::command_and_read " + cmd.name() + " failed!\n");
        RETHROW_EXCEPTION(ex);
    }
}


std::string CommandInterface::error_code(int val)
{
    switch (val)
    {
    case 1:
    return "Hardware problem";

    case 2:
    return "Not available";

    case 3:
    return "Parameter problem";

    case 4:
    return "Not initialized";

    case 20:
    return "CCD: NULL user pointer";

    case 21:
    return "CCD: Not enough memory";

    case 22:
    return "CCD: Altparam";

    case 23:
    return "CCD: Load";

    case 24:
    return "CCD: Read program";

    case 25:
    return "CCD: Timeout";

    case 26:
    return"CCD: Zeroloop";

    case 30:
    return "PDA: Multiscan error";

    case 31:
    return "Remote: Not enough memory";

    case 32:
    return "Remote: No data available";

    case 33:
    return "Remote: Binary transfer error";

    case 34:
    return "Remote: Illegal call sequence";

    default:
    return "Unknown error";
    }
}