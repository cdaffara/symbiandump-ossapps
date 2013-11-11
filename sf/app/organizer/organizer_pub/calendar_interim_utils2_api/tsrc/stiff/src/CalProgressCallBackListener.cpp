/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The Source file for the CCalProgressCallBackListener class.
*
*/


// INCLUDES
#include "CalProgressCallBackListener.h"

// -----------------------------------------------------------------------------
// CCalProgressCallBackListener::NewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalProgressCallBackListener* CCalProgressCallBackListener::NewL()
    {
    CCalProgressCallBackListener* self = new (ELeave) CCalProgressCallBackListener;
    CleanupStack::PushL( self );
    
    self->ConstructL();
    
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCalProgressCallBackListener::~CCalProgressCallBackListener
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalProgressCallBackListener::~CCalProgressCallBackListener()
    {
    }

// -----------------------------------------------------------------------------
// CCalProgressCallBackListener::Progress
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalProgressCallBackListener::Progress( TInt /*aPercentageCompleted*/ )
    {
    // Nothing.
    }

// -----------------------------------------------------------------------------
// CCalProgressCallBackListener::Completed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalProgressCallBackListener::Completed( TInt aError )
    {
    }

// -----------------------------------------------------------------------------
// CCalProgressCallBackListener::NotifyProgress
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalProgressCallBackListener::NotifyProgress()
    {
    return ETrue;
    }


// -----------------------------------------------------------------------------
// CCalProgressCallBackListener::CCalProgressCallBackListener
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalProgressCallBackListener::CCalProgressCallBackListener()
    {
    // Nothing.
    }

// -----------------------------------------------------------------------------
// CCalProgressCallBackListener::ConstructL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalProgressCallBackListener::ConstructL()
    {
    }

// End of file
