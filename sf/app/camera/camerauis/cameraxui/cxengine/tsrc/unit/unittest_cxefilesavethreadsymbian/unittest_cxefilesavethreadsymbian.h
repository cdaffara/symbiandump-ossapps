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
#ifndef UNITTEST_CXEFILESAVETHREADSYMBIAN_H
#define UNITTEST_CXEFILESAVETHREADSYMBIAN_H

#include <QObject>

// forward declartion
class CxeFileSaveThreadSymbian;

// class defining unit test cases for CxeFileSaveThreadSymbian engine class

class UnitTestCxeFileSaveThreadSymbian : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeFileSaveThreadSymbian();
    ~UnitTestCxeFileSaveThreadSymbian();

private slots:
    void init();
    void cleanup();
    void testSave();


private:
    // data
    CxeFileSaveThreadSymbian *mFileSaveThread;
};

#endif // UNITTEST_CXEFILESAVETHREADSYMBIAN_H
