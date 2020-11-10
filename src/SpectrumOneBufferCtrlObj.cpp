#include "SpectrumOneBufferCtrlObj.h"
#include "SpectrumOneSyncCtrlObj.h"
#include "SpectrumOneCamera.h"

using namespace lima;
using namespace lima::SpectrumOne;

BufferCtrlObj::BufferCtrlObj(Camera *cam) :
    m_exposing(false),
    m_cam(cam)
{
}

void BufferCtrlObj::prepareAcq()
{
}

void BufferCtrlObj::startAcq()
{
}

