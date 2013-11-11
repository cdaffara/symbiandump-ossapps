/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "ccustomnotedialog.h"

// CONSTRUCTION
CCustomNoteDialog* CCustomNoteDialog::NewL( CEikDialog** aSelfPtr )
    {
    CCustomNoteDialog* self = CCustomNoteDialog::NewLC( aSelfPtr );
    CleanupStack::Pop( self );
    return self;
    }
    
// CONSTRUCTION
CCustomNoteDialog* CCustomNoteDialog::NewLC( CEikDialog** aSelfPtr )
    {
    CCustomNoteDialog* self = new (ELeave) CCustomNoteDialog( aSelfPtr );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// Destructor
CCustomNoteDialog::~CCustomNoteDialog()
    {
    if ( iCallback )
        {
        // no need to handle error here
        TRAP_IGNORE( iCallback->DialogDismissedL( EAknSoftkeyCancel ) );
        }
    }
    
// Constructor
CCustomNoteDialog::CCustomNoteDialog( CEikDialog** aSelfPtr )
: CAknNoteDialog( aSelfPtr )
    {
    // no implementation
    }

// 2nd phase constructor
void CCustomNoteDialog::ConstructL()
    {
    // no implementation
    }
     
void CCustomNoteDialog::SetCallback( MProgressDialogCallback* aCallback )
    {
    iCallback = aCallback;
    }

//  End of File
