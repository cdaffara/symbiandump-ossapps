/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Transaction implementation
*
*/


#include "pstransaction.h"

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CPSTransaction* CPSTransaction::NewL( MPSTransactionEntity& aEntity )
    {
    CPSTransaction* self = new ( ELeave ) CPSTransaction( aEntity );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CPSTransaction::CPSTransaction( MPSTransactionEntity& aEntity )
    : iEntity( aEntity )
    {
    }
    
// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CPSTransaction::ConstructL()
    {
    iEntity.BeginTransactionL();
    }
    
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CPSTransaction::~CPSTransaction()
    {
    iEntity.Rollback();
    }
    
// ---------------------------------------------------------------------------
// Commits all changes done during the transaction.
// ---------------------------------------------------------------------------
//
EXPORT_C void CPSTransaction::CommitL()
    {
    iEntity.CommitL();
    }
    
// ---------------------------------------------------------------------------
// Reverts all changes done during the transaction.
// ---------------------------------------------------------------------------
//
EXPORT_C void CPSTransaction::Rollback()
    {
    iEntity.Rollback();
    }
