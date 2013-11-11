/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Defines interface for manipulating telephony touch pane 
*                buttons using ECOM plugin.
*
*/


#ifndef TELTOUCHPANEEXTENSION_H
#define TELTOUCHPANEEXTENSION_H

// INCLUDES
#include <e32base.h>


/**
 * Plugin interface class for manipulating telephony
 * touch pane items.
 *
 * @since S60 5.1
 */
class MTelTouchPaneExtension
    {

public:

    /**
     * Returns number of buttons in touch pane.
     *
     * @since S60 v5.1
     * @return Button count.
     */
    virtual TInt NumberOfButtonsInPane()  const = 0;

    /**
     * Returns command id assinged to button in given position.
     * If button is toggling, currently active command is 
     * returned.
     *
     * @since S60 v5.1
     * @param aButtonIndex Button index starting 0.
     * @return Command id assigned to given position.
     */
    virtual TInt ButtonCommandId( TInt aButtonIndex )  const = 0;

    /**
     * Sets button dimmed.
     *
     * @since S60 v5.1
     * @param aCommand Command id.
     * @param aDimmed  ETrue to set dimmed, EFalse to undim.
     */
    virtual void SetButtonDimmed( TInt aCommand, TBool aDimmed ) = 0;

    };


#endif // TELTOUCHPANEEXTENSION_H

//End of file
