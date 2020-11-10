#ifndef SPECTRUMONEEVENTCTRLOBJ_H
#define SPECTRUMONEEVENTCTRLOBJ_H

#include <lima/HwEventCtrlObj.h>
#include "CommandTask.h"

namespace lima
{
  namespace SpectrumOne
  {

    typedef SFAXCommunications::CommandTask::EventData EventData;
    typedef SFAXCommunications::CommandTask::EventType EventType;

    class EventCtrlObj : public HwEventCtrlObj
    {
    public:
    EventCtrlObj() {};
    virtual ~EventCtrlObj() {};
    void eventReportCallback(EventData data);
    Event::Severity getSeverity(EventType type);

    };
  } // namespace SpectrumOne
} // namespace lima

#endif // SPECTRUMONEEVENTCTRLOBJ_H
