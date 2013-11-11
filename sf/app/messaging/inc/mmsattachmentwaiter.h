/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Attachment Waiter for MMS Client MTM and UI.
*
*/



#ifndef __MMSATTACHMENTWAITER_H__
#define __MMSATTACHMENTWAITER_H__

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CMsvStore;
class MMsvAttachmentManager;

// CLASS DECLARATION

/**
*  CMmsAttachmentWaiter
*
* @lib mmsgenutils.lib
* since 3.2
*/
NONSHARABLE_CLASS( CMmsAttachmentWaiter ) : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMmsAttachmentWaiter* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CMmsAttachmentWaiter();
    
    public: // New functions

        /**
        * Start waiting for asychronous attachment handling functions to complete.
        * @param aStatus caller's request statut to complete when done
        * @param aStore pointer to store associated with the entry the attachment belongs to
        *    When the operation completes, the waiter commits the store and deletes the pointer
        * @param aAttachmentManager reference to the attachment manager handling the entry
        */
        IMPORT_C void StartWaitingL( TRequestStatus& aStatus, CMsvStore* aStore, MMsvAttachmentManager* aAttachmentManager );
    
private:
    CMmsAttachmentWaiter();
    
    void Reset();
    
    // from CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
    
private:
    TRequestStatus* iReportStatus;
    CMsvStore* iStore;
    MMsvAttachmentManager* iAttachmentManager;
    };

#endif // __MMSATTACHMENTWAITER_H__
