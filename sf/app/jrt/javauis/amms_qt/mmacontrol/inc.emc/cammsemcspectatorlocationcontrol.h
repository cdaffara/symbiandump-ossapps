/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Manipulates the virtual location of the SoundSource3D.
*
*/


#ifndef CAMMSEMCSPECTATORLOCATIONCONTROL_H
#define CAMMSEMCSPECTATORLOCATIONCONTROL_H

//  INCLUDES
#include <e32base.h>
#include<ListenerLocationControl.h>
#include <LocationData.h>
#include "cammslocationcontrol.h"
#include <cmmaemcaudioplayer.h>

using multimedia :: MListenerLocationControl;
using multimedia :: KListenerLocationEffectControl;

// CONSTANTS
_LIT(KAMMSEMCSpectatorLocationControl, "LocationControl");

// CLASS DECLARATION

/**
*
*  Controls for the Location Control effect for the Audio source.
*  This class delegates Location Control effect method calls to
*  MSourceLocationControl.
*
*
*  @since 5.1
*/
NONSHARABLE_CLASS(CAMMSEMCSpectatorLocationControl): public CAMMSLocationControl
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aPlayer Player that has this control.
    */
    static CAMMSEMCSpectatorLocationControl* NewLC(CMMAPlayer* aPlayer);

    /**
    * Destructor.
    */
    ~CAMMSEMCSpectatorLocationControl();

public: // Functions from base classes
    const TDesC& ClassName() const;

    /**
    * Prepares the Control.
    */
    void PrepareControlL();

    /**
    * Deallocates the Control.
    */
    void DeallocateControl();

private:
    /**
    * C++ constructor.
    * @param aPlayer Player that has this control.
    */
    CAMMSEMCSpectatorLocationControl(CMMAPlayer* aPlayer);


public:

    void SetLocationCartesianL(TInt& aX, TInt& aY, TInt& aZ);

    void LocationCartesian(TInt& aX, TInt& aY, TInt& aZ);

    void SetLocationSphericalL(TInt& aAzimuth, TInt& aElevation,
                               TInt& aRadius);

private:

    CMMAPlayer *iMMAPlayer;
    /**
    * EMC Listener Location Control
    */

    CMultimediaFactory* iFactory;
    MStreamControl* iStreamControl;
    MListenerLocationControl *iMListenerLocationControl;
};

#endif // CAMMSEMCSPECTATORLOCATIONCONTROL_H
