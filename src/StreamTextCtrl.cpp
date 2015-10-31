////////////////////////////////////////////////////////////////////////////////
//
// SmokeSignal - LocalChat standard compliant application
//
// Copyright (C) 2010-2013 Joseph Kuziel and Micca Osmar
//
// This file is part of SmokeSignal.
//
// SmokeSignal is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SmokeSignal is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SmokeSignal.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////


#include "StreamTextCtrl.h"

#ifdef SMOKESIGNAL_UI_WXWIDGETS

#include <iostream>


StreamTextCtrl::StreamTextCtrl(
	  wxWindow* parent
	, wxWindowID id
	, const wxString& value
	, const wxPoint& pos
	, const wxSize& size
	, long style
	, const wxValidator& validator
	, const wxString& name
) :
wxTextCtrl(
	  parent
	, id
	, value
	, pos
	, size
	, style
	, validator
	, name
)
{
	oldBuf = NULL;
	
	GetEventHandler()->Connect(
		GetId(), EVT_STREAMOUT,
		wxCommandEventHandler(StreamTextCtrl::OnStreamOut));
}

StreamTextCtrl::~StreamTextCtrl()
{
	releaseStream();
}

void StreamTextCtrl::aquireStream()
{
	releaseStream();
	
	oldBuf = std::cout.rdbuf(this);
}

void StreamTextCtrl::releaseStream()
{
	if(oldBuf != NULL) {
		std::cout.rdbuf(oldBuf);
		oldBuf = NULL;
	}
}

int StreamTextCtrl::overflow(int i)
{
	wxCommandEvent event(EVT_STREAMOUT, GetId());
	event.SetEventObject(this);
	event.SetString((wxChar)i);
	GetEventHandler()->AddPendingEvent(event);
	
	return i;
}

void StreamTextCtrl::OnStreamOut(wxCommandEvent& event)
{
	AppendText(event.GetString());
}

#endif // SMOKESIGNAL_UI_WXWIDGETS
