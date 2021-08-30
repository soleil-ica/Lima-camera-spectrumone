//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2021
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################

#include "SpectrumOneEventCtrlObj.h"

using namespace lima;
using namespace lima::SpectrumOne;

void EventCtrlObj::reportCameraEvent(const std::string & str, SpectrumComms::EventType evt)
{
    Event *my_event = new Event(Hardware, getSeverity(evt), 
        Event::Camera, Event::Default, "\n"+str);
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