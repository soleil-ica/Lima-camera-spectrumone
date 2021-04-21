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

