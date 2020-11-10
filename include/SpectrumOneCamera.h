#ifndef SPECTRUMONECAMERA_H
#define SPECTRUMONECAMERA_H

#include <lima/Debug.h>
#include <lima/Constants.h>
#include <lima/HwMaxImageSizeCallback.h>
#include <lima/Event.h>
#include <lima/HwInterface.h>

#include "GpibComms.h"
#include "CommandTask.h"
#include "SpectrumOneEventCtrlObj.h"

#include <yat/memory/UniquePtr.h>
#include <yat/utils/Logging.h>
#include <yat/Exception.h>
#include <yat/utils/String.h>

namespace lima
{
  namespace SpectrumOne
  {
    class EventCtrlObj;

    typedef SFAXCommunications::GpibComms::GpibConfig GpibConfig;
    typedef SFAXCommunications::CommandTask::State State;
    typedef SFAXCommunications::CommandTask::EventCallback EventCallback;

    class Camera : public HwMaxImageSizeCallbackGen
    {
    public:
        Camera(GpibConfig config, std::string tables_path);
        ~Camera();

        void init(EventCtrlObj* event);
        
        
        bool isMonochrome() const;

        VideoMode   getVideoMode() const;
        void        setVideoMode(VideoMode);
        void        checkBin(Bin&);
        void        setBin(const Bin&);
        void        getBin(Bin&);

        void        getCameraName(std::string& name);

        void        startAcq();
        void        reset();
        void        getStatus(HwInterface::StatusType& status);

        const std::string& read();
        void        rebootIfHung();
        char        whereAmI();
        void        write(const std::string& argin);

        bool        m_as_master;
        void        reportExceptionCallback(const std::string & str);



    private:
        bool            m_is_error;
        GpibConfig      m_config;
        VideoMode       m_video_mode;
        std::string     m_camera_name;
        std::string     m_tables_path;
        yat::UniquePtr<SFAXCommunications::CommandTask>   m_command;
    };
  }
}
#endif // SPECTRUMONECAMERA_H
