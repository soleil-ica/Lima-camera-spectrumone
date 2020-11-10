#include "SpectrumOneEventCtrlObj.h"

using namespace lima;
using namespace lima::SpectrumOne;

void EventCtrlObj::eventReportCallback(EventData data)
{
    Event *my_event = new Event(Hardware, getSeverity(data.evt), 
        Event::Camera, Event::Default, "\n"+data.str);
    reportEvent(my_event);
}

Event::Severity EventCtrlObj::getSeverity(EventType type)
{
    switch(type)
    {
        case EventType::Info:
        return Event::Severity::Info;

        case EventType::Warning:
        return Event::Severity::Warning;

        case EventType::Error:
        return Event::Severity::Error;

        case EventType::Fatal:
        return Event::Severity::Fatal;

        default:
        return Event::Severity::Info;
    }
}