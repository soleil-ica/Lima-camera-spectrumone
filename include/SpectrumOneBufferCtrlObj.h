#ifndef SPECTRUMONEBUFFERCTRLOBJ_H
#define SPECTRUMONEBUFFERCTRLOBJ_H

#include <lima/HwBufferMgr.h>
#include <yat/memory/SharedPtr.h>

namespace lima
{
  namespace SpectrumOne
  {
    class Camera;
    class SyncCtrlObj;
    class Interface;

    class BufferCtrlObj : public SoftBufferCtrlObj
    {

    public:
        BufferCtrlObj(Camera *cam);
        void prepareAcq();
        void startAcq();

    private:
        Camera*         m_cam;
        bool            m_exposing;
    };
  }
}
#endif // SPECTRUMONEBUFFERCTRLOBJ_H
