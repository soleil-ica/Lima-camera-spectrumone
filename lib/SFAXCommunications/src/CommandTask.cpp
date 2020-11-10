#include "CommandTask.h"

using namespace SFAXCommunications;

CommandTask::CommandTask(GpibComms::GpibConfig config, EventCallback report_event, std::string tables_path):
    m_report_event(report_event),
    m_state(State::Init),
    m_tables_path(tables_path)
{
    YAT_INFO << "command task!" << std::endl;

    m_comms.reset(new GpibComms(config));
    go(config.timeout+1000);
}

void CommandTask::connect()
{
    YAT_TRACE("CommandTask::connect");

    yat::Message* msg = yat::Message::allocate(CONNECT_SOCKET,INIT_MSG_PRIORITY, true);
    this->post(msg);
}

void CommandTask::init_sequence(bool force_config)
{
    YAT_TRACE("CommandTask::init_sequence");

    yat::Message* msg = yat::Message::allocate(INIT_SEQUENCE, DEFAULT_MSG_PRIORITY, true);
    msg->attach_data(force_config);
    this->post(msg);
}


void CommandTask::disconnect()
{
    exit();
    m_comms->disconnect();
}

CommandTask::State CommandTask::get_state()
{
    yat::AutoMutex<> guard(m_data_lock);
    return m_state;
}
