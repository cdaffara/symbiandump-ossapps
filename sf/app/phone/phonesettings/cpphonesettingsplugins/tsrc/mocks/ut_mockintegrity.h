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

#ifndef UT_MOCKINTEGRITY_H
#define UT_MOCKINTEGRITY_H

#include <QObject>
#include <QtTest/QtTest>

class UT_MockIntegrity : public QObject
{
    Q_OBJECT
    
public:    
    UT_MockIntegrity();
    ~UT_MockIntegrity();
    
private slots:
    
    void init();
    void cleanup();
};

#endif // UT_MOCKINTEGRITY_H
