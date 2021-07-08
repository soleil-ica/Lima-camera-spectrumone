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

#ifndef SPECTRUMONEINTERFACE_H
#define SPECTRUMONEINTERFACE_H

#include <lima/HwInterface.h>

namespace lima
{
  namespace SpectrumOne
  {
    class Camera;
    class DetInfoCtrlObj;
    class SyncCtrlObj;
    class EventCtrlObj;
    class RoiCtrlObj;
    class BinCtrlObj;

    class Interface : public HwInterface
    {
    private:
        DetInfoCtrlObj*     m_det_info;
        Camera*             m_cam;
        HwBufferCtrlObj*    m_buffer;
        SyncCtrlObj*        m_sync;
        EventCtrlObj*       m_event;
        RoiCtrlObj*         m_roi;
        BinCtrlObj*         m_bin;

    public:
        Interface(Camera*);
        virtual ~Interface();

        Camera* getCamera();

        virtual void getCapList(CapList &) const;

        virtual void reset(ResetLevel reset_level);
        virtual void prepareAcq();
        virtual void startAcq();
        virtual void stopAcq();
        virtual void getStatus(StatusType& status);

        virtual int getNbAcquiredFrames();
        virtual int getNbHwAcquiredFrames();


    };

  } // namespace SpectrumOne

} // namespace lima

#endif // SPECTRUMONEINTERFACE_H
