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

#ifndef CXEFILENAMEGENERATORDESKTOP_H
#define CXEFILENAMEGENERATORDESKTOP_H

#include "cxefilenamegenerator.h"

class CxeFilenameGeneratorDesktop : public CxeFilenameGenerator
{
public:
    CxeFilenameGeneratorDesktop();
    virtual ~CxeFilenameGeneratorDesktop();

public: // from CxeFilenameGenerator
    virtual CxeError::Id init(Cxe::CameraMode mode);
    virtual void startNewImageFilenameSequence();
    virtual CxeError::Id nextImageFilenameInSequence(QString &filename, const QString &fileExt);
    virtual CxeError::Id generateFilename(QString &filename, const QString &fileExt);
    virtual void raiseCounterValue();

private:
    void getNextVideoFilename(QString &filename);
    void getNextImageFilename(QString &filename);

    QString assembleFilename(const QString &baseName, int sequenceNumber, const QString &extension);

    Cxe::CameraMode mMode;

    QString mImageBaseFolder;
    QString mVideoBaseFolder;

    QString mImageExtension;
    QString mVideoExtension;

    int mImageSequenceNumber;
    int mVideoSequenceNumber;

};

#endif // CXEFILENAMEGENERATORDESKTOP_H
