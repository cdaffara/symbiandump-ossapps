/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef CBTPASSWORDQUERY_H
#define CBTPASSWORDQUERY_H

#include <e32base.h>
#include <e32std.h>
#include <btnotif.h>

#include "mconfirmationnotify.h"

class CBtPasswordQuery : public CActive
{
public:
	
	~CBtPasswordQuery();
	
    static CBtPasswordQuery* NewL( MConfirmationNotify& aNotify );  

    static CBtPasswordQuery* NewLC( MConfirmationNotify& aNotify );  

    void StartActiveObjectL ();
    
protected: // from CActive 

	void RunL();		
	
	void RunError();
	
	void DoCancel();
    
protected: // construction

	CBtPasswordQuery(MConfirmationNotify& aNotify);
	
	void ConstructL();

private:

	MConfirmationNotify& iNotify;
	
    TBTObexPasskey iPckg;		// Input parameters
	
    TObexPasskeyBuf iResultPckg; // Response
	
	RNotifier iNotifier;
	
};

#endif // CBTPASSWORDQUERY_H

//  End of File
