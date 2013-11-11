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


#ifndef UNITTEST_CXETHUMBNAILMANAGERSYMBIAN_H
#define UNITTEST_CXETHUMBNAILMANAGERSYMBIAN_H

#include <QObject>

// forward declaration
class CxeThumbnailManagerSymbian;

class UnitTestCxeThumbnailManagerSymbian : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeThumbnailManagerSymbian();
    ~UnitTestCxeThumbnailManagerSymbian();

private slots:
    void init();
    void cleanup();

    void testCreateThumbnail();
    void testCancelThumbnail();

private:
    //data members
    CxeThumbnailManagerSymbian* mThumbnailManager;
};

#endif //UNITTEST_CXETHUMBNAILMANAGERSYMBIAN_H
