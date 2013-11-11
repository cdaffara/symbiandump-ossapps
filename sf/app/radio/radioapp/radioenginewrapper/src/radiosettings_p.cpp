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

#include "radiosettings.h"
#include "radiosettings_p.h"
#include "mradioapplicationsettings.h"


/*!
 *
 */
RadioSettingsPrivate::RadioSettingsPrivate( RadioSettings* settingsPublic ) :
    q_ptr( settingsPublic ),
    mFlags( 0 )
{
}

/*!
 *
 */
RadioSettingsPrivate::~RadioSettingsPrivate()
{
}

/*!
 *
 */
void RadioSettingsPrivate::init( MRadioApplicationSettings* storage )
{
    mSettingsStorage = storage;
    if ( mSettingsStorage ) {
        mFlags = Toggles( mSettingsStorage->UiFlags() );
    }
}

/*!
 *
 */
bool RadioSettingsPrivate::isFirstTimeStart()
{
    if ( mSettingsStorage ) {
        return mSettingsStorage->IsFirstTimeStart();
    }
    return false;
}

/*!
 *
 */
void RadioSettingsPrivate::setFirstTimeStartPerformed( bool firstTimeStartPerformed )
{
    if ( mSettingsStorage ) {
        mSettingsStorage->SetFirstTimeStartPerformed( firstTimeStartPerformed );
    }
}

/*!
 *
 */
bool RadioSettingsPrivate::isFlagSet( ToggleFlag flag ) const
{
    return mFlags.testFlag( flag );
}

/*!
 *
 */
void RadioSettingsPrivate::toggleFlag( ToggleFlag flag )
{
    mFlags ^= flag;
}

/*!
 *
 */
void RadioSettingsPrivate::saveFlags()
{
    if ( mSettingsStorage ) {
        mSettingsStorage->SetUiFlags( mFlags );
    }
}
