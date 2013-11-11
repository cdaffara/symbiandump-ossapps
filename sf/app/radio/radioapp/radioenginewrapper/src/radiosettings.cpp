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

// System includes
#include <QVariant>

// User includes
#include "radiosettings.h"
#include "radiosettings_p.h"
#include "radio_global.h"

namespace RadioSetting
{
    enum TMtvSettingType
    {
        EFlags = 0,
        ESomething
    };
}

/*!
 *
 */
RadioSettings::RadioSettings() :
    d_ptr( new RadioSettingsPrivate( this ) )
{
}

/*!
 *
 */
RadioSettings::~RadioSettings()
{
}

/*!
 * \reimp
 */
bool RadioSettings::isFirstTimeStart()
{
    Q_D( RadioSettings );
    return d->isFirstTimeStart();
}

/*!
 * \reimp
 */
void RadioSettings::setFirstTimeStartPerformed( bool firstTimeStartPerformed )
{
    Q_D( RadioSettings );
    d->setFirstTimeStartPerformed( firstTimeStartPerformed );
}

/*!
 * \reimp
 */
bool RadioSettings::showFavorites() const
{
    Q_D( const RadioSettings );
    return d->isFlagSet( RadioSettingsPrivate::EShowFavorites );
}

/*!
 * \reimp
 */
void RadioSettings::setShowFavorites( bool showFavorites )
{
    Q_D( RadioSettings );
    if ( d->isFlagSet( RadioSettingsPrivate::EShowFavorites ) != showFavorites ) {
        d->toggleFlag( RadioSettingsPrivate::EShowFavorites );
    }
}

/*!
 * \reimp
 */
bool RadioSettings::toggleShowFavorites()
{
    Q_D( RadioSettings );
    d->toggleFlag( RadioSettingsPrivate::EShowFavorites );
    return d->isFlagSet( RadioSettingsPrivate::EShowFavorites );
}
