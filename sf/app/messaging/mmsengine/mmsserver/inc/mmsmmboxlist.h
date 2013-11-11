/*
* Copyright (c) 2004 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   State machine for mmbox list
*
*/




#ifndef MMSMMBOXLIST_H
#define MMSMMBOXLIST_H

//  INCLUDES
#include "mmsbaseoperation.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
*  State machine for mmbox list
*
*  @lib mmssrv.lib
*  @since 2.8
*/
class CMmsMmboxList : public CMmsBaseOperation
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMmsMmboxList* NewL( RFs& aFs, CMmsSettings* aMmsSettings );
        
        /**
        * Destructor.
        */
        virtual ~CMmsMmboxList();

    public: // New functions

        

    public: // Functions from base classes

        /**
        * Start the state machine.
        * @param aSelection list of message entry ids to be handled.
        * @param aServerEntry CMsvServerEntry pointer from CMmsServer
        * @param aSettings MMSC settings (entry points)
        * @param aService current MMS service id
        * @param aStatus TRequestStatus of the calling active object
        */
        void StartL(
            CMsvEntrySelection& aSelection,
            CMsvServerEntry& aServerEntry,
            TMsvId aService,
            TRequestStatus& aStatus );


        
    protected:  // New functions
        
        
    protected:  // Functions from base classes
        
        /**
        * From CMsgActive: Active object cancellation
        */
        void DoCancel();

    private: // New functions

        /**
        * Default constructor
        */
        CMmsMmboxList( RFs& aFs );

        /**
        * Set flags for a new entry
        */
        void SetFirstFlagsToNewEntry( TMsvEntry& aEntry );
      
        /**
        * Set flags for an entry being finalized
        */
        void SetFlagsToEntryBeingFinalized( TMsvEntry& aEntry );

        /**
        * Remove old notifications from mmbox folder
        */
        TInt RemoveOldNotifications();

        /**
        * New notifications become visible
        */

        TInt MakeNewNotificationsVisible();

        /**
        * Store content subject, sender etc. to notification entry
        */
        TInt StoreContentToNotificationEntryL( TMsvEntry aEntry );

        /**
        * Get old quota entry id
        */
        TMsvId OldQuotaEntryL();
        
        /**
        * The server time of the mmbox notification. 
        */
        TTime ServerDate(); 
        
        /**
        * Local mode fetch
        */
        void LocalModeFetchL();


    private:

        void ConstructL( CMmsSettings* aMmsSettings );

        /**
        * Functions from CMmsBaseOperation 
        */

        /**
        * Encode the m-mbox-view.req PDU to be sent to MMSC.
        */
        void EncodePDUL();

        /**
        * Submit a POST transaction.
        */
        void SubmitTransactionL();

        
        /**
        * Create new entry to receive data from MMSC.
        */
        void CreateEntryL();

        /**
        * Decode response m-mbox-view.conf and many
        * m-mbox-desc PDU from MMSC.
        */
        void DecodeResponseL();

        /**
        * Move trigger entry after handling.
        * Remove old notifications from mmbox folder.
        * New notifications become visible in mmbox folder.
        * Remove old quota entry. Finalize new quota entry.
        */
        void MoveEntryL();

        /**
        * Disconnect and do any other finalizing steps.
        */
        void FinalizeL();


    public:     // Data
    protected:  // Data
    private:    // Data

        CMmsHeaders* iMmsHeaders; // mms headers
        TMsvId iEntryUnderConstruction; // when list is updated the new quota info will be in this entry.
                                        
        CMsvEntrySelection* iOldNotifications; // notifications in mmbox folder, when updating begins
        TMsvId iMmboxFolder; // mmbox folder entry
        TMsvId iOldQuotaEntryId; // old quota entry
    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };

#endif      // MMSMMBOXLIST
            
// End of File
