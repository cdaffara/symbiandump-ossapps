/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A helper object to open messagin entries (e.g. emails).
*
*/

// INCLUDE FILES

#include "MsvUI.h"

#include <mtclbase.h>
#include <mtclreg.h>
#include <mtmuibas.h>
#include <mtuireg.h>


// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CMsvUI::CMsvUI
// C++ constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CMsvUI::CMsvUI()
    : CActive( EPriorityStandard )
    {
    }

// Destructor.
CMsvUI::~CMsvUI()
    {
    Cancel();
    if ( iOperation )
        {
        iOperation->Cancel();
        }
    delete iOperation;
    iOperation = NULL;
    delete iUI;
    iUI = NULL;
    delete iMtm;
    iMtm = NULL;
    delete iUIReg;
    iUIReg = NULL;
    delete iMtmReg;
    iMtmReg = NULL;
    delete iSession;
    iSession = NULL;
    }

// ---------------------------------------------------------------------------
// CMsvUI::OpenEntryL
// From MMsvUI. Opens the messaging entry.
// ---------------------------------------------------------------------------
//
void CMsvUI::OpenEntryL( const TUid& aMsvId )
    {
    if ( iOperation )
        {
        User::Leave( KErrInUse );
        }
    
    if ( ! iSession )
        {
        iSession = CMsvSession::OpenSyncL( *this );
        }
    
    if ( ! iMtmReg )
        {
        iMtmReg = CClientMtmRegistry::NewL( *iSession );
        }
    
    if ( ! iUIReg )
        {
        iUIReg = CMtmUiRegistry::NewL( *iSession );
        }
    
    TMsvId entryId( aMsvId.iUid ) ;
    TMsvEntry entry;
    User::LeaveIfError( iSession->GetEntry( entryId, entryId, entry ) );
    // entryId now contains the service ID of the entry (not used currently)
    
    if ( ! iMtm || iMtm->Type() != entry.iMtm )
        {
        // MTM changed. UI will most probably change, too.
        delete iUI;
        iUI = NULL;
        delete iMtm;
        iMtm = NULL;
        iMtm = iMtmReg->NewMtmL( entry.iMtm );
        }
    
    if ( ! iUI )
        {
        iUI = iUIReg->NewMtmUiL( *iMtm );
        }
    
    iMtm->SwitchCurrentEntryL( entry.Id() );
    iOperation = iUI->OpenL( iStatus );
    
    if ( iOperation && iOperation->IsActive() )
        {
        if ( ! IsAdded() )
            {
            CActiveScheduler::Add(this);
            }
        SetActive();
        }
    else
        {
        Deque();
        delete iOperation;
        iOperation = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CMsvUI::RunL
// From CActive. Handles an active object's request completion event.
// ---------------------------------------------------------------------------
//
void CMsvUI::RunL()
    {
    Deque();
    delete iOperation;
    iOperation = NULL;
    }

// ---------------------------------------------------------------------------
// CMsvUI::DoCancel
// From CActive. Implements cancellation of an outstanding request.
// ---------------------------------------------------------------------------
//
void CMsvUI::DoCancel()
    {
    if ( iOperation )
        {
        iOperation->Cancel();
        }
    delete iOperation;
    iOperation = NULL;
    }

// ---------------------------------------------------------------------------
// CIDSGenericResultItem::HandleSessionEventL
// From MMsvSessionObserver. Indicates an event has occurred.
// ---------------------------------------------------------------------------
//
void CMsvUI::HandleSessionEventL( TMsvSessionEvent aEvent,
                                  TAny* /* aArg1 */,
                                  TAny* /* aArg2 */,
                                  TAny* /* aArg3 */ )
    {
    switch ( aEvent )
        {
        case EMsvCloseSession:
        case EMsvServerTerminated:
            {
            Deque();
            delete iOperation;
            iOperation = NULL;
            delete iUI;
            iUI = NULL;
            delete iMtm;
            iMtm = NULL;
            delete iUIReg;
            iUIReg = NULL;
            delete iMtmReg;
            iMtmReg = NULL;
            delete iSession;
            iSession = NULL;
            break;
            }
        default:
            break;
        }
    }

// End of File
