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
* Description:  Observer interface to notify of preset changes
*
*/


#ifndef M_MPSPRESETOBSERVER_H
#define M_MPSPRESETOBSERVER_H

#include <e32base.h>

/**
 * Observer interface to notify of preset changes.
 */
class MPSPresetObserver
    {

public:

    /**
     * Possible reasons indicating why the preset was changed.
     */
    enum TPSReason
        {

        /**
         * The preset was created.
         */
        EPSCreated,

        /**
         * The preset was deleted.
         */
        EPSDeleted,

        /**
         * The preset was modified.
         */
        EPSModified

        };

public:

    /**
     * Invoked when a preset has been changed.
     *
     * @param   aId             Id of the preset that was changed.
     * @param   aDataHandler    Data handler of the preset that was changed.
     * @param   aType           Reason for the change.
     */
    virtual void HandlePresetChangedL( TInt aId, TUid aDataHandler, MPSPresetObserver::TPSReason aType ) = 0;

    };

#endif // M_MPSPRESETOBSERVER_H
			   