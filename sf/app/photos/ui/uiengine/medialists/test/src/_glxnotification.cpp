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
* Description:   Class that reconstructs a list from notifications
*
*/



//  CLASS HEADER
#include "_glxnotification.h"

//  EXTERNAL INCLUDES
    
//  INTERNAL INCLUDES

TNotification::TNotification( TNotification::TData aData )
    {
    iData = aData;
    }

TBool TNotification::operator==( const TNotification::TData& aNotificationData ) const
    {
    return iData.iType    == aNotificationData.iType 
        && iData.iAtIndex == aNotificationData.iAtIndex
        && iData.iData    == aNotificationData.iData;
    }
    
TBool TNotification::operator!=( const TNotification::TData& aNotificationData ) const
    {
    return !( *this == aNotificationData );
    }
