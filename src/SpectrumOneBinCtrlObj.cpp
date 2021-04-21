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
    YAT_ERROR << "BinCtrlObj::setBin" << std::endl;
    m_bin = bin;
    m_cam->setBin(m_bin);
}

void BinCtrlObj::getBin(Bin& bin)
{
    YAT_ERROR << "RoBinCtrlObj::getBin" << std::endl;
    bin = m_bin;
}

void BinCtrlObj::checkBin(Bin& bin)
{
    YAT_ERROR << "BinCtrlObj::checkBin" << std::endl;
}