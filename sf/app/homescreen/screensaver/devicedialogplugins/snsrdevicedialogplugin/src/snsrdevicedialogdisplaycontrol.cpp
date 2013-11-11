/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Platform agnostic Qt wrapper class for controlling display
*              power save mode. The power save mode gets actually activated
*              only on Symbian environment; in other environments the provided
*              functions do nothing.
*              The use of Symbian display power save API has been wrapped with
*              client-server pair because HbDeviceDialogs don't have enough
*              capabilities to call that API directly.
*/

#include <qglobal.h>
#include <qdebug.h>
#include "snsrdevicedialogdisplaycontrol.h"

/*!
    Constructor
 */
SnsrDeviceDialogDisplayControl::SnsrDeviceDialogDisplayControl()
{
#ifdef Q_OS_SYMBIAN
    int error = mDisplayClient.Open();
    if ( error ) {
        qWarning() << QString("Opening Display Control Client failed, error code %1").arg(error);
    }
#endif
}

/*!
    Destructor
 */
SnsrDeviceDialogDisplayControl::~SnsrDeviceDialogDisplayControl()
{
#ifdef Q_OS_SYMBIAN
    mDisplayClient.Close();
#endif
}

/*!
    Set display to full power mode (the normal mode).
*/
void SnsrDeviceDialogDisplayControl::setDisplayFullPower()
{
#ifdef Q_OS_SYMBIAN
    mDisplayClient.SetDisplayFullPower();
#endif
}

/*!
    Set display to low-power/partial mode. The area outside of set active area
    is left completely black.
    \param  startRow    The first active screen row in the native orientation of the display device.
    \param  endRow      The last active screen row in the native orientation of the display device. 
 */
void SnsrDeviceDialogDisplayControl::setDisplayLowPower( int startRow, int endRow )
{
#ifdef Q_OS_SYMBIAN
    mDisplayClient.SetDisplayLowPower( startRow, endRow );
#else
    Q_UNUSED( startRow );
    Q_UNUSED( endRow );
#endif
}

/*!
    Set display device off.
*/
void SnsrDeviceDialogDisplayControl::setDisplayOff()
{
#ifdef Q_OS_SYMBIAN
    mDisplayClient.SetDisplayOff();
#endif
}

