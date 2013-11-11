/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSConInboxHandler header file
*
*/


#ifndef _SCONINBOXHANDLER_H_
#define _SCONINBOXHANDLER_H_

// INCLUDES

#include <e32base.h>
#include <e32cons.h>
#include <msvapi.h>


//============================================================
// Class CSConInboxHandler declaration
//============================================================
NONSHARABLE_CLASS( CSConInboxHandler ) : public CBase, MMsvSessionObserver
    {
    public:
        static CSConInboxHandler* NewL();
        virtual ~CSConInboxHandler();
        
        /**
         * Creates an attachment to device's Inbox
         * @param aObject CObexBufObject object
         * @return KErrNone if no errors, else system wide error codes.
         */
        TInt CreateInboxAttachmentL( CObexBufObject*& aObject, 
                                        CBufFlat*& aBuffer );
        /**
         * Saves the object to the Inbox
         * @param aObject CObexBufObject object
         * @return KErrNone if no errors, else system wide error codes.
         */
        TInt SaveObjToInboxL( CObexBufObject*& aObject );
        /**
         * Aborts the storing operation
         * @param aObject CObexBufObject object
         * @return KErrNone if no errors, else system wide error codes.
         */
        TInt AbortInboxOperation( CObexBufObject*& aObject );
        /**
         * Implementation of MMsvSessionObserver::HandleSessionEventL
         * @param aEvent Indicates the event type. 
         * @param aArg1 Event type-specific argument value 
         * @param aArg2 Event type-specific argument value 
         * @param aArg3 Event type-specific argument value
         * @return none
         */
        void HandleSessionEventL( TMsvSessionEvent aEvent, 
                                TAny* aArg1, 
                                TAny* aArg2, 
                                TAny* aArg3 );
        
    private:
        CSConInboxHandler();
        
        TInt GetMessageCenterDriveL();
        
    private:
        TInt            iDrive;
        TMsvId          iMsvIdParent;
        TMsvId          iMsvIdAttach;
        RFile           iFile;          
};

#endif

// End of file
