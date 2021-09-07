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

#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <yat/threading/Thread.h>
#include "SpectrumComms/Common.h"

#define NO_TERM -1
#define NO_ACK ""
#define NO_DELAY 0

namespace SpectrumComms
{

    // ============================================================================
    //! \class Command
    //! \brief Class to hold all possible commands for SpectrumOne communication
    // ============================================================================
    class Command
    {
        public:
        Command(std::string name, std::string str, char term = NO_TERM, std::string ack = NO_ACK, unsigned long delay = NO_DELAY) :
        m_name(name),
        m_str(str),
        m_term(term),
        m_ack(ack),
        m_delay(delay)
        {
        }

        virtual ~Command() {}

        const std::string name() const {return m_name;}

        //! \brief Get the command as a string.
        //! \param args Vector of strings to add as parameters.
        const std::string str(std::vector<std::string> * args) const 
        {
            std::ostringstream ss;
            size_t i;

            ss << m_str;

            // Add all args
            if(args)
            {
                for(i=0; i < args->size(); i++)
                {
                    ss << ',';
                    ss << args->at(i);
                }
            }

            if (m_term != NO_TERM) ss << m_term;

            // Return the final string
            return ss.str();
        }

        void sleep() const
        {
            // Added sleep for specific commands
            if(m_delay != NO_DELAY) yat::Thread::sleep(m_delay);
        }

        bool is_ack(const std::string & resp) const
        {
            return IS_EQUAL(m_ack, NO_ACK) ? true : IS_EQUAL(m_ack, resp);
        }

        // Return expected reply size
        int expected_reply() const
        {
            if(IS_EQUAL(m_ack, NO_ACK)) return 0;
            else return m_ack.size();
        }

        private:
        unsigned long m_delay;
        char m_term;
        std::string m_str;
        std::string m_ack;
        std::string m_name;
    };

    // All the commands that can be used, cf manual:
    static const Command CCD_WHERE_AM_I("WhereAmI", " ");
    static const Command CCD_START_MAIN_PROGRAM("StartMainProgram", "O2000", '\0', "*");
    static const Command CCD_REBOOT_IF_HUNG("RebootIfHung", "\xDE");
    static const Command CCD_INIT("Init", "Z300,0", '\r');
    static const Command CCD_WRITE_DATA("WriteData", "Z340,0", '\r', "o");
    static const Command CCD_READ_CONFIG("ReadHardwareConfig", "Z310,0", '\r');
    static const Command CCD_WRITE_CONFIG("WriteHardwareConfig", "Z328,0", '\r', "o");
    static const Command CCD_READ_TEMP("ReadTemperature", "Z308,0", '\r');
    static const Command CCD_READ_ADC("SetMuxAndReadAdc", "Z345,0", '\r');
    static const Command CCD_READ_GAIN("ReadGain", "Z303,0", '\r');
    static const Command CCD_SET_GAIN("SetGain", "Z302,0", '\r', "o");
    static const Command CCD_SET_EXP_TIME("SetExpTime", "Z301,0", '\r', "o");
    static const Command CCD_GET_DATA_SIZE("GetDataSize", "Z327,0", '\r');
    static const Command CCD_START("Start", "Z311,0", '\r', "o");
    static const Command CCD_READ_IMAGE("ReadImage", "Z315,0", '\r', "o");
    static const Command CCD_RESET_IMAGE("ResetImage", "Z317,0", '\r', "o");
    static const Command CCD_SET_SHUTTER("SetShutter", "Z320,0", '\r', "o");
    static const Command CCD_STOP_ACQ("StopAcquisition", "Z314,0", '\r', "o");
    static const Command CCD_STATUS("GetStatus", "Z312,0", '\r');
    static const Command CCD_SET_FLUSHES("SetFlushesNumber", "Z305,0", '\r', "o");
    static const Command CCD_DEFINE_FORMAT("DefineAcqFormat", "Z325,0", '\r', "o");
    static const Command CCD_DEFINE_AREA("DefineArea", "Z326,0", '\r', "o");

    // We don't know what these commands do
    static const Command CCD_UNKNOWN_1("Unknown1", "Z330,0,0", '\r', "o");
    static const Command CCD_UNKNOWN_2("Unknown2", "Z348,0,0", '\r', "o");
    static const Command CCD_UNKNOWN_3("Unknown3", "Z352,0,1", '\r');

} // namespace SpectrumComms
#endif  // COMMANDS_HPP