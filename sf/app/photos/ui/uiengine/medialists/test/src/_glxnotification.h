/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Unit tests
*
*/



#ifndef ___T_GLXNOTIFICATION_H__
#define ___T_GLXNOTIFICATION_H__

//  EXTERNAL INCLUDES
#include <e32std.h>

//  INTERNAL INCLUDES

//  FORWARD DECLARATIONS

/**
 * Notification of change
 */
class TNotification
    {
    public:
        enum TNotificationType
            {
            ERemove,
            EAdd,
            EFocus,
            ESelection,
            EEndMarker // Last notification
            };
        
        // Data needs to be a simple struct with no member functions, to allow 
        // arrays to be easily declared 
        struct TData
            {
            TNotificationType iType;
            TInt iAtIndex; // Old focus index for EFocus
            TInt iData; // count for ERemove and EAdd, NGlxListDefs::TFocusChangeType for EFocus, TBool for ESelection
            };
    public:
        TNotification( TData aData );
        TBool operator==( const TNotification::TData& aNotification ) const;
        TBool operator!=( const TNotification::TData& aNotification ) const;
        
    private:
        TData iData;
    };

// Marker to signal the end of notifications list
#define END_MARKER TNotification::EEndMarker, 0, 0 // { EEndMarker, 0, 0 };
const TNotification::TData KEndMarkerData = { END_MARKER };
const TNotification KEndMarker = KEndMarkerData;

#endif      //  ___T_GLXNOTIFICATION_H__
