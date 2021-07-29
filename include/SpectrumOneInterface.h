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

    class DLL_EXPORT Interface : public HwInterface
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
