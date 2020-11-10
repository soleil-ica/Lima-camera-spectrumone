#include "SpectrumOneInterface.h"
#include "SpectrumOneCamera.h"
#include "SpectrumOneDetInfoCtrlObj.h"
#include "SpectrumOneBufferCtrlObj.h"
#include "SpectrumOneSyncCtrlObj.h"
#include "SpectrumOneEventCtrlObj.h"


using namespace lima;
using namespace lima::SpectrumOne;

Interface::Interface(Camera *cam) :
    m_cam(cam)
{
    m_det_info = new DetInfoCtrlObj(cam);
    m_buffer = new BufferCtrlObj(cam);
    m_sync = new SyncCtrlObj(cam, m_buffer);
    m_event = new EventCtrlObj();
    
    cam->init(m_event);
}

Interface::~Interface()
{
    delete m_det_info;
    delete m_buffer;
    delete m_sync;
    delete m_event;
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
}

void Interface::reset(ResetLevel reset_level)
{
}

void Interface::prepareAcq()
{
}

void Interface::startAcq()
{
}

void Interface::stopAcq()
{
}

void Interface::getStatus(StatusType& status)
{
    m_sync->getStatus(status);
}

int Interface::getNbAcquiredFrames()
{
    return 0;
}

int Interface::getNbHwAcquiredFrames()
{
    return 0;
}