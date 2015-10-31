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


#include "Frame.h"

#ifdef SMOKESIGNAL_UI_WXWIDGETS


#include "proc.h"

#include <iostream>

Frame* Frame::me;


Frame::Frame(const wxString& title):
	wxFrame(0, -1, title, wxDefaultPosition, wxSize(400, 300))
{
	me = this;

	SetMinSize(wxSize(400, 300));

	// menu bar
	wxMenuBar* menuBar = new wxMenuBar;
	
	// Actions menu
	wxMenu* mnuActions = new wxMenu;
	mnuActions->Append(ID_BCAST, wxT("Say &Hello"), 
		wxT("Broadcast to all clients on the subnet"));
	mnuActions->Append(ID_DROP_USER, wxT("&Drop User"), 
		wxT("Remove user from the user list"));
	mnuActions->AppendSeparator();
	mnuActions->Append(wxID_EXIT, wxT("E&xit"));

	// Options menu
	wxMenu* mnuOptions = new wxMenu;
	mnuOptions->AppendCheckItem(ID_INC_SEQ_NUM, 
		wxT("Increment &Sequence Number"));
	mnuOptions->Check(ID_INC_SEQ_NUM, true);
	mnuOptions->Append(ID_SET_TO_VAL, wxT("Set Message ACK &Timeout..."));

	// Help menu
	wxMenu* mnuHelp = new wxMenu;
	mnuHelp->Append(ID_ABOUT, wxT("&About..."),
		wxT("Display copyright information"));

	// menu bar
	menuBar->Append(mnuActions, wxT("&Actions"));
	menuBar->Append(mnuOptions, wxT("&Options"));
	menuBar->Append(mnuHelp, wxT("&Help"));	

	SetMenuBar(menuBar);
	
	wxPanel* panel = new wxPanel(this, wxID_ANY);

	// controls
	msgCtrl = new StreamTextCtrl(panel, ID_MSG_TB, wxT(""), 
      wxPoint(-1, -1), wxSize(-1, -1), wxTE_MULTILINE | wxTE_READONLY);

	msgCtrl->aquireStream();

	cmdCtrl = new wxTextCtrl(panel, ID_CMD_TB, wxT(""), wxPoint(-1, -1), 
		wxSize(-1, -1), wxTE_PROCESS_ENTER);
	cmdCtrl->SetMaxLength(MSG_STRLEN);
	cmdCtrl->SetFocus();

	sendBtn = new wxButton(panel, ID_SEND_BTN, wxT("Send"));
	sendBtn->Enable(false);

	userListCtrl = new wxListBox(panel, ID_USER_LB);

	useKeyCtrl = new wxCheckBox(panel, ID_USEKEY_CB, 
		wxT("Use Encryption Key"));
	useKeyCtrl->SetValue(false);

	keyCtrl = new wxTextCtrl(panel, ID_KEY_TB, wxT(""), wxPoint(-1, -1), 
		wxSize(-1, -1), wxTE_PROCESS_ENTER);
	keyCtrl->SetMaxLength(MSG_STRLEN);

	// sizers
	wxBoxSizer* hMainSizer = new wxBoxSizer(wxHORIZONTAL); // left/right sizer
	wxBoxSizer* lVSizer = new wxBoxSizer(wxVERTICAL); // left side
	wxBoxSizer* rVSizer = new wxBoxSizer(wxVERTICAL); // right side

	// msg window
	lVSizer->Add(new wxStaticText(panel, wxID_ANY, wxT("Chat Box")), 0, wxLEFT);
	lVSizer->Add(msgCtrl, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP | wxBOTTOM);

	// command bar
	wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);
	hSizer->Add(cmdCtrl, 1, wxEXPAND | wxLEFT | wxRIGHT);
	hSizer->Add(10, -1); // margin
	hSizer->Add(sendBtn, 0, wxRIGHT);

	lVSizer->Add(-1, 10); // margin
	lVSizer->Add(hSizer, 0, wxEXPAND | wxLEFT | wxRIGHT);

	// user controls
	rVSizer->Add(new wxStaticText(panel, wxID_ANY, wxT("Users Online")), 0, 
		wxLEFT);
	rVSizer->Add(userListCtrl, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP | wxBOTTOM);
	rVSizer->Add(-1, 10); // margin
	rVSizer->Add(useKeyCtrl);
	rVSizer->Add(-1, 5);
	rVSizer->Add(keyCtrl, 0, wxEXPAND | wxLEFT | wxRIGHT);

	// left side
	hMainSizer->Add(lVSizer, 1,
		wxEXPAND | wxLEFT | wxRIGHT | wxTOP | wxBOTTOM, 10);

	// right side
	hMainSizer->Add(rVSizer, 0,
		wxEXPAND | wxLEFT | wxRIGHT | wxTOP | wxBOTTOM, 10);

	panel->SetSizer(hMainSizer);

	Center();

	// events
	Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::OnExit));

	Connect(ID_SEND_BTN, wxEVT_COMMAND_BUTTON_CLICKED, 
		wxCommandEventHandler(Frame::OnSend));

	Connect(ID_CMD_TB, wxEVT_COMMAND_TEXT_ENTER, 
		wxCommandEventHandler(Frame::OnSend));

	Connect(ID_USER_LB, wxEVT_COMMAND_LISTBOX_SELECTED, 
		wxCommandEventHandler(Frame::OnUserSelect));

	Connect(ID_KEY_TB, wxEVT_COMMAND_TEXT_ENTER, 
		wxCommandEventHandler(Frame::OnKeyEnter));

	Connect(ID_USEKEY_CB, wxEVT_COMMAND_CHECKBOX_CLICKED, 
		wxCommandEventHandler(Frame::OnToggleKey));

	Connect(ID_CMD_TB, wxEVT_COMMAND_TEXT_UPDATED, 
		wxCommandEventHandler(Frame::OnCmdUpdate));

	Connect(ID_BCAST, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(Frame::OnBcast));

	Connect(ID_DROP_USER, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(Frame::OnDropUser));

	Connect(ID_INC_SEQ_NUM, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(Frame::OnIncSeqNum));

	Connect(ID_SET_TO_VAL, wxEVT_COMMAND_MENU_SELECTED, 
		wxCommandEventHandler(Frame::OnSetTOVal));

	Connect(ID_ABOUT, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Frame::OnAbout));

	Connect(ID_USER_LB, wxEVT_CONTEXT_MENU, 
		wxContextMenuEventHandler(Frame::OnUserContext));
		

	setNewUserCallback(OnNewUser);
	setRemoveUserCallback(OnRemoveUser);
}

