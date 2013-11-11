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
#ifndef UNITTEST_CXECAMERADEVICE_H
#define UNITTEST_CXECAMERADEVICE_H

#include <QObject>
#include <QDebug>

#include <ecam.h>
#include "cxenamespace.h"

class CxeCameraDevice;
class CxeDummyCamera;

class UnitTestCxeCameraDevice : public QObject, public MCameraObserver2
{
    Q_OBJECT

public:
    UnitTestCxeCameraDevice();
    ~UnitTestCxeCameraDevice();

    // from MCameraObserver2
    virtual void HandleEvent(const TECAMEvent& /*aEvent*/){};
    virtual void ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/){};
    virtual void ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/){};
    virtual void VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/){};

private slots:
    void init();
    void cleanup();

    void testNewCamera();
    void testReleaseCamera();
    void testDeleteCamera();
    void testSetCamera();

private:

    // class to be tested
    CxeCameraDevice *mCameraDevice;

};


#endif /* UNITTEST_CXECAMERADEVICE_H_ */
