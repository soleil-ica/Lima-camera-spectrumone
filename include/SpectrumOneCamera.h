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
    class BufferCtrlObj;

    typedef SFAXCommunications::GpibComms::GpibConfig GpibConfig;
    typedef SFAXCommunications::CommandTask::State State;
    typedef SFAXCommunications::CommandTask::EventCallback EventCallback;
    typedef HwInterface::StatusType::Basic BasicStatus;

    class Camera : public HwMaxImageSizeCallbackGen
    {
    public:
        Camera(GpibConfig config, std::string tables_path, std::string expert_config);
        virtual ~Camera();

        void init(EventCtrlObj* event);
        
        
        bool isMonochrome() const;

        VideoMode   getVideoMode() const;
        void        setVideoMode(VideoMode);
        void        checkBin(Bin&);
        void        setBin(const Bin&);
        void        getBin(Bin&);
        void        setExpTime(const double &);
        void        getExpTime(double &);


        void        getCameraName(std::string& name);

        void        startAcq();
        void        reset();
        void        getStatus(HwInterface::StatusType& status);
        
        const std::string& read();
        void        forceConfig();
        void        write(const std::string& argin);

        bool        m_as_master;



    private:
        bool            m_is_error;
        GpibConfig      m_config;
        VideoMode       m_video_mode;
        std::string     m_camera_name;
        std::string     m_tables_path;
        std::string     m_expert_config;
        yat::UniquePtr<SFAXCommunications::CommandTask, yat::TaskExiter>   m_command;
    };
  }
}
#endif // SPECTRUMONECAMERA_H
