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
* Description:
*
*/


#ifndef M_IRACTIVENETWORKOBSERVER_H
#define M_IRACTIVENETWORKOBSERVER_H

#include "irnetworkcontrollerobserver.h"


class MIRActiveNetworkObserver
    {
public:
    /**
     * Notifies all observers whose network request is active
     * to reissue the request  
     * NotifyActiveNetworkObserversL()
     */    
    virtual void NotifyActiveNetworkObserversL( TIRNetworkEvent aEvent ) = 0;
     
     
    /**
     * Notifies all observers whose network request is active
     * to reset the pending request status  
     * ResetPendingRequests()
     */    
    virtual void ResetPendingRequests( TBool aValue ) = 0;
    };

#endif // M_IRACTIVENETWORKOBSERVER_H
