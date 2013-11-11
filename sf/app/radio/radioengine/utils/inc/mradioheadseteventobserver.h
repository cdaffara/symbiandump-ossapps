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
* Description:
*
*/

#ifndef MRADIOHEADSETEVENTOBSERVER_H
#define MRADIOHEADSETEVENTOBSERVER_H


// Class declaration
/**
*
*  Observer for the Headset button state. Implementer will be notified each time the headset button
*  state has changed ( button pressed ).
*/
NONSHARABLE_CLASS( MRadioHeadsetEventObserver )
    {
public:

    /**
    * This callback will notify of the Headset to be connected
    */
    virtual void HeadsetConnectedCallbackL() = 0;

    /**
    * This callback will notify of the Headset to be disconnected
    */
    virtual void HeadsetDisconnectedCallbackL() = 0;

    };

#endif // MRADIOHEADSETEVENTOBSERVER_H


