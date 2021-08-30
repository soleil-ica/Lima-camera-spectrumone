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

#include "SpectrumOneShutterCtrlObj.h"
#include "SpectrumOneCamera.h"

using namespace lima;
using namespace lima::SpectrumOne;

ShutterCtrlObj::ShutterCtrlObj(Camera *cam):
    m_cam(cam)
{
    m_shutter_list.push_back(ShutterManual);
}

ShutterCtrlObj::~ShutterCtrlObj()
{
}

bool ShutterCtrlObj::checkMode(ShutterMode shut_mode) const
{
    if(shut_mode == ShutterManual) return true;
    return false;
}

void ShutterCtrlObj::getModeList(ShutterModeList&  mode_list) const
{
    mode_list = m_shutter_list;
}

void ShutterCtrlObj::setMode(ShutterMode  shut_mode) 
{

}

void ShutterCtrlObj::getMode(ShutterMode& shut_mode) const 
{
    shut_mode = ShutterManual;
}

void ShutterCtrlObj::setState(bool shut_open)
{
    m_cam->setShutter(shut_open);
}

void ShutterCtrlObj::getState(bool& shut_open) const
{
    m_cam->getShutter(shut_open);
}

void ShutterCtrlObj::setOpenTime (double  shut_open_time)
{

}

void ShutterCtrlObj::getOpenTime (double& shut_open_time) const
{
    shut_open_time = 0;
}

void ShutterCtrlObj::setCloseTime(double  shut_close_time)
{

}

void ShutterCtrlObj::getCloseTime(double& shut_close_time) const 
{
    shut_close_time = 0;
}