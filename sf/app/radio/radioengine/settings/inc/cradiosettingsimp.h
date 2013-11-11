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

#ifndef C_RADIOSETTINGSIMP_H
#define C_RADIOSETTINGSIMP_H

// System includes
#include <barsc.h>
// User includes
#include "cradiosettings.h"

// Forward declarations
class CRadioApplicationSettings;
class CRadioEngineSettings;
class RConeResourceLoader;

// The name of the radio settings resource file.
_LIT( KRadioSettingsResourceFile, "fmradioenginesettings.rsc" );

/**
 * Manages persistent application settings.
 *
 * Exposes access to the underlying settings implementations.
 */
NONSHARABLE_CLASS( CRadioSettingsImp ) : public CRadioSettings
    {

public:

    static CRadioSettingsImp* NewL();

    ~CRadioSettingsImp();

    RFs& FsSession();
    
// from base class CRadioSettings

    TBool IsRegionAllowed( TRadioRegion aRegionId ) const;
    MRadioApplicationSettings& ApplicationSettings() const;
    MRadioEngineSettings& EngineSettings() const;
    MRadioSettingsSetter& RadioSetter() const;
    void ResolveDriveL( TFileName& aFileName, const TDesC& aPath );

private:

    CRadioSettingsImp();

    void ConstructL();

private: // data

    /**
     * Implementation of the application settings.
     * Own
     */
    CRadioApplicationSettings*  iApplicationSettings;

    /**
     * Implementation of the radio settings.
     * Own
     */
    CRadioEngineSettings*       iEngineSettings;
    
    /**
     * File session.
     * Own
     */
    RFs*                        iFsSession; 

    };

#endif // C_RADIOSETTINGSIMP_H

