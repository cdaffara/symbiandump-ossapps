/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef CXESETTINGSMAPPERSYMBIAN_H
#define CXESETTINGSMAPPERSYMBIAN_H

#include <ecam.h>
#include <ecamadvsettings.h>
#include <ECamOrientationCustomInterface2.h>
#include <ecamusecasehintcustomapi.h>
#include "cxeautofocuscontrol.h"
#include "cxenamespace.h"
#include "cxequalitydetails.h"

/*!
    Map CameraX specific enums to ECam equivalents
*/
class CxeSettingsMapperSymbian
{
public:
    static CCamera::TWhiteBalance Map2CameraWb(Cxe::Whitebalance whiteBalance);
    static CCamera::TFlash Map2CameraFlash(Cxe::FlashMode flashMode);
    static CCamera::TExposure Map2CameraExposureMode(
            Cxe::ExposureMode exposureMode);
    static CCamera::CCameraImageProcessing::TEffect Map2CameraEffect(
            Cxe::Colortone colorTone);
    static CCamera::CCameraAdvancedSettings::TFocusRange Map2CameraAutofocus(
            CxeAutoFocusControl::Mode afMode);
    static MCameraOrientation::TOrientation Map2CameraOrientation(
            Cxe::DeviceOrientation orientation);
    static void Map2UseCaseHintVideoParameters(
            const CxeVideoDetails &videoDetails,
            MCameraUseCaseHint::TVideoCodec &codec,
            MCameraUseCaseHint::TVideoProfile &profile);
};

#endif /*CXESETTINGSMAPPERSYMBIAN_H*/

// end  of file
