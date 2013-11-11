/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Operation class for the deletion of SMTP and related 
*                POP3/IMAP4 service entries
*
*/


// NOT NEEDED

#ifndef IPSPLGSERVICEDELETIONOP_H
#define IPSPLGSERVICEDELETIONOP_H

/*#include <msvapi.h>
#include <msvipc.h>
#include "ipsplgbaseoperation.h"*/

/**
*
*/
/*class CIpsPlgServiceDeletionOp :
    public CMsvOperation
    {
    public:*/
    
        /**
        *
        */
        /*static CIpsPlgServiceDeletionOp* NewL(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TFSMailMsgId aFSMailboxId );*/

        /**
        *
        */
        //~CIpsPlgServiceDeletionOp();

        /**
        *
        */
       // virtual const TDesC8& ProgressL();

        /**
        *
        */
       // virtual const TDesC8& GetErrorProgressL(TInt aError);
        
    //protected:
    
        /**
        * From CActive
        */
       // virtual void RunL();

        /**
        * From CActive
        */
       // virtual void DoCancel();

   // protected:
    
        /**
        *
        */
        /*CIpsPlgServiceDeletionOp(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus );*/

        /**
        *
        */
       // void ConstructL( TFSMailMsgId aFSMailboxId );

        /**
        * Kick off an entry deletion and go active.
        */
      //  void BeginDeleteOperationL(TMsvId aServiceId);

        /**
        *
        */
        //void DoRunL();

        /**
        * Complete the observer and update internal state.
        */
       // void Complete();

        /**
        * Do our best to make sure the deletion is still carried
        * out if an error occurs.
        */
       // void ForceServiceRemoval();

        
        /**
        * Removes the account from the Central Repository
        * @since S60 3.0
        */
      //  void BeginRemoveAccountDeleteOpL();
        
        /**
        *
        */
     //   void RemoveAOAgentL();
        
   /* private:
    
        
        enum TState
            { EDeleteAccountSettings,
              EDeletePopImap,
              EDeleteSmtp,
              EStateComplete,
              EIdle };
              
        TState                          iState;
        CMsvOperation*                  iSubOperation;
        CMsvEntry*                      iRootEntry;
        TBuf8<KMsvProgressBufferLength> iProgressBuf;
        TInt                            iError;        
        TMsvEntry                       iTEntry;//account index entry
    };*/


#endif // IPSPLGSERVICEDELETIONOP_H
