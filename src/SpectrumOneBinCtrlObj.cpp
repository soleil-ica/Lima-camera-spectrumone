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

#include "SpectrumOneBinCtrlObj.h"
#include "SpectrumOneCamera.h"

using namespace lima;
using namespace lima::SpectrumOne;

BinCtrlObj::BinCtrlObj(Camera *cam):
    m_cam(cam)
{
}

BinCtrlObj::~BinCtrlObj()
{
}


void BinCtrlObj::setBin(const Bin& bin)
{
    // YAT_INFO << "BinCtrlObj::setBin" << std::endl;
    m_bin = bin;
    m_cam->setBin(m_bin);
}

void BinCtrlObj::getBin(Bin& bin)
{
    // YAT_INFO << "RoBinCtrlObj::getBin" << std::endl;
    bin = m_bin;
}

void BinCtrlObj::checkBin(Bin& bin)
{
    // YAT_INFO << "BinCtrlObj::checkBin" << std::endl;
    // not implemented
}