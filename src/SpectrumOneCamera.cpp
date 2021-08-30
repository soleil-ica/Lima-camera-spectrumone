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

#include "lima/Exceptions.h"

#include "SpectrumOneCamera.h"


using namespace lima;
using namespace lima::SpectrumOne;


Camera::Camera(GpibConfig gpib_config, CommandConfig command_config):
    m_camera_name("SpectrumOneCCD"),
    m_as_master(true),
    m_event(0),
    m_size(2000, 800),
    m_last_gain(0),
    m_last_temperature(0)
{
    m_frame_info.x_origin = 0;
    m_frame_info.y_origin = 0;
    m_frame_info.x_size = 2000;
    m_frame_info.y_size = 800;
    m_frame_info.x_bin = 1;
    m_frame_info.y_bin = 1;

    m_frame_nb = 0;

    m_command.reset(new SpectrumComms::CommandTask(gpib_config, command_config, this));
}

Camera::~Camera()
{
    YAT_INFO << "Camera::~Camera" << std::endl;
}

void Camera::init(EventCtrlObj* event)
{
    m_event = event;
    m_command->connect();
    m_command->init_sequence();
    //pollTemperature();
    //pollGain();
}

void Camera::forceTables()
{
    m_command->init_sequence(true);
}

void Camera::reConfig()
{
    m_command->re_config();
}

HwBufferCtrlObj* Camera::getBufferCtrlObj()
{
    return &m_buffer_ctrl_obj;
}

void Camera::on_buffer_filled()
{
    StdBufferCbMgr& buffer_mgr = m_buffer_ctrl_obj.getBuffer();
    HwFrameInfoType frame_info;
    frame_info.acq_frame_nb = 0;
    m_frame_nb = 1;
    buffer_mgr.newFrameReady(frame_info);
}

void Camera::prepareAcq()
{
    m_frame_nb = 0;
    m_command->prepare(m_frame_info);
}

void Camera::startAcq()
{
    StdBufferCbMgr& buffer_mgr = m_buffer_ctrl_obj.getBuffer();
    buffer_mgr.setStartTimestamp(Timestamp::now());
    m_command->snap(buffer_mgr.getFrameBufferPtr(0), m_frame_info.x_size, m_frame_info.y_size);
}

void Camera::setFrameDim(const FrameDim& frame_dim)
{
    // CHeck valid and stuff
    //m_frame_dim = frame_dim;
}

void Camera::getFrameDim(FrameDim& frame_dim)
{
    //frame_dim = m_frame_dim;
}


int Camera::getFrameNb() const
{
    return m_frame_nb;
}

/** @brief test if the camera is monochrome
 */
bool Camera::isMonochrome() const
{
    return true;
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

void Camera::getCameraName(std::string& name)
{
    name = m_camera_name;
}


void Camera::on_new_event(const std::string & str, SpectrumComms::EventType evt)
{
    if(m_event != 0)
    {
        m_event->reportCameraEvent(str, evt);
    }
}

void Camera::reset()
{
}

void Camera::setExpTime(const double & exp_time)
{
    m_command->set_exp_time(static_cast<int>(exp_time*1000));
}

void Camera::setGain(const int & exp_time)
{
    m_command->set_gain(exp_time);
}

void Camera::setNumFlushes(const int & num)
{
    m_command->set_num_flushes(num);
}

void Camera::setRoi(const Roi & set_roi)
{
    m_frame_info.x_origin = set_roi.getTopLeft().x;
    m_frame_info.y_origin = set_roi.getTopLeft().y;
    m_frame_info.x_size = set_roi.getSize().getWidth();
    m_frame_info.y_size = set_roi.getSize().getHeight();
}

void Camera::pollTemperature()
{
    m_command->get_temperature();
}

void Camera::temperature_callback(const double & temperature)
{
    yat::AutoMutex<> guard(m_attr_lock);
    m_last_temperature = temperature;
}

void Camera::getTemperature(double & temperature)
{
    yat::AutoMutex<> guard(m_attr_lock);
    temperature = m_last_temperature;
}

void Camera::pollGain()
{
    m_command->get_gain();
}

void Camera::gain_callback(const long & gain)
{
    yat::AutoMutex<> guard(m_attr_lock);
    m_last_gain = gain;
}

void Camera::getGain(long & gain)
{
    yat::AutoMutex<> guard(m_attr_lock);
    gain = m_last_gain;
}

void Camera::setBin(const Bin & set_bin)
{
    m_frame_info.x_bin = set_bin.getX();
    m_frame_info.y_bin = set_bin.getY();
}
