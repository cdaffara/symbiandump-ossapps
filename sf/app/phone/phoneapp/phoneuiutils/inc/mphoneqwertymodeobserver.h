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
* Description:  Observer for qwerty mode.
*
*/


#ifndef MPHONEQWERTYMODEOBSERVER_H 
#define MPHONEQWERTYMODEOBSERVER_H

// CLASS DECLARATION

/**
*  Observer for qwerty mode changes.
*/
class MPhoneQwertyModeObserver
    {

    public:
        /**
        * Handle Qwerty mode change.
        * @param aMode 0 = off, 1 = on
        */
        virtual void HandleQwertyModeChange( TInt aMode ) = 0;
        
        /**
        * Handle qwerty keyboard layout change.
        */
        virtual void HandleKeyboardLayoutChange() = 0;

    };

#endif // MPHONEQWERTYMODEOBSERVER_H

// End of File
