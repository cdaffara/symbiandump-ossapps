/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer interface for indicating changes in accessories.
*
*/

#ifndef MVTENGACCESSORYOBSERVER_H
#define MVTENGACCESSORYOBSERVER_H

//  INCLUDES
#include    <e32std.h>
#include    <AccPolAccessoryMode.h>

// CLASS DECLARATION

/**
*  Interface to indicate changes in accessories.
*
*  @lib videoteleng
*  @since 3.0
*/
class MVtEngAccessoryObserver
    {
    public:  // New functions

        /**
        * Callback to notify mode change in accessories.
        * @param aMode new mode 
        * @param aActivated audio is on 
        */
        virtual void AccessoryModeChanged( const TAccMode& aMode, 
            const TBool aActivated ) = 0;
    };

#endif // MVTENGACCESSORYOBSERVER_H
// End of File
