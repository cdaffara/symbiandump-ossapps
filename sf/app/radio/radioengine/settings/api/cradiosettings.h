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

#ifndef C_RADIOSETTINGS_H
#define C_RADIOSETTINGS_H

// System includes
#include <e32base.h>

// User includes
#include "radioengine.hrh"

// Forward declarations
class CCoeEnv;
class MRadioApplicationSettings;
class MRadioEngineSettings;
class MRadioSettingsSetter;
class CRadioRepositoryManager;
class MRadioPresetSettings;
class RFs;

/**
 * Manages persistent application settings.
 *
 * Exposes access to the underlying settings implementations.
 */
NONSHARABLE_CLASS( CRadioSettings ) : public CBase
    {

public:

    IMPORT_C static CRadioSettings* NewL();

    IMPORT_C virtual ~CRadioSettings();

    /**
     * Determines if region is allowed.
     */
    virtual TBool IsRegionAllowed( TRadioRegion aRegionId ) const = 0;

    /**
     * Returns the implemented application settings interface.
     *
     * @return  The implemented application settings interface.
     */
    virtual MRadioApplicationSettings& ApplicationSettings() const = 0;

    /**
     * Returns the implemented radio settings interface.
     *
     * @return  The implemented radio settings interface.
     */
    virtual MRadioEngineSettings& EngineSettings() const = 0;

    /**
     * Returns the implemented radio settings interface.
     *
     * @return  The implemented setter interface for radio settings.
     */
    virtual MRadioSettingsSetter& RadioSetter() const = 0;

    /**
     * Resolves the most recently updated location for a file.
     *
     * @param   aFileName      The file name to be searched.
     *                         On return, will contain the full path to the file.
     * @param   aPath          The absolute path to the file. If drive letter is
     *                         given then that one will be first tried.
     */
    virtual void ResolveDriveL( TFileName& aFileName, const TDesC& aPath ) = 0;

    };

#endif // C_RADIOSETTINGS_H

