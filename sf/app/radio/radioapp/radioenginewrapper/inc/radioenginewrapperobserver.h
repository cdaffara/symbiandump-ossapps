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

#ifndef RADIOENGINEWRAPPEROBSERVER_H_
#define RADIOENGINEWRAPPEROBSERVER_H_

#include "radio_global.h"
#include "radiowrapperexport.h"

class WRAPPER_DLL_EXPORT RadioEngineWrapperObserver
{
public:

    virtual void tunedToFrequency( uint frequency, int reason );

    virtual void radioStatusChanged( bool radioIsOn );

    virtual void rdsAvailabilityChanged( bool available );

    virtual void increaseVolume();
    virtual void decreaseVolume();
    virtual void volumeChanged( int volume );
    virtual void muteChanged( bool muted );

    virtual void audioRouteChanged( bool loudspeaker );
    virtual void antennaStatusChanged( bool connected );

    virtual void skipPrevious();
    virtual void skipNext();

};

#endif // RADIOENGINEWRAPPEROBSERVER_H_
