/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Videoteleng Camera Preferences extension Observer
*
*/


#ifndef MVTENGCAMERAPREFERENCESOBSERVER_H
#define MVTENGCAMERAPREFERENCESOBSERVER_H

//	INCLUDES
#include <mvtengcamerapreferences.h>

/**
*  Camera preferences extension observer.
*
*  @lib videoteleng.lib
*  @since S60 v5.0
*/
class MVtEngCameraPreferencesObserver
    {

public:

    /**
    * Called when observer is attached to camera preferences instance.
    * @param aCamPrefs Reference to camera preferences instance this observer
    * was attached to.
    */
    virtual void Attach( MVtEngCameraPreferences& aCamPrefs ) = 0;

    /**
    * Called when color tone is updated.
    * @param aColorTone New color tone value.
    */
    virtual void ColorToneUpdated(
        MVtEngCameraPreferences::TColorTone aColorTone ) = 0;

    /**
    * Called when white balance is updated.
    * @param aWhiteBalance New white balance value.
    */
    virtual void WhiteBalanceUpdated(
        MVtEngCameraPreferences::TWhiteBalance aWhiteBalance ) = 0;

    /**
    * Called when brightness is updated.
    * @param aBrightness New brightness value set.
    */
    virtual void BrightnessUpdated( TInt aBrightness ) = 0;

    /**
    * Called when contrast is updated.
    * @param aContrast New contrast value set.
    */
    virtual void ContrastUpdated( TInt aContrast ) = 0;

    /**
    * Called when observer is detached from camera preferences instance.
    * @param aCamPrefs Reference to camera preferences instance this observer
    * was detached from.
    */
    virtual void Detach( MVtEngCameraPreferences& aCamPrefs ) = 0;

    };

#endif // MVTENGCAMERAPREFERENCESOBSERVER_H

