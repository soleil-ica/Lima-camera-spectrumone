#include "lima/Exceptions.h"

#include "SpectrumOneCamera.h"


using namespace lima;
using namespace lima::SpectrumOne;


Camera::Camera(GpibConfig config, std::string tables_path, std::string expert_config):
    m_camera_name("SpectrumOneCCD"),
    m_as_master(true),
    m_config(config),
    m_tables_path(tables_path),
    m_expert_config(expert_config)
{
}

Camera::~Camera()
{
    YAT_INFO << "Camera::~Camera" << std::endl;
}

void Camera::init(EventCtrlObj* event)
{
    m_command.reset(new SFAXCommunications::CommandTask(m_config,
        EventCallback::instanciate(*event, &EventCtrlObj::eventReportCallback),
        m_tables_path, m_expert_config));

    m_command->init_sequence();
}

void Camera::forceConfig()
{
    m_command->init_sequence(true);
}

void Camera::startAcq()
{
    m_command->snap();
    // StdBufferCbMgr& buffer_mgr = m_buffer_ctrl_obj.getBuffer();
    // buffer_mgr.setStartTimestamp(Timestamp::now());
    // FrameDim frame_dim = buffer_mgr.getFrameDim();
    // Size frame_size = frame_dim.getSize();
}

/** @brief test if the camera is monochrome
 */
bool Camera::isMonochrome() const
{
}

void Camera::getStatus(HwInterface::StatusType& status)
{
    switch(m_command->get_state())
    {
        case State::Fault:
        status.set(BasicStatus::Fault);
        break;

        case State::Idle:
        status.set(BasicStatus::Ready);
        break;

        case State::Busy:
        status.set(BasicStatus::Exposure);
        break;

        case State::Init:
        case State::Config:
        status.set(BasicStatus::Config);
        break;

        default:
        status.set(BasicStatus::Fault);
        break;

    }
}

VideoMode Camera::getVideoMode() const
{
    return m_video_mode;
}

void Camera::getCameraName(std::string& name)
{
    name = m_camera_name;
}
void Camera::setVideoMode(VideoMode aMode)
{
}



void Camera::reset()
{
}

void Camera::setExpTime(const double & exp_time)
{
    m_command->set_exp_time(static_cast<int>(exp_time*1000));
}

void Camera::getExpTime(double & exp_time)
{
    
}


// void Camera::_newFrameCBK(tPvFrame* aFrame)
// {
// }

// void Camera::_newFrame(tPvFrame* aFrame)
// {
// }

//-----------------------------------------------------
// @brief range the binning to the maximum allowed
//-----------------------------------------------------
void Camera::checkBin(Bin &hw_bin)
{
}
//-----------------------------------------------------
// @brief set the new binning mode
//-----------------------------------------------------
void Camera::setBin(const Bin &set_bin)
{
}

//-----------------------------------------------------
// @brief return the current binning mode
//-----------------------------------------------------
void Camera::getBin(Bin &hw_bin)
{
}
