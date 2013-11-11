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
* Description:  RFS ECOM plugin for Internet Radio
*
*/


#ifndef C_IRRFSPLUGIN_H
#define C_IRRFSPLUGIN_H

#include <rfsPlugin.h>

/**
 *  Internet Radio RFS ECOM plugin implementation.
 *
 *  Handles deleting of Internet Radio files when RFS operation is invoked.
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CIRRfsPlugin ) : public CRFSPlugin
    {

public:

    /**
     * Two-phased constructor.
     *
     * @since   S60 v3.2
     * @param   aInitParams   Initialisation parameters.
     * @return  The created object.
     */
    static CIRRfsPlugin* NewL( TAny* aInitParams );

    /**
     * Destructor.
     *
     * @since   S60 v3.2
     */
    ~CIRRfsPlugin();

private:

    /**
     * Constructor.
     *
     * @since   S60 v3.2
     * @param   aInitParams   Initialisation parameters.
     */
    CIRRfsPlugin( TAny* aInitParams );

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

    };

#endif // C_CIRRFSPLUGIN_H
