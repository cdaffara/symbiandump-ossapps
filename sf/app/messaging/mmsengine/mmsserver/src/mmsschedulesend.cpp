/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     Scheduled sending of MMS messages
*
*/



// INCLUDE FILES
#include    <msvstd.h>
#include    <msventry.h>
#include    <msvschedulesend.h>
#include    <msvschedulesettings.h>

#include    "mmsschedulesend.h"
#include    "mmsscheduledentry.h"
#include    "mmsaccount.h"
#include    "mmscmds.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CMmsScheduleSend::CMmsScheduleSend( CMsvServerEntry& aServerEntry )
    : CMsvScheduleSend( aServerEntry )
    {
    }

// Two-phased constructor.
CMmsScheduleSend* CMmsScheduleSend::NewL( CMsvServerEntry& aServerEntry )
    {
    CMmsScheduleSend* self = new (ELeave) CMmsScheduleSend( aServerEntry );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CMmsScheduleSend::~CMmsScheduleSend()
    {
    
    }

// ---------------------------------------------------------
// CMmsScheduleSend::GetMessageL
// ---------------------------------------------------------
//
CMsvScheduledEntry* CMmsScheduleSend::GetMessageL(
    const TMsvId aId ) const
    {
    //  Returns a pointer to a newly created CMmsScheduledEntry,
    //  which encapsulates the scheduling and recipient information relating
    //  to the message identified by aId.

    //Set the iServerEntry to the message identified by aId
    User::LeaveIfError(iServerEntry.SetEntry(aId));

    //Create a new CFaxScheduledEntry
    CMmsScheduledEntry* entry = CMmsScheduledEntry::NewL(iServerEntry.Entry());
    CleanupStack::PushL(entry);

    CMsvStore* store = iServerEntry.ReadStoreL();
    CleanupStack::PushL(store);

    //Restore the entry from the message's store.
    entry->RestoreL(*store);

    CleanupStack::PopAndDestroy( store );
    CleanupStack::Pop( entry );

    return entry;
    }

// ---------------------------------------------------------
// CMmsScheduleSend::LoadSettings
// ---------------------------------------------------------
//
void CMmsScheduleSend::LoadSettingsL( TInt aCommand )
    {
    CMmsAccount* account = CMmsAccount::NewL();
    CleanupStack::PushL( account ); // ***
    
    switch( aCommand )
        {
        case EMmsReceive:
        case EMmsScheduledReceive:
        case EMmsReceiveForced:
        case EMmsScheduledReceiveForced:
            account->LoadScheduleReceiveSettingsL(
                ScheduleSettings(), 
                OffPeakTimes(),
                SendErrorActions(), 
                AgentActions() );        
        break;
        default: // loading send settings as default
            account->LoadScheduleSendSettingsL(
                ScheduleSettings(), 
                OffPeakTimes(), 
                SendErrorActions(), 
                AgentActions() );        
        }
    
    CleanupStack::PopAndDestroy( account );
    }
    
// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  

