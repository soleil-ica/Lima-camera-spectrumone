#ifndef SPECTRUMONESYNCCTRLOBJ_H
#define SPECTRUMONESYNCCTRLOBJ_H

#include <lima/HwSyncCtrlObj.h>
#include <lima/HwInterface.h>

namespace lima
{
  namespace SpectrumOne
  {
    class Camera;

    class SyncCtrlObj : public HwSyncCtrlObj
    {
    public:
      SyncCtrlObj(Camera*, HwBufferCtrlObj*);
      virtual ~SyncCtrlObj();

      virtual bool checkTrigMode(TrigMode trig_mode);
      virtual void setTrigMode(TrigMode  trig_mode);
      virtual void getTrigMode(TrigMode& trig_mode);

      virtual void setExpTime(double  exp_time);
      virtual void getExpTime(double& exp_time);

      virtual void setLatTime(double  lat_time);
      virtual void getLatTime(double& lat_time);

    //   virtual void setNbFrames(int  nb_frames);
    //   virtual void getNbFrames(int& nb_frames);

      virtual void setNbHwFrames(int  nb_frames);
      virtual void getNbHwFrames(int& nb_frames);

      virtual void getValidRanges(ValidRangesType& valid_ranges);

    private:
        Camera*         m_cam;
        HwBufferCtrlObj*  m_buffer;
        TrigMode        m_trig_mode;
        int             m_nb_frames;
        bool            m_started;
        double          m_exp_time;
    };

  } // namespace Prosilica
} // namespace lima

#endif // SPECTRUMONESYNCCTRLOBJ_H
