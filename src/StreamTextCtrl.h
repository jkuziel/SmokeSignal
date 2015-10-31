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


#include "version.h"

#ifdef SMOKESIGNAL_UI_WXWIDGETS

#ifndef __STREAMTEXTCTRL_H__
#define __STREAMTEXTCTRL_H__

#include <streambuf>
#include <wx/textctrl.h>


DEFINE_EVENT_TYPE(EVT_STREAMOUT)


class StreamTextCtrl : public wxTextCtrl
{
	public:
		StreamTextCtrl(
			  wxWindow* parent
			, wxWindowID id
			, const wxString& value = ""
			, const wxPoint& pos = wxDefaultPosition
			, const wxSize& size = wxDefaultSize
			, long style = 0
			, const wxValidator& validator = wxDefaultValidator
			, const wxString& name = wxTextCtrlNameStr
		);
		virtual ~StreamTextCtrl();
		
		void aquireStream();
		void releaseStream();
		
	protected:
		virtual int overflow(int i);
		virtual void OnStreamOut(wxCommandEvent&);
		
	private:
		std::streambuf* oldBuf;
};


#endif // __STREAMTEXTCTRL_H__

#endif // SMOKESIGNAL_UI_WXWIDGETS
