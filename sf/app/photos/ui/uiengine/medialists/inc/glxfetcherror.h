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




#ifndef T_GLXFETCHERROR_H
#define T_GLXFETCHERROR_H

#include <e32base.h>
#include <mpxattribute.h>

/**
 * TGlxFetchError
 *
 * Contains information about an attribute retrieval error
 *
 * @lib glxmedialists.lib
 */
class TGlxFetchError
    {
public:
    /**
     * Constructor. Timestamp is set to current universal time.
     * @param aAttr The attribute with the error
     * @param aError The error code to record
     */
    TGlxFetchError(TMPXAttribute aAttr, TInt aError);

public:
    /// The errored attribute     
    TMPXAttribute iAttr;
    /// The error code
    TInt iError;
    /// Timestamp of when the error was recorded
    TDateTime iTimestamp;
    };
    
#endif // T_GLXFETCHERROR_H
