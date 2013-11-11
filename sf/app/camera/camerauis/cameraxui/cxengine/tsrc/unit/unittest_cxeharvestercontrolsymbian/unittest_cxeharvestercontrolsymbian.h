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


#ifndef UNITTEST_CXEHARVESTERCONTROLSYMBIAN_H
#define UNITTEST_CXEHARVESTERCONTROLSYMBIAN_H

#include <QObject>

// forward declaration
class CxeHarvesterControlSymbian;

class UnitTestCxeHarvesterControlSymbian : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeHarvesterControlSymbian();
    ~UnitTestCxeHarvesterControlSymbian();

private slots:
    void init();
    void cleanup();

    void testHarvestFile();

private:
    //data members
    CxeHarvesterControlSymbian* mHarvesterControl;
};

#endif //UNITTEST_CXEHARVESTERCONTROLSYMBIAN_H
