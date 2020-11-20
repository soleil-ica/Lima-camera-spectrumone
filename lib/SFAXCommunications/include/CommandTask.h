#ifndef COMMANDTASK_H
#define COMMANDTASK_H

#include <yat/threading/Task.h>
#include <yat/threading/Utilities.h>
#include <yat/utils/String.h>
#include <yat/memory/UniquePtr.h>
#include <yat/utils/Callback.h>
#include <yat/utils/Logging.h>

#include "GpibComms.h"
#include "Commands.hpp"
#include "Utils.h"

//#define CONNECT_SOCKET  (yat::FIRST_USER_MSG + 101)
#define INIT_SEQUENCE   (yat::FIRST_USER_MSG + 102)
#define SET_EXP_TIME    (yat::FIRST_USER_MSG + 103)
#define SET_GAIN        (yat::FIRST_USER_MSG + 104)
#define SNAP            (yat::FIRST_USER_MSG + 105)

namespace SFAXCommunications
{
class GpibComms;

class CommandTask : public yat::Task
{
public:
    enum State {
        Init, Config, Idle, Fault, Busy,
    };

    enum EventType {
        Info, Warning, Error, Fatal,
    };

    struct EventData
    {
        std::string str;
        EventType evt;
    };

    YAT_DEFINE_CALLBACK(EventCallback, EventData);

    CommandTask(GpibComms::GpibConfig config, EventCallback report_event,
        std::string tables_path, std::string expert_config);
    virtual ~CommandTask();
    
    void init_sequence(const bool & force_config = false);
    void set_exp_time(const int & exp_time);
    void set_gain(const int & gain);
    void snap();


    State get_state();


protected:
    virtual void handle_message(yat::Message & msg);

private:
    // Shared:
    yat::Mutex m_data_lock;
    State m_state;

    // Used by thread:
    EventCallback m_report_event;
    std::string m_tables_path;
    std::string m_expert_config;
    yat::UniquePtr<GpibComms> m_comms;


    // CommandFunctions.cpp
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

    void send_table(const std::string & table, unsigned offset);
    static void get_table(const std::string & tab_name, std::vector<std::string> & output_vect);
    void reset(std::string & reply, const size_t & number_retries);
    void config_CCD();
    std::string read_adc_temperature();
    std::vector<std::string> get_config(const std::string & config_str);

    static std::string error_code(int val);

    // CommandMessages.cpp:
    void t_connect_gpib();
    void t_init_sequence(const bool & force_config);
    void t_set_gain(const int & gain);
    void t_set_exp_time(const int & exp_time);
    void t_snap();

    // ReportEvent
    inline void report_event(const std::string & str, EventType evt)
    {
        EventData data;
        data.str = str;
        data.evt = evt;
        m_report_event(data);
    }
    void report_error(const std::string & str)
        {report_event(str, EventType::Error);}
    void report_info(const std::string & str)
        {report_event(str, EventType::Info);}
    void report_fatal(const std::string & str)
        {report_event(str, EventType::Fatal);}

    // State
    void set_state(State state)
    {
        yat::AutoMutex<> guard(m_data_lock);
        m_state = state; 
    }

}; // Class


}
#endif  // COMMANDTASK_H