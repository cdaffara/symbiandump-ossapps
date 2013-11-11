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
* Description:  Client-server preset notifier
*
*/


#ifndef R_RPSPRESETNOTIFIER_H
#define R_RPSPRESETNOTIFIER_H

#include "pscommon.h"
#include "pspresetobserver.h"

class RPSServ;

/**
 * Client-server preset notifier.
 */
class RPSServNotifier
    {

public:

    /**
     * Constructor.
     *
     * @param   aServer     Session with the preset server.
     * @param   aId         The ID of the preset to observe, or <code>KErrNotFound</code>
     *                      if all changes should result in a notification.
     */    
    RPSServNotifier( RPSServ& aServer, TInt aId );

    /**
     * Closes all resources allocated.
     */    
    void Close();

    /**
     * Request a notify when the preset with the supplied identifier is changed.
     *
     * @param   aStatus         The request status to signal when the preset is changed.
     */    
    void Notify( TRequestStatus& aStatus );

    /**
     * Cancels the pending notify request.
     */    
    void CancelNotify();

    /**
     * Returns the id of the preset.
     *
     * @return  Id of the preset.
     */
    TInt Id() const;

    /**
     * Returns the data handler of the preset.
     *
     * @return  Data handler of the preset.
     */
    TUid DataHandler() const;

    /**
     * Returns the reason of the change.
     *
     * @return  Reason of the change.
     */
    MPSPresetObserver::TPSReason Reason() const;    

private:

    /** The preset server. */
    RPSServ& iServer;
    /** Packaged data of the latest notify. */
    TPckgBuf<TPSNotifyDataPackage> iData;
    /** The ID of the preset to observe, or <code>KErrNotFound</code> if all changes should result in a notification. */
    TInt iId;

    };

#endif // R_RPSPRESETNOTIFIER_H
