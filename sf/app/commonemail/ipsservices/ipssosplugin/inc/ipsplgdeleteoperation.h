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
* Description:  Deletes messages first locally and then from server
*
*/
//<qmail> class renamed
#ifndef IPSPLGDELETEOPERATION_H
#define IPSPLGDELETEOPERATION_H
//</qmail>
//  INCLUDES
#include <e32def.h>
#include <e32des8.h>    // TDesC8

class CMsvOperation;
class CMsvEntry;
class CMsvEntrySelection;
class CMsvSession;
class TRequestStatus;

/**
* class CIpsPlgDeleteOperation
* Encapsulates delete locally operation and delete from server operation.
* First deletes message locally and after it has been completed, deletes from the server
*/
//<qmail>
NONSHARABLE_CLASS ( CIpsPlgDeleteOperation ) : public CMsvOperation
//</qmail>        
    {
    public:
        // <qmail>
        /**
        * Two-phased constructor
        * 
        * @param aMsvSession                client/server session to MsvServer
        * @param aObserverRequestStatus     operations observer status
        * @param aEntriesToDelete           Message entries to be deleted from server and locally  
        *
        * @return CIpsPlgCreateForwardMessageOperation* self pointer                                  
        */
        static CIpsPlgDeleteOperation* NewL(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            CMsvEntrySelection* aEntriesToDelete );
        
        /**
        * Two-phased constructor
        * 
        * @param aMsvSession                client/server session to MsvServer
        * @param aObserverRequestStatus     operations observer status
        * @param aEntriesToDelete           Message entries to be deleted from server and locally
        * @param aOperationObserver         observes the progress of this operation
        * @param aRequestId                 identifier for this instance of operation, 
        *                                   assigned by the client
        * @return CIpsPlgCreateForwardMessageOperation* self pointer                                  
        */
        static CIpsPlgDeleteOperation* NewL(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            CMsvEntrySelection* aEntriesToDelete,
            MFSMailRequestObserver& aOperationObserver,
            const TInt aRequestId);
        // </qmail>    
//<qmail> class renamed + comments removed
        virtual ~CIpsPlgDeleteOperation();
        
        virtual const TDesC8& ProgressL();
//</qmail> 

// <qmail>
    protected:        
        void SignalFSObserver( TInt aStatus );
// </qmail>
        
    private:
        // <qmail>   
        enum TIpsPlgDeleteMessagesState 
            {
            ESetFlags = 0,    
            ELocally,
            EFromServer
            };
        // </qmail>

        /**
        * Constructor.
        */
		//<qmail> class renamed
        CIpsPlgDeleteOperation(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus );
		//</qmail>
        //<qmail>
        CIpsPlgDeleteOperation(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            MFSMailRequestObserver& aOperationObserver,
            const TInt aRequestId);
        //</qmail>
        // <qmail> 
        /**
        * 2nd phase constructor
        */
        void ConstructL( CMsvEntrySelection* aEntriesToDelete );
        // </qmail>
        
        /**
        * From CActive
        */
        void DoCancel();
        
        /**
        * From CActive
        */
        void RunL();

        /**
        * Starts new local delete progress
        */
        void StartNextDeleteLocally();
        //<qmail>
        /**
        * Creates object of CIpsPlgDeleteLocal
        */
        void MakeNextDeleteLocallyL();
        //</qmail>

        /**
        * Starts entry delete after local delete is completed.
        */
        void StartDeleteFromServer();

        /**
        * Creates delete operation
        */
        void MakeDeleteFromServerL();

		// <qmail>    
        /**
        * Sets the locally deleted flag for the next item (iSetFlagIndex)
        * return true if operation started successfully, otherwise false
        */
        TBool SetNextLocallyDeletedFlagL();
		// </qmail>    

    private:
        // <qmail>
        CMsvOperation*              iOperation; // owned
        CMsvEntry*                  iEntry; // owned
        CMsvEntrySelection*         iEntrySelection; // owned
        MFSMailRequestObserver* iOperationObserver;  // not owned
        TFSProgress iFSProgress;
        TInt iFSRequestId;
        // <qmail> removed member iEntryCount
        TInt                        iSetFlagIndex;
        CMsvEntry*                  iSetFlagEntry; // owned
        // <qmail> removed member iBlank
		//<qmail> enum renamed
        TIpsPlgDeleteMessagesState  iState;
        // </qmail>
        
    };

#endif      // IPSPLGDELETEREMOTE_H

// End of File
