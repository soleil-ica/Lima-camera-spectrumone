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

#include <sstream>
#include "SpectrumOneSyncCtrlObj.h"
#include "SpectrumOneCamera.h"

using namespace lima;
using namespace lima::SpectrumOne;

SyncCtrlObj::SyncCtrlObj(Camera *cam, HwBufferCtrlObj* buffer) :
    m_cam(cam),
    m_buffer(buffer),
    m_trig_mode(IntTrig),
    m_nb_frames(1),
    m_exp_time(0),
    m_started(false)
{
    m_access_mode = cam->m_as_master ? 
    HwSyncCtrlObj::Master : HwSyncCtrlObj::Monitor;
}

SyncCtrlObj::~SyncCtrlObj()
{
}

bool SyncCtrlObj::checkTrigMode(TrigMode trig_mode)
{
    switch(trig_mode)
    {
        case IntTrig:
        case IntTrigMult:
        case ExtTrigMult:
        return true;

        default:
        return false;
    }
}

void SyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
    m_trig_mode = trig_mode;
}

void SyncCtrlObj::getTrigMode(TrigMode &trig_mode)
{
    trig_mode = m_trig_mode;
}

void SyncCtrlObj::setExpTime(double set_exp_time)
{
    YAT_ERROR << "SyncCtrlObj::setExpTime" << std::endl;
    m_exp_time = set_exp_time;
    m_cam->setExpTime(set_exp_time);
}

void SyncCtrlObj::getExpTime(double & get_exp_time)
{
    YAT_ERROR << "SyncCtrlObj::getExpTime" << std::endl;
    get_exp_time = m_exp_time;
}

void SyncCtrlObj::setLatTime(double  lat_time)
{
  //No latency managed
}

void SyncCtrlObj::getLatTime(double& lat_time)
{
    lat_time = 0.;        // Don't know
}

void SyncCtrlObj::setNbHwFrames(int  nb_frames)
{
    m_nb_frames = nb_frames;
}

void SyncCtrlObj::getNbHwFrames(int& nb_frames)
{
    nb_frames = m_nb_frames;
}

void SyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges)
{
  valid_ranges.min_exp_time = 1e-6; // Don't know
  valid_ranges.max_exp_time = 6000.; // Don't know
  valid_ranges.min_lat_time = 0.; // Don't know
  valid_ranges.max_lat_time = 0.; // Don't know
}
