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

#include "SpectrumOneRoiCtrlObj.h"
#include "SpectrumOneCamera.h"

using namespace lima;
using namespace lima::SpectrumOne;

RoiCtrlObj::RoiCtrlObj(Camera *cam, Size size):
    m_max_size(size),
    m_cam(cam)
{
}

RoiCtrlObj::~RoiCtrlObj()
{
}

void RoiCtrlObj::checkRoi(const Roi& set_roi, Roi& hw_roi)
{
    YAT_ERROR << "RoiCtrlObj::checkRoi" << std::endl;
    hw_roi = set_roi;
}

void RoiCtrlObj::setRoi(const Roi& set_roi)
{
    YAT_ERROR << "RoiCtrlObj::setRoi" << std::endl;

    m_roi = set_roi;
    if(m_roi.isEmpty()) m_cam->setRoi(Roi(Point(0, 0), m_max_size));
    else m_cam->setRoi(m_roi);
}

void RoiCtrlObj::getRoi(Roi& hw_roi)
{
    hw_roi = m_roi;
}