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
* Description:  
*      CMsvOperation class for MMS message creation.
*
*/



#ifndef CMMSMESSAGEOPERATION_H
#define CMMSMESSAGEOPERATION_H

//  INCLUDES
#include <mtclbase.h>   // TMsvPartList
#include <msvapi.h>     // CMsvOperation etc

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class TRequestStatus;
class CMmsHeaders;
class CMmsSettings;

// CLASS DECLARATION

/**
*  CMsvOperation class for MMS message creation.
*
*  Currently this supports creation of Reply, Forward and New message entry
*  to the desired destination folder. This class is used by MMS Client MTM, 
*  but can be used by other client application modules too. 
*
*  CMmsMessageOperation is derived from CMsvOperation class and can therefore 
*  supply progress information to a caller while the asynchronous operation is 
*  being carried out.
* 
*  The progress information provides the id of the created message when the 
*  message has been created in a 8-bit descriptor. While the operation is in 
*  progress the package will contain a null id (KMsvNullIndexEntryId). 
*  If there was an error while creating the message, then the message will be 
*  deleted and the package will contain null id.
*
*  The progress will be packaged into an 8-bit descriptor in the normal manner, 
*  as described by the CMsvOperation API.
*/
class CMmsMessageOperation :public CMsvOperation
    {
    public:  // Constructors and destructor
                
        /**
        * Destructor.
        */
        IMPORT_C ~CMmsMessageOperation();

    public: // New functions
        
        /**
        * Crete new message entry.
        * @param aObserverRequestStatus observer's request status.
        * @param aMsvSession message server session.
        * @param aDestination destination folder id.
        * @param aServiceId MMS service id. 
        * @param aVisible if the new entry is visible.
        * @param aInPreparation if the new entry is left under construction.
        * @return this message operation object.
        */
        IMPORT_C static CMmsMessageOperation* CreateNewL(
            TRequestStatus& aObserverRequestStatus,
            CMsvSession& aMsvSession,
            TMsvId aDestination,
            TMsvId aServiceId,
            TBool  aVisible = EFalse,
            TBool  aInPreparation = ETrue );

        /**
        * Crete new reply message entry.
        * @param aObserverRequestStatus observer's request status.
        * @param aMsvSession message server session.
        * @param aMessageId original message id
        * @param aDestination destination folder id
        * @param aPartList message part list included in the new entry
        * @param aServiceId MMS service id 
        * @param aVisible if the new entry is visible.
        * @param aInPreparation if the new entry is left under construction.
        * @return this message operation object.
        */
        IMPORT_C static CMmsMessageOperation* CreateReplyL(
            TRequestStatus& aObserverRequestStatus,
            CMsvSession& aMsvSession,
            TMsvId aMessageId,
            TMsvId aDestination,
            TMsvPartList aPartList,
            TMsvId aServiceId,
            TBool  aVisible = EFalse,
            TBool  aInPreparation = ETrue );

        /**
        * Crete new forward message entry.
        * @param aObserverRequestStatus observer's request status.
        * @param aMsvSession message server session.
        * @param aMessageId original message id
        * @param aDestination destination folder id
        * @param aPartList message part list included in the new entry
        * @param aServiceId MMS service id 
        * @param aVisible if the new entry is visible.
        * @param aInPreparation if the new entry is left under construction.
        * @return this message operation object.
        */
        IMPORT_C static CMmsMessageOperation* CreateForwardL(
            TRequestStatus& aObserverRequestStatus,
            CMsvSession& aMsvSession,
            TMsvId aMessageId,
            TMsvId aDestination,
            TMsvPartList aPartList,
            TMsvId aServiceId,
            TBool  aVisible = EFalse,
            TBool  aInPreparation = ETrue );

        /**
        * Crete new forward without retrieval entry.
        *     This function is used only for forwarding a message based on a received
        *     notification without fetching the whole message into the terminal.
        *     This is a static and synchronous function.
        * @param aMsvSession message server session.
        * @param aContentLocation of the related notification
        * @param aRelatedEntry entryId of the notification that this forward refers to
        * @param aDestination destination folder id
        * @param aServiceId MMS service id 
        * @param aVisible if the new entry is visible.
        * @param aInPreparation if the new entry is left under construction.
        * @return entry id of the newly created entry
        */
        IMPORT_C static TMsvId CreateForwardWithoutRetrievalL( 
            CMsvSession& aMsvSession,
            const CMmsHeaders& aNotificationHeaders,
            const TMsvId aRelatedEntry,
            const TMsvId aDestination,
            TMsvId aServiceId,
            TBool  aVisible = EFalse,
            TBool  aInPreparation = ETrue );

        /**
        * This returns the id of the created message if a message was created
        * packaged into an 8-bit descriptor. If there was an error while creating
        * the message, then the message will be deleted and the package will
        * contain a null id.
        * @return the id of the created message packaged into an 8-bit descriptor.
        */
        IMPORT_C const TDesC8& FinalProgress();

    public: // Functions from base classes

        /**
        * Active object completion
        */
        void RunL();

        /**
        * Active object cancellation
        */
        void DoCancel();

        /**
        * Use this function to get information on the progress of the operation.
        * @return descriptor holding progress information (message id).
        */
        const TDesC8& ProgressL();
        
    protected:  // New functions

    protected:  // Functions from base classes

    private:

        // Different operations that this class is able to perform
        // NOTE: Forward without retrieval operation is not in this list
        //       because it is a static and synchronous operation.
        enum TMmsOperation
            {
            ENew,
            EReply,
            EForward
            };

        // Possible states
        // NOTE: forward without retrieval operation is synchronous and
        //       is not related to these
        enum TMmsOperationState
            {
            ECreateMmsHeaders,
            ECreateMessageEntry,
            EStoreMmsHeaders,
            ECopyAttachments,
            ECompleteMessage,
            EFinished
            };

        /**
        * Symbian OS constructor is private.
        * @param aMessageId original message id
        * @param aOperation what to do.
        */
        void ConstructL(
            TMsvId aMessageId, 
            TMmsOperation aOperation );

        /**
        * C-style constructor.
        * @param aObserverRequestStatus observer's request status
        * @param aMsvSession message server session
        * @param aDestination destination folder id
        * @param aPartList message part list included in the new entry
        * @param aServiceId MMS service id
        * @param aVisible if the new entry is visible.
        * @param aInPreparation if the new entry is left under construction.
        */
        CMmsMessageOperation( 
            TRequestStatus& aObserverRequestStatus,
            CMsvSession& aMsvSession,
            TMsvId aDestination,
            TMsvPartList aPartList,
            TMsvId aServiceId,
            TBool  aVisible,
            TBool  aInPreparation );

        /**
        * Tidy up environment after error, and free memory.
        * @param aError error code to be returned
        */
        void ErrorRecovery( TInt aError );

        /**
        * Change the state of this operation.
        */
        void ChangeStateL();

        /**
        * Select the next state of this operation.
        */
        void SelectNextStateL();

        /**
        * Do the operations and change the state.
        */
        void SelectAndChangeToNextStateL();

        /**
        * Signal the completion of a request. ( one step ) 
        */
        void RequestComplete( TInt aError );

        /**
        * Create MMS headers. Set default values from MMS service entry.
        */
        void CreateNewMmsHeaderL();

        /**
        * Create new message entry.
        */
        void CreateMessageEntryL();

        /**
        * Store MMS headers of the message
        */
        void StoreMmsHeadersL();

        /**
        * Copy attachments entries and binary files from original message entry.
        */
        void CopyAttachmentsL();

        /**
        * Complete the message. Count the message size and set flags.
        */
        void CompleteMessageL();

        /**
        * Count the total size of attachments
        */
        void AttachmentsSizeL();

    public:     // Data
    
    protected:  // Data

    private:    // Data
        TInt                iState;
        TPckgBuf<TMsvId>    iDataMember;
        TMsvId              iNewMessageId;
        CMsvOperation*      iMsvOperation;
        TMsvId              iDestinationId;
        TMsvPartList        iPartList;
        TMsvId              iOrigMessageId;
        TMmsOperation       iOperation;
        TMsvId              iServiceId;
        CMmsSettings*       iSettings;
        CMmsHeaders*        iNewMmsHeaders;
        TBool               iVisible;
        TBool               iInPreparation;
        TInt                iAttachmentsSize;
        TInt                iMmsHeaderSize;
        HBufC*              iDescription;   // message description
        TMsvAttachmentId    iOriginalRoot;  // to retain root when forwarding
        TInt                iNewRoot;       // index of root in new entry
        // Separate source and target entry and store are needed
        // for hanling attachments using the new attachment manager.
        CMsvStore*          iSourceStore;   // need to keep open to copy attachments
        CMsvStore*          iTargetStore;   // need to keep open to copy attachments
        CMsvEntry*          iSourceEntry;
        CMsvEntry*          iTargetEntry;
        TInt                iAttachmentCount; //number of attachments to be handled
        TInt                iCurrentAttachment; // index of attachmnet being handled

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // CMMSMESSAGEOPERATION_H  
            
// End of File
