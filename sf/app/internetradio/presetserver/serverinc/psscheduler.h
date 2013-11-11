/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Custom active scheduler for the preset server
*
*/


#ifndef C_CPSSCHEDULER_H
#define C_CPSSCHEDULER_H

#include <e32base.h>

class CPSServer;

/**
 * Custom active scheduler for the preset server.
 */
NONSHARABLE_CLASS( CPSScheduler ) : public CActiveScheduler
    {

public:

    /**
     * Constructor.
     */
    CPSScheduler();

    /**
     * Destructor.
     */
    ~CPSScheduler();    

    /**
     * Sets the server to use.
     *
     * @param   aServer     The server to use.
     */
    void SetServer( CPSServer* aServer );

// from CActiveScheduler

    /**
     * @see CActiveScheduler::Error( TInt aError ) const
     */
    void Error( TInt aError ) const;
    
private:

    /** The preset server. */
    CPSServer* iServer;
    
    };

#endif // C_CPSSCHEDULER_H
