/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  RFS ECOM plugin for Preset Server
*
*/


#ifndef C_PSRRFSPLUGIN_H
#define C_PSRRFSPLUGIN_H

#include <rfsPlugin.h>

/**
 *  Preset Server RFS ECOM plugin implementation.
 *
 *  Handles deleting of Preset Server files when RFS operation is invoked.
 *
 *  @since S60 v3.2
 */
class CPSRfsPlugin : public CRFSPlugin
    {

public:

    /**
     * Two-phased constructor.
     *
     * @since   S60 v3.2
     * @param   aInitParams   Initialisation parameters.
     * @return  The created object.
     */
    static CPSRfsPlugin* NewL( TAny* aInitParams );

    /**
     * Destructor.
     *
     * @since   S60 v3.2
     */
    ~CPSRfsPlugin();

private:

    /**
     * Constructor.
     *
     * @since   S60 v3.2
     * @param   aInitParams   Initialisation parameters.
     */
    CPSRfsPlugin( TAny* aInitParams );

    /**
     * Second-phase constructor.
     *
     * @since   S60 v3.2
     */
    void ConstructL();

// from base class CRFSPlugin

    void RestoreFactorySettingsL( const TRfsReason aType );
    void GetScriptL( const TRfsReason aType, TDes& aPath );
    void ExecuteCustomCommandL( const TRfsReason aType, TDesC& aCommand );

    /**
     * Resolves the most recently updated location for a file.
     * The paths searched are, in order, RAM and MMC.
     * 
     * @param   aFs         The file server session to use.
     * @param   aFileName   The file name of the file which is wanted to find.
     *                      On return, will contain the full path to the file.
     * @param   aPath       The absolute path to the file. If drive letter is
     *                      given then that one will be first tried. 
     * @return KErrNotFound if the file is not found, otherwise KErrNone
     */
    TInt ResolveDrive( RFs& aFs, TFileName& aFileName, const TFileName& aPath );
    
    
    };

#endif // C_CPSRFSPLUGIN_H
