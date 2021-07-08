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

#include <cstdlib>
#include "SpectrumOneDetInfoCtrlObj.h"
#include "SpectrumOneCamera.h"

using namespace lima;
using namespace lima::SpectrumOne;

DetInfoCtrlObj::DetInfoCtrlObj(Camera *cam):
    m_cam(cam)
{
}

DetInfoCtrlObj::~DetInfoCtrlObj()
{
}

void DetInfoCtrlObj::getMaxImageSize(Size& max_image_size)
{
    max_image_size = m_cam->m_size;
}

void DetInfoCtrlObj::getDetectorImageSize(Size& det_image_size)
{
  getMaxImageSize(det_image_size);
}

void DetInfoCtrlObj::getDefImageType(ImageType& def_image_type)
{
    def_image_type = Bpp16;
}

void DetInfoCtrlObj::getCurrImageType(ImageType& curr_image_type)
{
    curr_image_type = Bpp16;
}

void DetInfoCtrlObj::setCurrImageType(ImageType curr_image_type)
{
}

void DetInfoCtrlObj::getPixelSize(double& x_size,double& y_size)
{  
  x_size = y_size = -1.;        // @todo don't know
}

void DetInfoCtrlObj::getDetectorType(std::string& det_type)
{
    det_type = "SpectrumOne";
}

void DetInfoCtrlObj::getDetectorModel(std::string& det_model)
{
    det_model = "3000";
}

void DetInfoCtrlObj::registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{

}

void DetInfoCtrlObj::unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{

}

