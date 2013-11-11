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

#ifndef UNITTEST_MPDETAILSVIEW_H_
#define UNITTEST_MPDETAILSVIEW_H_

#include <QtTest/QtTest>
#include <hbview.h>
#include <QNetworkReply>

//Forward Declaration
class MpDetailsView;



class TestMpDetailsView : public HbView
{
    Q_OBJECT

public:

    TestMpDetailsView();
    virtual ~TestMpDetailsView();
    

signals:
    void back ();    
    void albumArtChanged();
 
public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
private slots:
  //Test Cases  
    void testactivateView();
    void testdeactivateView();
    void testbackSlot();
    void testAlbumArtChanged();
    void testHandlePlaybackInfoChanged();
    void testHandleDetailsGroupBoxToggled();
    void testHandleInspireMeGroupBoxToggled();
    
private:
    MpDetailsView *mTest;
    
};

#endif /* UNITTEST_MPDETAILSVIEW_H_ */
