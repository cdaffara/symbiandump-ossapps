/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Contact Parser Client
*
*/



#if !defined(__CNTPARSERCLIENT_H__)
#define __CNTPARSERCLIENT_H__

#include <e32base.h>
#include <e32std.h>

enum TCntParserConnection
{
	ECntParserNotConnected,
	ECntParserConnected
};

//**********************************
//RCntParserServer
//**********************************

class RCntParserServer : public RSessionBase
{
	public:
	IMPORT_C	RCntParserServer();
	IMPORT_C	TInt Connect();
	IMPORT_C	TInt Disconnect();
	IMPORT_C	TVersion Version() const;
	IMPORT_C	TInt CreateIrMCL2PhoneBook(TInt aMaxNumberOfContacts, TBool aForced);
	IMPORT_C	TInt ContactsParsed();
	IMPORT_C	TInt ContactsSaved();
	IMPORT_C	TInt TotalNumberOfContacts();
	IMPORT_C	TInt Cancel(); 		//cancels phonebook request
	IMPORT_C	TInt CancelAll(); 	//cancels all request

	private:
	TCntParserConnection	iConnected;
};


#endif

