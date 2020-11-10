#include "lima/Exceptions.h"

#include "SpectrumOneCamera.h"


using namespace lima;
using namespace lima::SpectrumOne;


Camera::Camera(GpibConfig config, std::string tables_path):
    m_camera_name("SpectrumOneCCD"),
    m_as_master(true),
    m_config(config),
    m_tables_path(tables_path)
{
}

Camera::~Camera()
{
    try
    {
        m_command->disconnect();
    }
    catch(const yat::Exception & e)
    {
        throw LIMA_HW_EXC(Error, "Error when disconnecting from camera!\n" + e.to_string());
    }
}

void Camera::init(EventCtrlObj* event)
{
    m_command.reset(new SFAXCommunications::CommandTask(m_config,
        EventCallback::instanciate(*event, &EventCtrlObj::eventReportCallback), m_tables_path));

    m_command->connect();
    m_command->init_sequence();
}

char Camera::whereAmI()
{
    m_command->init_sequence(true);
}



void Camera::rebootIfHung()
{
    //long i = 222;
    //m_comms->gpibWrite("\xDE");
    //m_command->reboot

    //m_command->gpibWrite("O2000\x0");
    //m_command->force_tables();

    YAT_INFO << "Received:" <<  m_command->gpib_read() << std::endl;
}


void Camera::write(const std::string & argin)
{
    try
    {
        m_command->gpib_write(argin);
    }
    catch(const yat::Exception & e)
    {
        //m_is_error = true;
        throw LIMA_HW_EXC(Error, "Cannot write!" + e.to_string());
    }
}

const std::string& Camera::read(void)
{
    try
    {
        return m_command->gpib_read();
    }
    catch(const yat::Exception & e)
    {
        //m_is_error = true;
        throw LIMA_HW_EXC(Error, "Cannot read!" + e.to_string());
    }

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
        default:
        case State::Fault:
        status.det = DetFault;
        status.acq = AcqFault;
        break;

        case State::Idle:
        status.det = DetIdle;
        status.acq = AcqReady;
        break;

        case State::Init:
        status.det = DetIdle;
        status.acq = AcqConfig;
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

// void Camera::_allocBuffer()
// {
// }
/** @brief start the acquisition.
    must have m_video != NULL and previously call _allocBuffer
*/
void Camera::startAcq()
{
}

void Camera::reset()
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
