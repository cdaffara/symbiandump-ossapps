/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    TV Out observer
*
*/



/**
 * @internal reviewed 24/08/2007 by D Holland
 */

#ifndef M_GLXTVOBSERVER_H
#define M_GLXTVOBSERVER_H

#include <glxtvconstants.h> // for TTvChangeType
/**
 * Class that observes the TV out state change
 */
NONSHARABLE_CLASS( MGlxTvObserver )
    {
public:     
    /*
     * Handle notification of a change in the TV out state
     * @param The change type - Connection/activation
     */    
    virtual void HandleTvStatusChangedL ( TTvChangeType aChangeType ) = 0;
    };



#endif // M_GLXTVOBSERVER_H