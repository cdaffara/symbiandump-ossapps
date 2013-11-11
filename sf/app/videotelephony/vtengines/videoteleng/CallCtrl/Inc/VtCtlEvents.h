/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Call control events
*
*/



#ifndef VTCLTDEFS_H
#define VTCLTDEFS_H

//  INCLUDES
#include <e32base.h>

// DATA TYPES
typedef TInt TVtCtlEvent;

// CONSTANTS

/**
* Session status is changed. aParams in 
* MVtCtlEventObserver::HandleVtSessionEventL is of 
* type MVtCtlCallControl::TVtCtlState
*/
const TVtCtlEvent KVtCtlEventSessionStatusChanged = 1;

/**
* Session duration is changed. aParams 
* MVtCtlEventObserver::HandleVtSessionEventL is of 
* type TVtCtlDuration
*/
const TVtCtlEvent KVtCtlEventDurationChanged = 2;

#endif      // VTCLTDEFS_H

// End of File
