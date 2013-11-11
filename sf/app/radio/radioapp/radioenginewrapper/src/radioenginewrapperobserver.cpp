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

#include "radioenginewrapperobserver.h"

/*!
 *
 * @param frequency
 * @param commandSender
 */
void RadioEngineWrapperObserver::tunedToFrequency( uint frequency, int reason )
{
    // Default implementation does nothing
    Q_UNUSED( frequency );
    Q_UNUSED( reason );
}

/*!
 *
 * @param radioIsOn
 */
void RadioEngineWrapperObserver::radioStatusChanged( bool radioIsOn )
{
    // Default implementation does nothing
    Q_UNUSED( radioIsOn );
}

/*!
 *
 * @param available
 */
void RadioEngineWrapperObserver::rdsAvailabilityChanged( bool available )
{
    // Default implementation does nothing
    Q_UNUSED( available );
}

/*!
 *
 */
void RadioEngineWrapperObserver::increaseVolume()
{
    // Default implementation does nothing
}

/*!
 *
 */
void RadioEngineWrapperObserver::decreaseVolume()
{
    // Default implementation does nothing
}

/*!
 *
 * @param volume
 */
void RadioEngineWrapperObserver::volumeChanged( int volume )
{
    // Default implementation does nothing
    Q_UNUSED( volume );
}

/*!
 *
 * @param muted
 */
void RadioEngineWrapperObserver::muteChanged( bool muted )
{
    // Default implementation does nothing
    Q_UNUSED( muted );
}

/*!
 *
 * @param loudspeaker
 */
void RadioEngineWrapperObserver::audioRouteChanged( bool loudspeaker )
{
    // Default implementation does nothing
    Q_UNUSED( loudspeaker );
}

/*!
 *
 * @param connected
 */
void RadioEngineWrapperObserver::antennaStatusChanged( bool connected )
{
    // Default implementation does nothing
    Q_UNUSED( connected );
}

/*!
 *
 */
void RadioEngineWrapperObserver::skipPrevious()
{
    // Default implementation does nothing
}

/*!
 *
 */
void RadioEngineWrapperObserver::skipNext()
{
    // Default implementation does nothing
}

