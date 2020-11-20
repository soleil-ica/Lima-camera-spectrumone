#include "CommandTask.h"

using namespace SFAXCommunications;

CommandTask::CommandTask(GpibComms::GpibConfig config,
    EventCallback report_event, std::string tables_path,
    std::string expert_config)
{
    YAT_TRACE("CommandTask::CommandTask");

    m_report_event = report_event;
    m_state = State::Init;
    m_tables_path = tables_path;
    m_expert_config = expert_config;

    m_comms.reset(new GpibComms(config));

    // launch task:
    go_asynchronously();
}

// void CommandTask::connect()
// {
//     YAT_TRACE("CommandTask::connect");

//     yat::Message* msg = yat::Message::allocate(CONNECT_SOCKET,
//         INIT_MSG_PRIORITY, true);
//     this->post(msg);
// }

void CommandTask::init_sequence(const bool & force_config)
{
    YAT_TRACE("CommandTask::init_sequence");

    yat::Message* msg = yat::Message::allocate(INIT_SEQUENCE,
        DEFAULT_MSG_PRIORITY, true);
    msg->attach_data(force_config);
    this->post(msg);
}

void CommandTask::set_exp_time(const int & exp_time)
{
    YAT_TRACE("CommandTask::set_exp_time");

    yat::Message* msg = yat::Message::allocate(SET_EXP_TIME,
        DEFAULT_MSG_PRIORITY, true);
    msg->attach_data(exp_time);
    this->post(msg);
}

void CommandTask::set_gain(const int & gain)
{
    YAT_TRACE("CommandTask::set_gain");

    yat::Message* msg = yat::Message::allocate(SET_GAIN,
        DEFAULT_MSG_PRIORITY, true);
    msg->attach_data(gain);
    this->post(msg);
}

void CommandTask::snap()
{
    YAT_TRACE("CommandTask::snap");

    yat::Message* msg = yat::Message::allocate(SNAP,
        DEFAULT_MSG_PRIORITY, true);
    this->post(msg);
}

CommandTask::State CommandTask::get_state()
{
    yat::AutoMutex<> guard(m_data_lock);
    return m_state;
}

CommandTask::~CommandTask()
{
    YAT_INFO << "CommandTask::~CommandTask" << std::endl;
}
