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
#ifndef CXEFAKEQUALITYPRESETS_H
#define CXEFAKEQUALITYPRESETS_H

#include "cxequalitypresets.h"


class CxeFakeQualityPresets : public CxeQualityPresets
{

public: // From base class
    CxeFakeQualityPresets();
    virtual ~CxeFakeQualityPresets();
    QList<CxeImageDetails> imageQualityPresets(Cxe::CameraIndex cameraId);
    QList<CxeVideoDetails> videoQualityPresets(Cxe::CameraIndex cameraId);
    int recordingTimeAvailable(const CxeVideoDetails& details, qint64 space);
public:
    static CxeImageDetails fakeImageDetails();
    static CxeVideoDetails fakeVideoDetails();
};

#endif // CXEFAKEZOOMCONTROL_H

// End of file
