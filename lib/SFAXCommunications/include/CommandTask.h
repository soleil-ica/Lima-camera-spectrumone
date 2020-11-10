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

#define CONNECT_SOCKET  (yat::FIRST_USER_MSG + 101)
#define INIT_SEQUENCE   (yat::FIRST_USER_MSG + 102)

#define STATE(state)                        \
{                                           \
    yat::AutoMutex<> guard(m_data_lock);    \
    m_state = state;                        \
}

namespace SFAXCommunications
{
class GpibComms;

class CommandTask : public yat::Task
{
public:
    enum State {
        Init, Idle, Fault,
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
        std::string tables_path);
    virtual ~CommandTask() {}

    

    void disconnect();
    void connect();
    void init_sequence(bool force_config = false);

    void gpib_write(std::string argin) {m_comms->gpib_write(argin);}
    const std::string gpib_read() {return m_comms->gpib_read();}


    State get_state();


protected:
    virtual void handle_message(yat::Message & msg);

private:
    yat::Mutex m_data_lock;
    EventCallback m_report_event;
    State m_state;
    std::string m_tables_path;
    yat::UniquePtr<GpibComms> m_comms;


    // CommandFunctions.cpp
    std::string command_and_wait(const Command & cmd, size_t timeout,
        std::vector<std::string> * args = 0);
    std::string command_and_read(const Command & cmd,
        std::vector<std::string> * args = 0);
    std::string command_and_flush(const Command & cmd,
        std::vector<std::string> * args = 0);


    void send_table(const std::string & table, unsigned offset);
    static void get_table(const std::string & tab_name, std::vector<std::string> & output_vect);
    std::string reset(size_t number_retries);
    void config_CCD();
    std::string read_adc_temperature();

    

    static std::string error_code(int val);

    // CommandMessages.cpp:
    void t_connect_gpib();
    void t_init_sequence(bool force_config);

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

}; // Class


}
#endif  // COMMANDTASK_H