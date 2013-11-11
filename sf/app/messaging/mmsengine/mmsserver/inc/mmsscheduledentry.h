/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     entry for scheduled sending of mms messages
*
*/



#ifndef MMSSCHEDULEDENTRY_H
#define MMSSCHEDULEDENTRY_H

//  INCLUDES
#include    <e32base.h>
#include    <msvrcpt.h>
#include    <msvscheduledentry.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMsvRecipient;

// CLASS DECLARATION

/**
* Recipient info for scheduled operations.
*/
class CMmsRecipient :public CMsvRecipient
    {

    public:  // Constructors and destructor

        // This is needed only because the constructor of CMsvRecipient is protected.

        /**
        * constructor.
        */
        static CMmsRecipient* NewL();

        /**
        * Destructor.
        */
        virtual ~CMmsRecipient();

public: // New functions

        /**
        * Set max retries override
        * @param aMaxRetries maximum number of retries
        */
        inline void SetMaxRetries( TInt16 aMaxRetries );

        /**
        * return max retries override
        * @return maximum number of retries
        */
        inline TInt16 MaxRetries();

public: // Functions from base classes

        /**
        * From CMsvRecipient.
        * @param aReadStream read stream
        */
        void InternalizeL(RReadStream& aReadStream);

        /**
        * From CMsvRecipient.
        * @param aWriteStream write stream
        */
        void ExternalizeL(RWriteStream& aWriteStream) const;

private:
        /**
        * By default Symbian OS constructor is private.
        */
        CMmsRecipient();                

        // By default, prohibit copy constructor
        CMmsRecipient( const CMmsRecipient& );
        // Prohibit assignment operator
        CMmsRecipient& operator= ( const CMmsRecipient& );

    private:    // Data
        TInt16 iMaxRetries; // override for max retries

    };

inline void CMmsRecipient::SetMaxRetries( TInt16 aMaxRetries )
    {
    iMaxRetries = aMaxRetries;
    }

inline TInt16 CMmsRecipient::MaxRetries()
    {
    return iMaxRetries;
    }

/**
* Scheduling info for message entries.
*/
class CMmsScheduledEntry :public CMsvScheduledEntry
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMmsScheduledEntry* NewL( const TMsvEntry& aEntry );
        
        /**
        * Destructor.
        */
        virtual ~CMmsScheduledEntry();

    public: // New functions
        
        /**
        * Get access to recipient.
        * @return reference to recipient member
        */
        inline CMsvRecipient& MmsRecipient() const;

    public: // Functions from base classes

        /**
        * From CMsvScheduledEntry Check if message can be sent.
        * Called by CMsvScheduleSend::CanSchedule
        * @param  aErrorActions
        * @param  aAction
        * @return ETrue if can send to any of the recipients of iEntry,
        *     Otherwise returns EFalse        
        */
        TBool CanSendToAnyRecipients( const CMsvSendErrorActions& aErrorActions, TMsvSendErrorAction& aAction );
        
        /**
        * From CMsvScheduledEntry Check if message can be sent.
        * Called by CMsvScheduleSend::CanSchedule
        * @param  aAction
        * @return ETrue if can send to any of the recipients of iEntry,
        *     Otherwise returns EFalse        
        */
        TBool CanSendToAnyRecipients(const TMsvSendErrorAction& aAction );
        
        /**
        * From CMsvScheduledEntry Reset retry count.
        */
        void RecipientsResetRetries();
        
        /**
        * From CMsvScheduledEntry Increase retry count.
        */
        void RecipientsIncreaseRetries();
        
        /**
        * From CMsvScheduledEntry Mark sending failed.
        */
        void RecipientsSetFailed();
        
        /**
        * From CMsvScheduledEntry Query if all sent.
        * @return ETrue, if message sent, EFalse if not
        */
        TBool RecipientsAllSent() const;
        
        /**
        * From CMsvScheduledEntry Store recipient info
        * @param editable store
        */
        void RecipientsStoreL(CMsvStore& aStore) const;

        /**
        * From CMsvScheduledEntry Restore recipient info
        * @param readable store
        */
        void RecipientsRestoreL(CMsvStore& aStore);

    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * By default Symbian OS constructor is private.
        */
        CMmsScheduledEntry( const TMsvEntry& aEntry );
        void ConstructL();

        /**
        * Check if the message should be resent or what
        * @param aRecipient scheduling info
        * @param aAction
        * @return ETrue, if message can still be sent
        */
        TBool CheckRecipient( CMsvRecipient& aRecipient, const TMsvSendErrorAction& aAction );

        // By default, prohibit copy constructor
        CMmsScheduledEntry( const CMmsScheduledEntry& );
        // Prohibit assignment operator
        CMmsScheduledEntry& operator= ( const CMmsScheduledEntry& );

    public:     // Data
    
    protected:  // Data

    private:    // Data
        CMmsRecipient* iRecipient; // scheduling information only

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };

inline CMsvRecipient& CMmsScheduledEntry::MmsRecipient() const
    {
    return *iRecipient;
    }

#endif      // MMSSCHEDULEDENTRY_H   
            
// End of File