void Frame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void Frame::OnSend(wxCommandEvent& event)
{
	if(cmdCtrl->GetValue().empty())
		return;
	sendMsg(cmdCtrl->GetValue().c_str());
	cmdCtrl->SetValue(wxT(""));
	sendBtn->Disable();
}

void Frame::OnCmdUpdate(wxCommandEvent& event)
{
	if(cmdCtrl->GetValue().empty())
		sendBtn->Disable();
	else
		sendBtn->Enable();
}

void Frame::OnUserSelect(wxCommandEvent& event)
{
	if(userListCtrl->GetSelection() == wxNOT_FOUND)
		return;
	user_t* usr = getUser(userListCtrl->GetStringSelection().c_str());
	if(!usr)
		return;

	useKeyCtrl->SetValue(usr->encrypt == '1');
	keyCtrl->SetValue(usr->encryptKey);
	sendToUser(usr->userName);
}

void Frame::OnKeyEnter(wxCommandEvent& event)
{
	user_t* usr = getUser(userListCtrl->GetStringSelection().c_str());
	if(!usr)
		return;
	strncpy(usr->encryptKey, keyCtrl->GetValue(), 140);
}

void Frame::OnToggleKey(wxCommandEvent& event)
{
	if(useKeyCtrl->IsChecked())
	{
		user_t* usr = getUser(userListCtrl->GetStringSelection().c_str());
		if(!usr)
			return;
		usr->encrypt = '1';
		strncpy(usr->encryptKey, keyCtrl->GetValue(), 140);
	}else{
		user_t* usr = getUser(userListCtrl->GetStringSelection().c_str());
		if(usr)
			usr->encrypt = '0';
	}
	
}

void Frame::OnBcast(wxCommandEvent& event)
{
	bcstHello();
}

void Frame::OnDropUser(wxCommandEvent& event)
{
	if(userListCtrl->GetSelection() == wxNOT_FOUND)
		return;
	dropUser(userListCtrl->GetStringSelection().c_str());
	userListCtrl->Delete(userListCtrl->GetSelection());
}

void Frame::OnIncSeqNum(wxCommandEvent& event)
{
	setIncSeqNum(event.IsChecked());
}

void Frame::OnSetTOVal(wxCommandEvent& event)
{
	// do dialog box to set TO value
	wxDialog* diag = new wxDialog(0, wxID_ANY, wxT("Set Timeout"), 
		wxDefaultPosition, wxSize(130, 70));
	
	char a[8];
	sprintf(a, "%-d", getTOVal());

	wxTextCtrl* txtCtrl = new wxTextCtrl(diag, wxID_ANY, a, 
		wxPoint(10, 10), wxSize(60, 20));
	txtCtrl->SetMaxLength(5);
	wxButton* okBtn = new wxButton(diag, wxID_OK, wxT("OK"), 
		wxPoint(80, 10), wxSize(40, 20));

	diag->Center();
	if(diag->ShowModal() == wxID_OK)
		setTOVal(atoi(txtCtrl->GetValue()));

	diag->Destroy();
}

void Frame::OnAbout(wxCommandEvent& event)
{
    wxString versionStr;
    
    if(SMOKESIGNAL_VERSION_PATCH == 0) {
        versionStr = wxString::Format("%d.%d",
            SMOKESIGNAL_VERSION_MAJOR,
            SMOKESIGNAL_VERSION_MINOR);
    } else {
        versionStr = wxString::Format("%d.%d.%d",
            SMOKESIGNAL_VERSION_MAJOR,
            SMOKESIGNAL_VERSION_MINOR,
            SMOKESIGNAL_VERSION_PATCH);
    }

	wxMessageDialog* diag = new wxMessageDialog(0, 
		wxT("Copyright (c) 2010 Joseph Kuziel and Micca Osmar"), 
		wxT("SmokeSignal v" + versionStr), wxOK);
	diag->ShowModal();
}

void Frame::OnUserContext(wxContextMenuEvent& event)
{
	wxMenu* menu = new wxMenu;
	menu->Append(ID_DROP_USER, wxT("Drop User"));
	userListCtrl->PopupMenu(menu, userListCtrl->ScreenToClient(
		event.GetPosition()));
}

void Frame::OnNewUser(const char* usr)
{
	me->userListCtrl->AppendString(usr);
}

void Frame::OnRemoveUser(const char* usr)
{
	int i = me->userListCtrl->FindString(usr);
	if(i == wxNOT_FOUND)
		return;
	me->userListCtrl->Delete(i);

}


#endif // SMOKESIGNAL_UI_WXWIDGETS
