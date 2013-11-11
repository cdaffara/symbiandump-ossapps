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
* Description: 
*
*/


// INCLUDE FILES
#include "loadgen_loadbase.h"
#include "loadgen.hrh"

// ===================================== MEMBER FUNCTIONS =====================================

CLoadBase::CLoadBase()
    {
    }

// --------------------------------------------------------------------------------------------

void CLoadBase::ConstructL()
    {
    iState = ELoadStateInvalid;
    }
    
// --------------------------------------------------------------------------------------------

void CLoadBase::Resume()
    {
    iState = ELoadStateRunning;
    }

// --------------------------------------------------------------------------------------------

void CLoadBase::Suspend()
    {
    iState = ELoadStateSuspended;
    }

// --------------------------------------------------------------------------------------------

void CLoadBase::SetPriority()
    {
    }
    
// --------------------------------------------------------------------------------------------

void CLoadBase::Close()
    {
    iState = ELoadStateClosed;
    }
    
// --------------------------------------------------------------------------------------------

TPtrC CLoadBase::Description()
    {
    return TPtrC(KNullDesC);
    }               
               
// --------------------------------------------------------------------------------------------

// End of File
