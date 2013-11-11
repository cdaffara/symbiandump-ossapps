/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


//  INCLUDES
#ifndef __CLEARLISTTEST_H__
#define __CLEARLISTTEST_H__
#include <E32Base.h>
#include <badesca.h>
#include <logwrap.h>
#include <logcli.h>

//  CLASS DECLARATION


class CClearlisttest : public CActive
{
    public:    // Constructors and destructor   
    
        static CClearlisttest* NewL();

        ~CClearlisttest();
            
public: // From CActive
    	void RunL ();
    	void DoCancel ();			
                                                                     
    private:

        CClearlisttest();
        void ConstructL();
        TInt ClearList();
        
    private: // data
    	HBufC* iDllName;
    	TLogId iId;
			TUid iEventType;
			TTime iTime;
			TLogDurationType iDurationType;
			TLogDuration iDuration;
			TLogContactItemId  iContact;
			TLogLink iLink;
			TLogFlags iFlags;
			HBufC* iDescription;
			HBufC* iRemoteParty;
			HBufC* iDirection;
			HBufC* iCallStatus;
			HBufC* iSubject;
			HBufC* iNumber;
			HBufC8* iData;
      TInt iCompleteError;
      //
      CLogClient* iLogClient;  
      RFs    iFsSession;
      CLogEvent* iLogEvent;
      RPointerArray<CLogEvent> iLogEvents;
    private:    // Friend classes               
    // none                                     

};

// End of File __CLEARLISTTEST_H__
#endif