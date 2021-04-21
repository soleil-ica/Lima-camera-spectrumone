#ifndef SPECTRUMONEROICTRLOBJ_H
#define SPECTRUMONEROICTRLOBJ_H

#include <lima/HwRoiCtrlObj.h>
#include <lima/Debug.h>


namespace lima
{
  namespace SpectrumOne
  {
    class Camera;
    class RoiCtrlObj : public HwRoiCtrlObj
    {
      DEB_CLASS_NAMESPC(DebModCamera, "RoiCtrlObj","SpectrumOne");

    public:
        RoiCtrlObj(Camera*, Size);
        virtual ~RoiCtrlObj();

        virtual void checkRoi(const Roi& set_roi, Roi& hw_roi);
        virtual void setRoi(const Roi& set_roi);
        virtual void getRoi(Roi& hw_roi);
    private:
        Camera* m_cam;
        Roi m_roi;
        const Size m_max_size;
    };

  } // namespace SpectrumOne
} // namespace lima


#endif // SPECTRUMONEROICTRLOBJ_H