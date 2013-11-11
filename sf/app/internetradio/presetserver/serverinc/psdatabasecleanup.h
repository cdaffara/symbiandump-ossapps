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
* Description:  Handles periodical database cleanup routines
*
*/


#ifndef C_CPSDATABASECLEANUP_H
#define C_CPSDATABASECLEANUP_H

#include <e32base.h>

class RPSDatabase;

/**
 * Handles periodical database cleanup routines.
 */
NONSHARABLE_CLASS( CPSDatabaseCleanup ) : public CTimer
    {

public:

    /**
     * Two-phased constructor.
     *
     * @param   aDatabase   The preset database to use.
     * @return  The newly created object.
     */
    static CPSDatabaseCleanup* NewL( RPSDatabase& aDatabase );

    /**
     * Destructor.
     */
    ~CPSDatabaseCleanup();
    
    /**
     * Requests database cleanup asynchronously.
     *
     */
    void RequestCleanupCheck();

private:

    /**
     * Constructor.
     *
     * @param   aDatabase   The preset database to use.
     */
    CPSDatabaseCleanup( RPSDatabase& aDatabase );

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * Deletes orphaned presets from the database.
     */
    void DeleteOrphanedPresetsL();

// from base class CTimer

    /**
     * @see CTimer::RunL().
     */
    void RunL();

private:

    /** The preset database. */
    RPSDatabase& iDatabase;
    
    };

#endif // C_CPSDATABASECLEANUP_H
