/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSConPCD service header
*
*/


#ifndef _SCONPCD_H_
#define _SCONPCD_H_

// INCLUDES

#include <e32base.h>
#include <e32cons.h>
#include <s32strm.h>

#include "sconconmltask.h"

class CSConInstallerQueue;
class CSConBackupRestoreQueue;
class CSconSyncHandler;

enum TSConLastConMLOperation
    {
    ENoTask = 0,
    EPutTask,
    EGetReply
    };

//============================================================
// Class CSConPCD declaration
//============================================================
class CSConPCD : public CBase
    {
    public:
        /**
         * Two-phase constructor
         * @return CSConPCD instance
         */
        static CSConPCD* NewL();
        /**
         * Destructor
         * @return none
         */
        ~CSConPCD();
        /**
         * Receives a new task
         * @param aTask A new task
         * @return KErrNone if no errors, else system wide error codes
         */
        virtual TInt PutTaskL( CSConTask*& aTask );
        /**
         * Returns a reply to previous PutTaskL operation
         * @return CSConStatusReply if reply exists, else return NULL
         */
        virtual CSConStatusReply* GetReply();
        /**
         * Resets the module to the initial state
         * @return none
         */
        virtual void ResetPCD();
        
        virtual void HandleGetSyncRequestL( const TDesC8& aRequest, RWriteStream& aResult, TInt aMaxObjectSize );
        virtual void HandlePutSyncRequestL( const TDesC8& aRequest, RReadStream& aResult );
    
    private:        
        /**
         * Collects a status of a specified task / all tasks
         * @param aTask The number of the specified task
         * @param aAll If ETrue => receive status of all tasks
         * @return KErrNone if no errors, else system wide error codes
         */
        TInt GetStatusL( TInt aTask, TBool aAll );
        
        /**
         * Cancels a specified task / all tasks
         * @param aTask The number of the specified task
         * @param aAll If ETrue => receive status of all tasks
         * @return KErrNone if no errors, else system wide error codes
         */
        TInt CancelL( TInt aTask, TBool aAll );
        
        /**
         * Fills a reply with predefined supported task -information
         * @param aTask The task
         * @return KErrNone if no errors, else system wide error codes
         */
        TInt SupportedInstTaskL( CSConTask* aTask );
        
        /**
         * Fills a reply with predefined supported task -information
         * @param aTask The task
         * @return KErrNone if no errors, else system wide error codes
         */
        TInt SupportedBRTaskL( CSConTask* aTask );
        
        /**
         * Fills a reply with predefined Reboot task -information
         * @param aTask The task
         * @return KErrNone if no errors, else system wide error codes
         */
        TInt RebootTaskL( const CSConTask* aTask );
        
        /**
         * Fills a reply with predefined GetMetadata task -information
         * @param aTask The task
         * @return none
         */
        void GetMetadataTaskL( CSConTask& aTask );
        
        /**
         * Creates a reply for UpdateDeviceInfo task
         * @param aMaxObjectSize A max object size to be used
         * @return none
         */
        TInt UpdateDeviceInfoL( TInt aMaxObjectSize );
        
        /**
         * Stores a reply
         * @param aReply The reply to be stored
         * @return none
         */
        void StoreReply( CSConStatusReply*& aReply );
        
        /**
         * Constructor
         * @return none
         */
        CSConPCD();
        /**
         * Initializes member data
         * @return none
         */
        virtual void ConstructL();
        
    private:
        CSConInstallerQueue*        iInstallerQueue;
        CSConBackupRestoreQueue*    iBackupRestoreQueue;
        CSConStatusReply*           iLatestReply;
        TInt                        iTaskNumber;
        TSConLastConMLOperation     iLastOperation;
        RFs                         iFs;
        CSconSyncHandler*           iSyncHandler;
    }; 
    
    IMPORT_C CSConPCD* CreateCSConPCDL();
    typedef CSConPCD* (*TSConCreateCSConPCDFunc) (); 

#endif // SCONPCD

// End of file
