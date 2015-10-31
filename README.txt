SmokeSignal v1.1 README

Copyright (c) 2010-2013 Joseph Kuziel & Micca Osmar
--------------------------------------------------------------------------------

DESCRIPTION
	A chat client adhering to the LocalChat protocol.
	
DOWNLOAD
	Source: https://github.com/jkuziel/SmokeSignal

DEPENDENCIES
	CMake 2.8+			http://www.cmake.org/
	wxWidgets 2.9.4  	http://www.wxwidgets.org
	C++11 compatible compiler:
		* Visual Studio 2012+
		* GCC 4.7+

BUILDING FROM SOURCE
	1. Download and build wxWidgets
	2. Run CMake GUI or run cmake from the command line in the src directory
	3. Build resulting project/make file
	
INSTALLATION
	No installation necessary. Run program from directory.
	
STARTUP
	Your user name will appear in parentheses in the title bar to the right
	of the SmokeSignal name. The user name can be changed in the config
	file. The program must be restarted after the config file is edited to
	see any	changes.
	
MENU BAR
	Actions			- Lists possible program actions.
		Say Hello	- Broadcasts user name on the subnet for
				  available users.
		Drop User 	- Drop user from user list.
		Exit      	- Close the application
	Options
		Increment Sequence Number	- When checked, the sequence
						  number will be incremented by
						  one on every successfully sent
						  message.
		Set Message ACK Timeout		- Set amount of time that the
						  application will wait for each
						  message acknowledgement.
	Help
		About	- Shows a message box displaying copyright information.
								
USERS ONLINE LIST BOX
	Users will automatically appear in the Users Online list box on the
	right when they send out broadcasts. Select Actions -> Say Hello to
	broadcast to all available users on a subnet. Users running LocalChat
	standard compliant applications will be available to chat. To remove a
	user from the list while his or her name is selected, right click the
	Users Online list box and click Drop User, or select
	Actions -> Drop User. To start sending messages to a user, click on his
	or her name in the list box.
	
MESSAGE BOX
	To send a message to a selected user, type the message in the text box
	below the Chat Box and either press Enter or click Send. If message was
	not sent successfully, the failure will be displayed in the Chat Box.

CHAT BOX
	Messages to and from users will display here. When users are discovered
	on the subnet, their name is displayed on the Users Online list box and
	the Chat Box. His or her IP address will be displayed in parentheses in
	the Chat Box. "<<" next to the user name means the message was sent to
	that user. ">>" next to the user name means the message was received by
	that user.
	
ENCRYPTION KEY
	To use an encryption key for a specific user, select the user, type the
	key into the text box below the	Use Encryption Key check box and then
	press Enter or check the Use Encryption Key check box. All messages sent
	to that user will be encrypted while the check box is checked. The check
	box does not have to be checked to decrypt encrypted messages, but the
	key must be entered in the encryption key text box, otherwise encrypted
	messages will not be readable.
	
LOCALCHAT PROTOCOL
	The LocalChat protocol was developed by the students of the University
	of South Florida College of Engineering as part of a class assignment
	for Networks I. LocalChat outlines the delivery of text messages sent
	across local networks.

