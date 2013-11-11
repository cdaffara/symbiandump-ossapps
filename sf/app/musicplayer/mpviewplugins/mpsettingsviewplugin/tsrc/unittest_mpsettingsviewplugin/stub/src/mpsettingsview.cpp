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
* Description: MpSettingsView stub for testing MpSettingsViewplugin
*
*/

#include "stub/inc/mpsettingsview.h"


/*!
 Constructs the MpSettingsView stub
 */
MpSettingsView::MpSettingsView()
    :mInitializeViewCount(0),
    mActivateViewCount(0),
    mDeactivateViewCount (0)
{
}

/*!
 Destructs the MpSettingsView stub
 */
MpSettingsView::~MpSettingsView()
{
}
    
/*!
 Initializes the MpSettingsView stub
 */
void MpSettingsView::initializeView()
{
    mInitializeViewCount++;
}

/*!
 Activates the MpSettingsView stub
 */
void MpSettingsView::activateView()
{
    mActivateViewCount++;
}

/*!
 Deactivates the MpSettingsView stub
 */
void MpSettingsView::deactivateView()
{
    mDeactivateViewCount++;
}

void MpSettingsView::resetCounters()
{
    mInitializeViewCount = 0;
    mActivateViewCount = 0;
    mDeactivateViewCount = 0;
}

/*!
 emits the signal command
 */
void MpSettingsView::emitCommand(int cmd)
{
    emit command(cmd);
}
