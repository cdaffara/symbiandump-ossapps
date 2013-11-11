/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CCONFIRMATIONQUERY_H
#define CCONFIRMATIONQUERY_H
	
#include <e32base.h>
#include <e32std.h>

#include "mconfirmationnotify.h"

class CConfirmationQuery : public CActive
{
public:
	
	~CConfirmationQuery();
	
    static CConfirmationQuery* NewL( MConfirmationNotify& aNotify );  

    static CConfirmationQuery* NewLC( MConfirmationNotify& aNotify );  
    
    void CConfirmationQuery::StartWaitingAnswer( );
        
protected: // from CActive 

	void RunL();		
	
	void RunError();
	
	void DoCancel();
    
protected: // construction

	CConfirmationQuery(MConfirmationNotify& aNotify);
	
	void ConstructL();

private:

	MConfirmationNotify& iNotify;
	TBool iCancelledByUser;
};


#endif // CCONFIRMATIONQUERY_H

//  End of File
