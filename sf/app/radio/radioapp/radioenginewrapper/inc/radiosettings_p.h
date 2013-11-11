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

#ifndef RADIOSETTINGS_P_H_
#define RADIOSETTINGS_P_H_

class MRadioApplicationSettings;

class RadioSettingsPrivate
{
    Q_DECLARE_PUBLIC( RadioSettings )
    Q_DISABLE_COPY( RadioSettingsPrivate )

    /**
     * On/Off type flags.
     *
     * Bit shifting in the value initialization is used to easily create
     * progressing bit patterns like so:
     *  1 << 0 equals 0001 equals 0x1
     *  1 << 1 equals 0010 equals 0x2
     *  1 << 2 equals 0100 equals 0x4
     *  1 << 3 equals 1000 equals 0x8
     */
    enum ToggleFlag
    {
        EShowFavorites  = 1 << 0
    };
    Q_DECLARE_FLAGS( Toggles, ToggleFlag )

public:

    RadioSettingsPrivate( RadioSettings* settingsPublic );

    ~RadioSettingsPrivate();

    void init( MRadioApplicationSettings* storage );

    // First time start
    bool isFirstTimeStart();
    void setFirstTimeStartPerformed( bool firstTimeStartPerformed );

    bool isFlagSet( ToggleFlag flag ) const;
    void toggleFlag( ToggleFlag flag );
    void saveFlags();

private: // data

    /**
     * Pointer to the public class
     * Not own.
     */
    RadioSettings*              q_ptr;

    Toggles                     mFlags;

    MRadioApplicationSettings*  mSettingsStorage;

};

#endif // RADIOSETTINGS_P_H_
