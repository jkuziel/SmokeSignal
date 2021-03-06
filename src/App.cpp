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


#include "App.h"

#ifdef SMOKESIGNAL_UI_WXWIDGETS

#include "Frame.h"
#include "proc.h"


IMPLEMENT_APP(App)


bool App::OnInit()
{
	initProc();

	wxString title("SmokeSignal (");
	title.append(getUserName());
	title.append(")");
	
	Frame* frame = new Frame(title);
	frame->Show(true);

	frame->CreateStatusBar();
	frame->SetStatusText(_T("Ready"));

	this->SetTopWindow(frame);

	return true;
}

int App::OnExit()
{
	exitProc();
	return wxApp::OnExit();
}

#endif // SMOKESIGNAL_UI_WXWIDGETS
