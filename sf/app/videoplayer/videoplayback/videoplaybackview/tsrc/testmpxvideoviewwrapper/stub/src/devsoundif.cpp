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
* Description:  Implementation of CDevSoundIf
*
*/

// Version : %version: 1 %



// INCLUDE FILES
#include <devsoundif.h>



// CONSTANTS


// ============================ MEMBER FUNCTIONS ===================================================

// -------------------------------------------------------------------------------------------------
// CDevSoundIf::CDevSoundIf
// C++ default constructor can NOT contain any code, that might leave.
// -------------------------------------------------------------------------------------------------
//
CDevSoundIf::CDevSoundIf()
{
    iVolumeSteps = 10;
}

// -------------------------------------------------------------------------------------------------
// CDevSoundIf::ConstructL
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CDevSoundIf::ConstructL()
{
}

// -------------------------------------------------------------------------------------------------
// CDevSoundIf::NewL
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
CDevSoundIf* CDevSoundIf::NewL()
{
    CDevSoundIf* self = new( ELeave ) CDevSoundIf;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
// CDevSoundIf::~CDevSoundIf
// Destructor
// -------------------------------------------------------------------------------------------------
//
CDevSoundIf::~CDevSoundIf()
{
}

// -------------------------------------------------------------------------------------------------
// CDevSoundIf::GetNumberOfVolumeSteps
// -------------------------------------------------------------------------------------------------
//
TInt CDevSoundIf::GetNumberOfVolumeSteps()
{
    return iVolumeSteps;
}

//  End of File
