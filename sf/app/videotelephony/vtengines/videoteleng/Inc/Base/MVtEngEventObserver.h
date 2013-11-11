/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer interface for video telephony events.
*
*/



#ifndef MVTENGEVENTOBSERVER_H
#define MVTENGEVENTOBSERVER_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Video telephony event observer interface.
*
*  @lib videlteleng
*  @since 2.6
*/
class MVtEngEventObserver
    {
    public: // New functions        
        
        /**
        * Handler method for events
        * @param aEvent events defined in VtEngEvents.h
        */
        virtual void HandleVtEventL( 
            TInt aEvent ) = 0;

    };

#endif      // MVTENGEVENTOBSERVER_H
            
// End of File
