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
* Description: Light monitor for Playback - Private.
*
*/

#include "mplightmonitor_p.h"
#include "mplightmonitor.h"
#include "mptrace.h"


/*!
    \class MpLightMonitorPrivate
    \brief Wrapper for light monitor - private implementation.

    This is a private implementation of the light monitor interface.
*/

/*!
 \internal
 */
MpLightMonitorPrivate::MpLightMonitorPrivate( MpLightMonitor *qq )
    :q_ptr( qq ),
    iLightStatus( CHWRMLight::ELightStatusUnknown ),
    iLight( 0 )
{
    TX_LOG
}

/*!
 \internal
 */
void MpLightMonitorPrivate::init()
{
    TX_ENTRY
    TRAPD(err, DoInitL());
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpLightMonitorPrivate::DoInitL()
{
    TX_ENTRY
    iLight = CHWRMLight::NewL( this );
    TX_EXIT
}

/*!
 \internal
 */
MpLightMonitorPrivate::~MpLightMonitorPrivate()
{
    TX_ENTRY
    delete iLight; 
    TX_EXIT
}

/*!
 \internal
 */
void MpLightMonitorPrivate::LightStatusChanged( TInt aTarget, CHWRMLight::TLightStatus aStatus )
{
    TX_ENTRY
    if ( aTarget == CHWRMLight::EPrimaryDisplay && aStatus != iLightStatus ) {        
        if ( aStatus == CHWRMLight::ELightOn ) {
            q_ptr->emit lcdLightStatus ( true ); 
        }
        else if( aStatus == CHWRMLight::ELightOff ) {
            q_ptr->emit lcdLightStatus ( false );
        }             
        iLightStatus = aStatus;        
    }
    TX_EXIT
}

