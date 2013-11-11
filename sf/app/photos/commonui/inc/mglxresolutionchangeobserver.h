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
* Description:   Screen resolution changed observer
*
*/


/**
 * @internal reviewed 24/08/2007 by D Holland
 */

#ifndef M_GLXRESOLUTIONCHANGEOBSERVER_H
#define M_GLXRESOLUTIONCHANGEOBSERVER_H


/**
 * Class that observes the screen resolution change
 */
NONSHARABLE_CLASS( MGlxResolutionChangeObserver )
    {
    /*
     * Handle notification of change in the screen resolution
     */
public:     
    virtual void HandleResolutionChanged () = 0;
    };



#endif // M_GLXRESOLUTIONCHANGEOBSERVER_H