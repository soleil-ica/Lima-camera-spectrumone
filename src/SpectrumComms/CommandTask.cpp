#include "SpectrumComms/CommandTask.h"

using namespace SpectrumComms;

CommandTask::CommandTask(GpibComms::GpibConfig gpib_config, CommandConfig command_config, Listener* listener) :
    m_state(State::Init),
    m_listener(listener),
    m_interface(gpib_config, listener),
    m_config(command_config),
    m_col_size(0)
{
    YAT_TRACE("CommandTask::CommandTask");

    m_cam_data.modified = false;
    m_cam_data.x_origin = 0;
    m_cam_data.y_origin = 0;
    m_cam_data.x_size = 2000;
    m_cam_data.y_size = 800;
    m_cam_data.x_bin = 1;
    m_cam_data.y_bin = 1;
    m_cam_data.num_flushes = 2;
    m_cam_data.gain = 0;
    m_cam_data.exp_time = 1000;
    m_cam_data.modified = true;

    // launch task:
    go_asynchronously();
}

void CommandTask::connect()
{
    YAT_TRACE("CommandTask::connect");

    yat::Message* msg = yat::Message::allocate(CONNECT_SOCKET,
        INIT_MSG_PRIORITY, true);
    post(msg);
}

void CommandTask::init_sequence(const bool & force_config)
{
    YAT_TRACE("CommandTask::init_sequence");

    yat::Message* msg = yat::Message::allocate(INIT_SEQUENCE,
        DEFAULT_MSG_PRIORITY, true);
    msg->attach_data(force_config);
    post(msg);
}

void CommandTask::set_exp_time(const int & exp_time)
{
    YAT_TRACE("CommandTask::set_exp_time");

    if(exp_time == m_cam_data.exp_time) return;

    yat::Message* msg = yat::Message::allocate(SET_EXP_TIME,
        DEFAULT_MSG_PRIORITY, true);
    msg->attach_data(exp_time);
    post(msg);

    m_cam_data.exp_time = exp_time;
}

void CommandTask::set_gain(const int & gain)
{
    YAT_TRACE("CommandTask::set_gain");

    if(gain == m_cam_data.gain) return;

    yat::Message* msg = yat::Message::allocate(SET_GAIN,
        DEFAULT_MSG_PRIORITY, true);
    msg->attach_data(gain);
    post(msg);

    m_cam_data.gain = gain;
}

void CommandTask::set_num_flushes(const int & num)
{
    YAT_TRACE("CommandTask::set_num_flushes");

    if(num == m_cam_data.num_flushes) return;

    yat::Message* msg = yat::Message::allocate(SET_FLUSHES,
        DEFAULT_MSG_PRIORITY, true);
    msg->attach_data(num);
    post(msg);

    m_cam_data.num_flushes = num;
    // m_cam_data.modified = true;
}

void CommandTask::prepare(const FrameInfo & frame)
{
    YAT_TRACE("CommandTask::prepare");

    if( frame.x_origin != m_cam_data.x_origin ||
        frame.y_origin != m_cam_data.y_origin ||
        frame.x_size != m_cam_data.x_size ||
        frame.y_size != m_cam_data.y_size ||
        frame.x_bin != m_cam_data.x_bin ||
        frame.y_bin != m_cam_data.y_bin)
        m_cam_data.modified = true;

    if(!m_cam_data.modified) return;

    m_cam_data.x_origin = frame.x_origin;
    m_cam_data.y_origin = frame.y_origin;
    m_cam_data.x_size = frame.x_size;
    m_cam_data.y_size = frame.y_size;
    m_cam_data.x_bin = frame.x_bin;
    m_cam_data.y_bin = frame.y_bin;

    yat::Message* msg = yat::Message::allocate(PREPARE,
        DEFAULT_MSG_PRIORITY, true);
    msg->attach_data(m_cam_data);
    post(msg);

    m_cam_data.modified = false;
}

void CommandTask::snap(void* buffer_ptr, const int & x_size, const int & y_size)
{
    YAT_TRACE("CommandTask::snap");

    yat::Message* msg = yat::Message::allocate(SNAP,
        DEFAULT_MSG_PRIORITY, true);
    SnapInfo info;
    info.buffer_ptr = buffer_ptr;
    info.x_size = x_size;
    info.y_size = y_size;
    msg->attach_data(info);
    post(msg);
}

void CommandTask::get_temperature()
{
    YAT_TRACE("CommandTask::get_temperature");

    yat::Message* msg = yat::Message::allocate(GET_TEMPERATURE,
        DEFAULT_MSG_PRIORITY, true);
    post(msg);
}

void CommandTask::get_gain()
{
    YAT_TRACE("CommandTask::get_gain");

    yat::Message* msg = yat::Message::allocate(GET_GAIN,
        DEFAULT_MSG_PRIORITY, true);
    post(msg);
}

void CommandTask::re_config()
{
    YAT_TRACE("CommandTask::re_config");

    yat::Message* msg = yat::Message::allocate(RE_CONFIG,
        DEFAULT_MSG_PRIORITY, true);
    post(msg);
}

CommandTask::State CommandTask::get_state()
{
    yat::AutoMutex<> guard(m_data_lock);
    return m_state;
}

CommandTask::~CommandTask()
{
    YAT_TRACE("CommandTask::~CommandTask");
}
