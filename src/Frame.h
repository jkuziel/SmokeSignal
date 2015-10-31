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


#ifndef __FRAME_H__
#define __FRAME_H__


#include "StreamTextCtrl.h"
#include <wx/wx.h>
#include <iostream>


class Frame : public wxFrame
{
	// Control IDs
	enum CTRLID {
		ID_NONE = 0,
		ID_MSG_TB,
		ID_SEND_BTN,
		ID_CMD_TB,
		ID_USERNAME_TB,
		ID_USER_LB,
		ID_USEKEY_CB,
		ID_KEY_TB,
		ID_BCAST,
		ID_DROP_USER,
		ID_INC_SEQ_NUM,
		ID_SET_TO_VAL,
		ID_ABOUT
	};

	static Frame* me;

	protected:

		// Chat Box
		StreamTextCtrl* msgCtrl;
  
		// Message Box
		wxTextCtrl* cmdCtrl;

		// Send Button
		wxButton*	sendBtn;

		// Users Online List Box
		wxListBox*	userListCtrl;

		// Encryption Key Check Box
		wxCheckBox*	useKeyCtrl;

		// Encryption Key Text Box
		wxTextCtrl*	keyCtrl;

	public:
		Frame(const wxString& title);

		/*
		 * Program exits
		 */
		void OnExit(wxCommandEvent& event);

		/*
		 * On send button click
		 */
		void OnSend(wxCommandEvent& event);

		/*
		 * On message box input
		 */
		void OnCmdUpdate(wxCommandEvent& event);

		/*
		 * On users online list box selection change
		 */
		void OnUserSelect(wxCommandEvent& event);

		/*
		 * User presses enter while encryption key box is focused
		 */
		void OnKeyEnter(wxCommandEvent& event);

		/*
		 * On use encryption key checkbox clicked
		 */
		void OnToggleKey(wxCommandEvent& event);

		/*
		 * Actions -> Say Hello menu item selected
		 */
		void OnBcast(wxCommandEvent& event);

		/*
		 * Actions -> Drop User menu item selected
		 */
		void OnDropUser(wxCommandEvent& event);

		/*
		 * Options -> Increment Sequence Number menu item selected
		 */
		void OnIncSeqNum(wxCommandEvent& event);

		/*
		 * Options -> Set ACK Timeout Value...
		 */
		void OnSetTOVal(wxCommandEvent& event);

		/*
		 * Help -> About
		 */
		void OnAbout(wxCommandEvent& event);

		/*
		 * User right clicks in Users Online list box
		 */
		void OnUserContext(wxContextMenuEvent& event);

		/*
		 * New user callback function
		 */
		static void OnNewUser(const char*);

		/*
		 * Remove user callback function
		 */
		static void OnRemoveUser(const char*);
};


#endif // __FRAME_H__

#endif // SMOKESIGNAL_UI_WXWIDGETS
