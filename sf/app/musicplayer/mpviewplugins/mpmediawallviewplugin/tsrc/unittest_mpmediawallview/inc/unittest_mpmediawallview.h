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
* Description: Unit Test for mpdetailsview.
*
*/

#ifndef UNITTEST_MPMEDIAWALLVIEW_H_
#define UNITTEST_MPMEDIAWALLVIEW_H_

#include <QtTest/QtTest>

//Forward Declaration
class MpMediaWallView;



class TestMpMediaWallView : public QObject
{
    Q_OBJECT

public:

    TestMpMediaWallView();
    virtual ~TestMpMediaWallView();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
private slots:

    void testInitializeView();
    void testDeactivateView();   
    void testContextOpened();
    void testContainerContentsChanged();
    void testLibraryUpdated();
    void testListItemActivated();

private:
    MpMediaWallView *mTest;
    
};

#endif /* UNITTEST_MPMEDIAWALLVIEW_H_ */
