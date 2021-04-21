#ifndef SPECTRUMONEBINCTRLOBJ_H
#define SPECTRUMONEBINCTRLOBJ_H

#include <lima/HwBinCtrlObj.h>
#include <lima/Debug.h>


namespace lima
{
  namespace SpectrumOne
  {
    class Camera;
    class BinCtrlObj : public HwBinCtrlObj
    {
      DEB_CLASS_NAMESPC(DebModCamera, "BinCtrlObj","SpectrumOne");

    public:
        BinCtrlObj(Camera*);
        virtual ~BinCtrlObj();

        virtual void setBin(const Bin& bin);
        virtual void getBin(Bin& bin);
        virtual void checkBin(Bin& bin);
    private:
        Camera* m_cam;
        Bin m_bin;
    };

  } // namespace SpectrumOne
} // namespace lima


#endif // SPECTRUMONEBINCTRLOBJ_H