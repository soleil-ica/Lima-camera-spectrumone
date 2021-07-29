#ifndef SPECTRUMONEEVENTCTRLOBJ_H
#define SPECTRUMONEEVENTCTRLOBJ_H

#include <lima/HwEventCtrlObj.h>
#include "SpectrumComms/CommandTask.h"

namespace lima
{
  namespace SpectrumOne
  {

    typedef SpectrumComms::EventType EventType;

    class EventCtrlObj : public HwEventCtrlObj
    {
    public:
    EventCtrlObj() {};
    virtual ~EventCtrlObj() {};
    void reportCameraEvent(const std::string & str, SpectrumComms::EventType evt);
    Event::Severity getSeverity(EventType type);

    };
  } // namespace SpectrumOne
} // namespace lima

#endif // SPECTRUMONEEVENTCTRLOBJ_H
