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

#include "SpectrumOneInterface.h"
#include "SpectrumOneCamera.h"
#include "SpectrumOneDetInfoCtrlObj.h"
#include "SpectrumOneSyncCtrlObj.h"
#include "SpectrumOneEventCtrlObj.h"
#include "SpectrumOneRoiCtrlObj.h"
#include "SpectrumOneBinCtrlObj.h"


using namespace lima;
using namespace lima::SpectrumOne;

Interface::Interface(Camera *cam) :
    m_cam(cam)
{
    m_det_info = new DetInfoCtrlObj(cam);
    m_buffer = cam->getBufferCtrlObj();
    m_sync = new SyncCtrlObj(cam, m_buffer);
    m_event = new EventCtrlObj();
    m_roi = new RoiCtrlObj(cam, cam->m_size);
    m_bin = new BinCtrlObj(cam);
    
    cam->init(m_event);
}

Interface::~Interface()
{
    delete m_det_info;
    delete m_buffer;
    delete m_sync;
    delete m_event;
    delete m_roi;
    delete m_bin;
}

Camera* Interface::getCamera()
{
    return m_cam;
}

void Interface::getCapList(CapList &cap_list) const
{
    cap_list.push_back(HwCap(m_det_info));
    cap_list.push_back(HwCap(m_sync));
    cap_list.push_back(HwCap(m_buffer));
    cap_list.push_back(HwCap(m_event));
    cap_list.push_back(HwCap(m_roi));
    cap_list.push_back(HwCap(m_bin));
}

void Interface::reset(ResetLevel reset_level)
{
}

void Interface::prepareAcq()
{
    YAT_ERROR << "Interface::prepareAcq" << std::endl;
    m_cam->prepareAcq();
}

void Interface::startAcq()
{
    YAT_ERROR << "Interface::startAcq" << std::endl;
    m_cam->startAcq();
}

void Interface::stopAcq()
{
    YAT_ERROR << "Interface::stopAcq" << std::endl;
    m_cam->stopAcq();
}

void Interface::getStatus(StatusType& status)
{
    m_cam->getStatus(status);
}

int Interface::getNbAcquiredFrames()
{
    return m_cam->getFrameNb();
}

int Interface::getNbHwAcquiredFrames()
{
    return m_cam->getFrameNb();
}