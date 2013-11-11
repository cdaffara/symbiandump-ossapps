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
*     Entry for scheduled sending of MMS messages
*
*/



// INCLUDE FILES
#include    <apparc.h>
#include    <msvrcpt.h>
#include    "mmsscheduledentry.h"
#include    "mmsconst.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

CMmsRecipient* CMmsRecipient::NewL()
    {
    CMmsRecipient* self = new (ELeave) CMmsRecipient();
    return self;
    }

CMmsRecipient::~CMmsRecipient()
    {
    }

CMmsRecipient::CMmsRecipient()
    {
    iMaxRetries = -1; // can be overridden later
    }

void CMmsRecipient::InternalizeL(RReadStream& aReadStream)
    {
    CMsvRecipient::InternalizeL( aReadStream );
    aReadStream >> iMaxRetries;
    }

void CMmsRecipient::ExternalizeL(RWriteStream& aWriteStream) const
    {
    CMsvRecipient::ExternalizeL( aWriteStream );
    aWriteStream << iMaxRetries;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CMmsScheduledEntry::CMmsScheduledEntry( const TMsvEntry& aEntry )
    : CMsvScheduledEntry( aEntry )
    {
    }

// Symbian OS default constructor can leave.
void CMmsScheduledEntry::ConstructL()
    {
    iRecipient = CMmsRecipient::NewL();
    }

// Two-phased constructor.
CMmsScheduledEntry* CMmsScheduledEntry::NewL( const TMsvEntry& aEntry )
    {
    CMmsScheduledEntry* self = new (ELeave) CMmsScheduledEntry( aEntry );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); // self

    return self;
    }

    
// Destructor
CMmsScheduledEntry::~CMmsScheduledEntry()
    {
    delete iRecipient;
    }


// ---------------------------------------------------------
// CMmsScheduledEntry::CanSendToAnyRecipients
//
// ---------------------------------------------------------
//
TBool CMmsScheduledEntry::CanSendToAnyRecipients(
    const CMsvSendErrorActions& aErrorActions,
    TMsvSendErrorAction& aAction )
    {

    TBool retVal = EFalse;

    TMsvSendErrorAction action;

    if (aErrorActions.GetSendErrorAction(iRecipient->Error(), action) != KErrNone)
        {
        action = aErrorActions.Default();
        }

    if ( CheckRecipient( *iRecipient, action ) )
        {
        retVal = ETrue;
        aAction = action;
        }

    return retVal;

    }

// ---------------------------------------------------------
// CMmsScheduledEntry::CanSendToAnyRecipients
//
// ---------------------------------------------------------
//
TBool CMmsScheduledEntry::CanSendToAnyRecipients(
    const TMsvSendErrorAction& aAction )
    {

    TBool retVal = EFalse;
    retVal = CheckRecipient( *iRecipient, aAction );
    return retVal;

    }

// ---------------------------------------------------------
// CMmsScheduledEntry::RecipientsResetRetries
//
// ---------------------------------------------------------
//
void CMmsScheduledEntry::RecipientsResetRetries()
    {

    if ( iRecipient->Status() != CMsvRecipient::ESentSuccessfully )
        {
        iRecipient->ResetRetries();
        }

    }

// ---------------------------------------------------------
// CMmsScheduledEntry::RecipientsIncreaseRetries
//
// ---------------------------------------------------------
//
void CMmsScheduledEntry::RecipientsIncreaseRetries()
    {

    if (iRecipient->Status() != CMsvRecipient::ESentSuccessfully)
        {
        iRecipient->IncreaseRetries();
        }

    }

// ---------------------------------------------------------
// CMmsScheduledEntry::RecipientsSetFailed
//
// ---------------------------------------------------------
//
void CMmsScheduledEntry::RecipientsSetFailed()
    {

    if (iRecipient->Status() != CMsvRecipient::ESentSuccessfully)
        {
        iRecipient->SetStatus(CMsvRecipient::EFailedToSend);
        }

    }

// ---------------------------------------------------------
// CMmsScheduledEntry::RecipientsAllSent
//
// ---------------------------------------------------------
//
TBool CMmsScheduledEntry::RecipientsAllSent() const
    {

    return ( iRecipient->Status() == CMsvRecipient::ESentSuccessfully );

    }

// ---------------------------------------------------------
// CMmsScheduledEntry::RecipientsStoreL
//
// ---------------------------------------------------------
//
void CMmsScheduledEntry::RecipientsStoreL(
    CMsvStore& aStore ) const
    {

    RMsvWriteStream writeStream;
    writeStream.AssignLC(aStore, KUidMmsRecipientStream);
    
    iRecipient->ExternalizeL( writeStream );

    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream ); // close writeStream

    }

// ---------------------------------------------------------
// CMmsScheduledEntry::RecipientsRestoreL
//
// ---------------------------------------------------------
//
void CMmsScheduledEntry::RecipientsRestoreL(
    CMsvStore& aStore )
    {
    delete iRecipient;
    iRecipient = NULL;
    iRecipient = CMmsRecipient::NewL();

    if ( aStore.IsPresentL( KUidMmsRecipientStream ) )
        {
        RMsvReadStream readStream;
        readStream.OpenLC(aStore, KUidMmsRecipientStream);
        iRecipient->InternalizeL( readStream );
        CleanupStack::PopAndDestroy( &readStream ); // close readStream
        }

    }

// ---------------------------------------------------------
// CMmsScheduledEntry::CheckRecipient
//
// ---------------------------------------------------------
//
TBool CMmsScheduledEntry::CheckRecipient(
    CMsvRecipient& aRecipient,
    const TMsvSendErrorAction& aAction )
    {
    TBool retVal = EFalse;

    // If max retry override set, use it
    TInt16 retries = ((CMmsRecipient&)aRecipient).MaxRetries();
    if ( retries < 0 ) 
        {
        retries = aAction.MaxRetries();
        }

    if (aRecipient.Status() != CMsvRecipient::ESentSuccessfully)
        {
        if (aAction.iAction == ESendActionSentAlready)
            {
            aRecipient.SetStatus(CMsvRecipient::ESentSuccessfully);
            }
        else if (aAction.iAction == ESendActionFail)
            {
            aRecipient.SetStatus(CMsvRecipient::EFailedToSend);
            }
        else if (aAction.iRetries == ESendRetriesInfinite || aRecipient.Retries() < retries )
            {
            aRecipient.SetStatus(CMsvRecipient::ENotYetSent);
            retVal = ETrue;
            }
        else
            {
            aRecipient.SetStatus(CMsvRecipient::EFailedToSend);
            }
        }

    return retVal;

    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  

