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

#ifndef SPECTRUMONECAMERA_H
#define SPECTRUMONECAMERA_H

#include <lima/Debug.h>
#include <lima/Constants.h>
#include <lima/HwMaxImageSizeCallback.h>
#include <lima/Event.h>
#include <lima/HwInterface.h>

#include "GpibComms.h"
#include "CommandTask.h"
#include "SpectrumOneEventCtrlObj.h"

#include <yat/memory/UniquePtr.h>
#include <yat/utils/Logging.h>
#include <yat/Exception.h>
#include <yat/utils/String.h>

namespace lima
{
  namespace SpectrumOne
  {
    class EventCtrlObj;
    class BufferCtrlObj;

    typedef SpectrumComms::GpibComms::GpibConfig GpibConfig;
    typedef SpectrumComms::CommandTask::CommandConfig CommandConfig;
    typedef SpectrumComms::CommandTask::State State;
    typedef SpectrumComms::CommandTask::FrameInfo FrameInfo;
    typedef HwInterface::StatusType::Basic BasicStatus;

    class Camera : public HwMaxImageSizeCallbackGen, public SpectrumComms::Listener
    {
    public:
        Camera(GpibConfig gpib_config, CommandConfig command_config);
        virtual ~Camera();

        void init(EventCtrlObj* event);
        HwBufferCtrlObj *getBufferCtrlObj();
        
        bool isMonochrome() const;

        VideoMode   getVideoMode() const;
        void        setVideoMode(VideoMode);
        void        setExpTime(const double &);
        void        setGain(const int &);
        void        setNumFlushes(const int &);
        void        setShutter(const bool & shutter);
        void        setBin(const Bin&);
        void        setRoi(const Roi&);

        virtual void on_buffer_filled();
        virtual void on_new_event(const std::string & str, SpectrumComms::EventType evt);
        virtual void temperature_callback(const double & temperature);
        virtual void gain_callback(const long & gain);

        void        setFrameDim(const FrameDim& frame_dim);
        void        getFrameDim(FrameDim& frame_dim);


        void        getCameraName(std::string& name);

        void        startAcq();
        void        stopAcq();
        void        prepareAcq();
        void        reset();
        void        getStatus(HwInterface::StatusType& status);
        
        void        forceTables();
        void        reConfig();
        void pollTemperature();
        void pollGain();
        void getGain(long & gain);
        void getTemperature(double & temperature);

        int getFrameNb() const;

        bool        m_as_master;
        const Size m_size;
        const std::string     m_camera_name;

    private:
        int             m_frame_nb;
        FrameInfo       m_frame_info;
        yat::Mutex      m_attr_lock;
        double          m_last_temperature;
        long            m_last_gain;

        
        SoftBufferCtrlObj m_buffer_ctrl_obj;
        EventCtrlObj*   m_event;
        yat::UniquePtr<SpectrumComms::CommandTask, yat::TaskExiter>   m_command;
    };
  }
}
#endif // SPECTRUMONECAMERA_H
