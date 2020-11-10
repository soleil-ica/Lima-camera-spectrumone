#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <yat/threading/Thread.h>
#include "Utils.h"

#define NO_TERM -1
#define NO_ACK ""
#define NO_DELAY 0

namespace SFAXCommunications
{

    class Command
    {
        public:
        Command(std::string name, std::string str, char term,
        std::string ack, unsigned long delay)
        {
            m_name = name;
            m_str = str;
            m_term = term;
            m_ack = ack;
            m_delay = delay;
        }

        Command(std::string name, std::string str, char term,
        std::string ack)
        {
            m_name = name;
            m_str = str;
            m_term = term;
            m_ack = ack;
            m_delay = NO_DELAY;
        }

        Command(std::string name, std::string str, char term)
        {
            m_name = name;
            m_str = str;
            m_term = term;
            m_ack = NO_ACK;
            m_delay = NO_DELAY;
        }

        Command(std::string name, std::string str)
        {
            m_name = name;
            m_str = str;
            m_term = NO_TERM;
            m_ack = NO_ACK;
            m_delay = NO_DELAY;
        }

        virtual ~Command() {}

        const std::string name() const {return m_name;}

        const std::string str(std::vector<std::string> * args) const 
        {
            std::ostringstream ss;
            size_t i;

            ss << m_str;

            if(args)
            {
                for(i=0; i < args->size(); i++)
                {
                    ss << ',';
                    ss << args->at(i);
                }
            }


            if (m_term != NO_TERM) ss << m_term;

            return ss.str();
        }

        void sleep() const
        {
            if(m_delay != NO_DELAY) yat::Thread::sleep(m_delay);
        }

        bool is_ack(const std::string & resp) const
        {
            return IS_EQUAL(m_ack, resp);
        }

        private:
        unsigned long m_delay;
        char m_term;
        std::string m_str;
        std::string m_ack;
        std::string m_name;
    };

    static const Command WHERE_AM_I("WhereAmI", " ");
    static const Command START_MAIN_PROGRAM("StartMainProgram", "O2000", '\0', "*", 1000);
    static const Command REBOOT_IF_HUNG("RebootIfHung", "\xDE", NO_TERM, NO_ACK, 1000);
    static const Command CCD_INIT("CCDInit", "Z300,0", '\r');
    static const Command WRITE_DATA("WriteData", "Z340,0", '\r', "o");
    static const Command READ_CONFIG("ReadHardwareConfig", "Z310,0", '\r', NO_ACK, 500);
    static const Command READ_TEMP("ReadTemperature", "Z308,0", '\r', NO_ACK, 500);
    static const Command READ_ADC("SetMuxAndReadAdc", "Z345,0", '\r', NO_ACK, 500);
    static const Command GET_GAIN("GetGain", "Z303,0", '\r', NO_ACK, 500);
    static const Command GET_DATA_SIZE("GetDataSize", "Z327,0", '\r', NO_ACK, 500);


} // namespace SFAXCommunications
#endif  // COMMANDS_HPP