/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Mediator interface
*
*/

#ifndef MPEMEDIATOR_H
#define MPEMEDIATOR_H

#include <e32std.h>

/**
 *  Mediator interface
 *
 *  @lib PhoneModel.lib
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( MPEMediator )
    {

public:
    
    /**
     * Sends response to mediator command.
     * 
     * @param aCategory Command category
     * @param aCommandId Id of the command
     * @param aData Data to be sent.
     */
    virtual void SendResponse( 
        TUid aCategory, 
        TInt aCommandId, 
        const TDesC8& aData ) = 0;
    
    /**
     * Raises mediator event.
     * 
     * @param aCategory Event category
     * @param aEventId Id of the event
     * @param aData Data to be sent.
     */
    virtual void RaiseEvent( 
        TUid aCategory, 
        TInt aEventId,
        const TDesC8& aData ) = 0;

    };


#endif // MPEMEDIATOR_H
