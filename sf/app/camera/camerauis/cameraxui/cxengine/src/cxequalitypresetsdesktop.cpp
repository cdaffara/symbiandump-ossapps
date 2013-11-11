/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cxutils.h"
#include "cxequalitydetails.h"
#include "cxequalitypresetsdesktop.h"

/*!
* Intializes ImagingConfigManager
*/
CxeQualityPresetsDesktop::CxeQualityPresetsDesktop()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();

}

/*!
* CxeQualityPresetsDesktop::~CxeQualityPresetsDesktop()
*/
CxeQualityPresetsDesktop::~CxeQualityPresetsDesktop()
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
This function returns sorted list of image qualities from highest to lowest resolution.
i.e. first element in the list represent highest supported image resolution and so on.
@param cameraId The CameraIndex which defines which camera we are using primary/secondary.
Returns sorted list of image qualities in descending order.
*/
QList<CxeImageDetails> CxeQualityPresetsDesktop::imageQualityPresets(Cxe::CameraIndex cameraId)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();

    return QList<CxeImageDetails>();
}



/*!
This function returns sorted list of video qualities from highest to lowest resolution.
i.e. first element in the list represent highest supported video resolution and so on.
@param cameraId The CameraIndex which defines which camera we are using primary/secondary.
Returns sorted list if image qualities in descending order.
*/
QList<CxeVideoDetails> CxeQualityPresetsDesktop::videoQualityPresets(Cxe::CameraIndex cameraId)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();

    return QList<CxeVideoDetails>();
}

int CxeQualityPresetsDesktop::recordingTimeAvailable(const CxeVideoDetails &details, qint64 space)
{
    Q_UNUSED(details);
    Q_UNUSED(space);

    return 0;
}

/*!
Operator to sort values in ascending order.
@param s1 type of data to be sorted.
*/
bool CxeImageDetails::operator<(const CxeImageDetails &s1) const
{
    return mHeight < s1.mHeight;
}


/*!
Operator to sort values in ascending order.
@param s1 type of data to be sorted.
*/
bool CxeVideoDetails::operator<(const CxeVideoDetails &s1) const
{
    return mHeight < s1.mHeight;
}
