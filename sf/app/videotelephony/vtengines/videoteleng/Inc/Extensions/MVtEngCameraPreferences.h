/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Videoteleng Camera Preferences extension API
*
*/


#ifndef MVTENGCAMERAPREFERENCES_H
#define MVTENGCAMERAPREFERENCES_H

//	INCLUDES
#include <ecam.h>
#include <ecamimageprocessing.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

// CONSTANTS
const TUid KVtEngExtensionCameraPreferences = {0x10282814};

/**
*  Camera preferences extension API class.
*
*  @lib videoteleng.lib
*  @since S60 v3.2
*/

class MVtEngCameraPreferences
    {
    public:

        enum TColorTone
        	{
    		/** Normal colors. */
            ENormal 	= CCamera::CCameraImageProcessing::EEffectNone,
            /** Sepial. */
            ESepia  	= CCamera::CCameraImageProcessing::EEffectSepia,
            /** Black and White. */
            EGrayscale 	= CCamera::CCameraImageProcessing::EEffectMonochrome,
            /** Inverted colors. */
            ENegative 	= CCamera::CCameraImageProcessing::EEffectNegative
            };

        enum TWhiteBalance
    		{
    		/** Automatically adjusted. */
    		EAuto 			= CCamera::EWBAuto,
    		/** Normal daylight. */
    		EDaylight		= CCamera::EWBDaylight,
    		/** Overcast daylight. */
    		ECloudy			= CCamera::EWBCloudy,
    		/** Tungsten filament lighting. */
    		ETungsten		= CCamera::EWBTungsten,
    		/** Fluorescent bulb lighting. */
    		EFluorescent 	= CCamera::EWBFluorescent,
    		/** Flash lighting. */
    		EFlash			= CCamera::EWBFlash,
    		/** High contrast daylight primarily snowy. */
    		ESnow 			= CCamera::EWBSnow,
    		/** High contrast daylight primarily near the sea. */
    		EBeach 			= CCamera::EWBBeach,
    		/** User configurable mode. */
    		EManual 		= CCamera::EWBManual,
    		/** Shade. */
     		EShade			= CCamera::EWBShade
            };

        enum TBrightness
        	{
        	/** Automatically set brightness value*/
            EAutomaticBrightness = KMinTInt
            };

        enum TContrasts
        	{
        	/** Automatically set contrast value*/
            EAutomaticContrast = KMinTInt
            };

        /**
        * Gets current color tone value from camera
        * @param aColortone reference value for current color tone
        * @return Symbian error code
        */
        virtual TInt GetColorTone( TColorTone& aColortone ) const = 0;

        /**
        * Gets all suported color tone values from camera
        * @param aSupportedColorTones reference value for supported colortone
        * values (a bit field)
        * @return Symbian error code
        */
        virtual TInt GetSupportedColorTones(
        	TUint32& aSupportedColorTones ) const = 0;

        /**
        * Gets current whitebalance value from camera
        * @param aWhiteBalance reference value for current whitebalance
        * @return Symbian error code
        */
        virtual TInt GetWhiteBalance(
        	TWhiteBalance& aWhiteBalance ) const = 0;

        /**
        * Gets all suported whitebalance values from camera
        * @param aWhiteBalance reference value for supported whitebalance
        * values (a bit field)
        * @return Symbian error code
        */
        virtual TInt GetSupportedWhiteBalanceModes(
        	TUint32& aWhiteBalance ) const = 0;

        /**
        * Gets current brightness value from camera
        * @param aBrightness reference value for current brightness
        * @return Symbian error code
        */
        virtual TInt GetBrightness( TInt& aBrightness) const = 0;

        /**
        * Gets current contrast value from camera
        * @param aContrast reference value for current brightness
        * @return Symbian error code
        */
        virtual TInt GetContrast( TInt& aContrast ) const = 0;

    };

#endif //MVTENGCAMERAPREFERENCES_H

