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
#ifndef UNITTEST_CXEERRORMAPPERSYMBIAN_H
#define UNITTEST_CXEERRORMAPPERSYMBIAN_H

#include <QObject>

/*
 * Class that defines unit test for cxeerrormappingsymbian
 */
class UnitTestCxeErrorMapperSymbian : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeErrorMapperSymbian();
    ~UnitTestCxeErrorMapperSymbian();

private slots:
    void init();
    void cleanup();
    void testSymbianErr2CxeErrorId();
};

#endif // UNITTEST_CXEERRORMAPPERSYMBIAN_H
