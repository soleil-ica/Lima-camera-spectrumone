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