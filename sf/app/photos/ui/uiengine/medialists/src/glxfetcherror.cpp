/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Record for an attribute retrieval error
*
*/




#include "glxfetcherror.h"
#include <glxtracer.h>                         // For Logs

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
TGlxFetchError::TGlxFetchError(TMPXAttribute aAttr, TInt aError)
:   iAttr(aAttr),
    iError(aError)
    {
    TRACER("TGlxFetchError::Default Constructor");
    
    // Set the timestamp to now
    TTime now;
    now.UniversalTime();
    iTimestamp = now.DateTime();
    }

