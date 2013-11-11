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
#ifndef CXEFAKEFILENAMEGENERATOR_H
#define CXEFAKEFILENAMEGENERATOR_H

#include "cxefilenamegeneratorsymbian.h"  //API
#include "cxestatemachine.h"

//forward declarations
class CxeCameraDevice;


/**
 * CxeFilenameGeneratorSymbian provides file names for saving images and videos.
 */
class CxeFakeFilenameGenerator : public CxeFilenameGenerator
{
public:

    CxeFakeFilenameGenerator();
    virtual ~CxeFakeFilenameGenerator();

public: // From CxeFilenameGenerator
    CxeError::Id init(Cxe::CameraMode mode = Cxe::ImageMode);
    void startNewImageFilenameSequence();
    CxeError::Id nextImageFilenameInSequence(QString &filename, const QString &fileExt);
    CxeError::Id generateFilename(QString &filename, const QString &fileExt);
    void raiseCounterValue();

private:
    // Current mode: image/video
    Cxe::CameraMode mCurrentMode;

};

#endif // CXEFAKEFILENAMEGENERATOR_H
