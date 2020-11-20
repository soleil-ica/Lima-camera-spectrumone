#include <sstream>
#include "SpectrumOneSyncCtrlObj.h"
#include "SpectrumOneBufferCtrlObj.h"
#include "SpectrumOneCamera.h"

using namespace lima;
using namespace lima::SpectrumOne;

SyncCtrlObj::SyncCtrlObj(Camera *cam, BufferCtrlObj* buffer) :
    m_cam(cam),
    m_buffer(buffer),
    m_trig_mode(IntTrig),
    m_nb_frames(1),
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

void SyncCtrlObj::setExpTime(double exp_time)
{
    YAT_ERROR << "SyncCtrlObj::setExpTime" << std::endl;
    m_cam->setExpTime(exp_time);
}

void SyncCtrlObj::getExpTime(double &exp_time)
{
    YAT_ERROR << "SyncCtrlObj::getExpTime" << std::endl;
    m_cam->getExpTime(exp_time);
}

void SyncCtrlObj::setLatTime(double  lat_time)
{
  //No latency managed
}

void SyncCtrlObj::getLatTime(double& lat_time)
{
    lat_time = 0.;        // Don't know
}

void SyncCtrlObj::setNbFrames(int  nb_frames)
{
    m_nb_frames = nb_frames;
}

void SyncCtrlObj::getNbFrames(int& nb_frames)
{
    nb_frames = m_nb_frames;
}

void SyncCtrlObj::setNbHwFrames(int  nb_frames)
{
    setNbFrames(nb_frames);
}

void SyncCtrlObj::getNbHwFrames(int& nb_frames)
{
    getNbFrames(nb_frames);
}

void SyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges)
{
  valid_ranges.min_exp_time = 1e-6; // Don't know
  valid_ranges.max_exp_time = 6000.; // Don't know
  valid_ranges.min_lat_time = 0.; // Don't know
  valid_ranges.max_lat_time = 0.; // Don't know
}

void SyncCtrlObj::startAcq()
{
    YAT_ERROR << "SyncCtrlObj::startAcq" << std::endl;
    m_started = true;
}

void SyncCtrlObj::stopAcq(bool clearQueue)
{
    YAT_ERROR << "SyncCtrlObj::stopAcq" << std::endl;
    m_started = false;
}

void SyncCtrlObj::getStatus(HwInterface::StatusType& status)
{
    m_cam->getStatus(status);
}
