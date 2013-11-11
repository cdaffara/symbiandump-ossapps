/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QTest>
#include <QThread>
#include <QByteArray>
#include <QSignalSpy>
#include <locationtrailpskeys.h>
#include <rlocationtrail.h>

#include "unittest_cxegeotaggingtrail.h"
#include "cxefakestillcapturecontrol.h"
#include "cxefakevideocapturecontrol.h"
#include "cxegeotaggingtrail.h"
#include "cxefakesettings.h"
#include "cxetestutils.h"
#include "cxenamespace.h"




/**
* UnitTestCxeGeoTaggingTrail::UnitTestCxeGeoTaggingTrail()
*/
UnitTestCxeGeoTaggingTrail::UnitTestCxeGeoTaggingTrail()
: mTrail(NULL), mSettings(NULL)
{
}

/**
* UnitTestCxeGeoTaggingTrail::~UnitTestCxeGeoTaggingTrail()
*/
UnitTestCxeGeoTaggingTrail::~UnitTestCxeGeoTaggingTrail()
{
    cleanup();
}

/**
* UnitTestCxeGeoTaggingTrail::init()
*/
void UnitTestCxeGeoTaggingTrail::init()
{
    CxeFakeStillCaptureControl *fakeStillControl = new CxeFakeStillCaptureControl();
    CxeFakeVideoCaptureControl *fakeVideoControl = new CxeFakeVideoCaptureControl();
    mSettings = new CxeFakeSettings();
    
    CxeStillCaptureControl *stillControl = qobject_cast<CxeStillCaptureControl*>(fakeStillControl); 
    CxeVideoCaptureControl *videoControl = qobject_cast<CxeVideoCaptureControl*>(fakeVideoControl); 
    
    // trail is started automatically during construction
    mTrail = new CxeGeoTaggingTrail(*stillControl, *videoControl, *mSettings);
    QCOMPARE(mTrail->state(), CxeGeoTaggingTrail::NotConnected);    
}

/**
* UnitTestCxeGeoTaggingTrail::cleanup()
*/
void UnitTestCxeGeoTaggingTrail::cleanup()
{
    delete mTrail;
    mTrail = NULL;
    
    delete mSettings;
    mSettings = NULL;
}


/**
* UnitTestCxeGeoTaggingTrail::testStart()
*/
void UnitTestCxeGeoTaggingTrail::testStart()
{
    int settingValue;
    // case 1: try to start/stop geotagging trail when setting is off
    settingValue = 0;
    mSettings->set(CxeSettingIds::GEOTAGGING, settingValue);
    mTrail->start();
    QCOMPARE(mTrail->state(), CxeGeoTaggingTrail::NotConnected);
    
    mTrail->stop();
    QCOMPARE(mTrail->state(), CxeGeoTaggingTrail::NotConnected);
    
    // case 2: geotagging setting is ON, starting trail and emulating real state changes in P&S key
    settingValue = 1;
    mSettings->set(CxeSettingIds::GEOTAGGING, settingValue);
    mTrail->start();
    
    QCOMPARE(mTrail->state(), CxeGeoTaggingTrail::TrailStarted);
    
    // trail started, emulate as if P&S key has state updates from RLocationTrail.
    int newState = RLocationTrail::ETrailStarted;
    mSettings->emulate(KPSUidLocationTrail.iUid, KLocationTrailState, newState);
    QCOMPARE(mTrail->state(), CxeGeoTaggingTrail::DataAvailable);
        
    // case 3: geotagging setting on, trail started, but RLocationTrail doesnt get trail ongoing
    settingValue = 1;
    mSettings->set(CxeSettingIds::GEOTAGGING, settingValue);
    mTrail->start();
    
    QCOMPARE(mTrail->state(), CxeGeoTaggingTrail::DataAvailable);
    
    // trail started, emulate as if P&S key has state updates from RLocationTrail.
    newState = RLocationTrail::EWaitingGPSData;
    mSettings->emulate(KPSUidLocationTrail.iUid, KLocationTrailState, newState);
    QCOMPARE(mTrail->state(), CxeGeoTaggingTrail::DataAvailable);
    
    // we now try to stop trail
    mTrail->stop();
    QCOMPARE(mTrail->state(), CxeGeoTaggingTrail::Connected);

    // Case 4: just changing the setting value
    // setting value is ON, trail should start
    settingValue = 1;
    mSettings->set(CxeSettingIds::GEOTAGGING, settingValue);
    QCOMPARE(mTrail->state(), CxeGeoTaggingTrail::TrailStarted);
    
    // trail started, emulate as if P&S key has state updates from RLocationTrail.
    newState = RLocationTrail::ETrailStarted;
    mSettings->emulate(KPSUidLocationTrail.iUid, KLocationTrailState, newState);
    QCOMPARE(mTrail->state(), CxeGeoTaggingTrail::DataAvailable);
    
    // setting value is OFF, trail should stop
    settingValue = 0;
    mSettings->set(CxeSettingIds::GEOTAGGING, settingValue);
    QCOMPARE(mTrail->state(), CxeGeoTaggingTrail::Connected);
    
}

/**
* UnitTestCxeGeoTaggingTrail::testStop()
*/
void UnitTestCxeGeoTaggingTrail::testStop()
{
    // case 1: Trying to stop the trail even before starting
    QCOMPARE(mTrail->state(), CxeGeoTaggingTrail::NotConnected);
    mTrail->stop();
    QCOMPARE(mTrail->state(), CxeGeoTaggingTrail::NotConnected);
    
    // case 2: trail connected but not started
    int settingValue = 1;
    mSettings->set(CxeSettingIds::GEOTAGGING, settingValue);
    mTrail->start();
    
    QCOMPARE(mTrail->state(), CxeGeoTaggingTrail::TrailStarted);
    
    // now stopping trail:
    mTrail->stop();
    // here we are connected to trail but trail not started.
    QCOMPARE(mTrail->state(), CxeGeoTaggingTrail::Connected);
    
    // trying to stop the trail again
    mTrail->stop();
    QCOMPARE(mTrail->state(), CxeGeoTaggingTrail::Connected);
   
}



/**
* UnitTestCxeGeoTaggingTrail::testState()
*/
void UnitTestCxeGeoTaggingTrail::testState()
{
    QCOMPARE(mTrail->state(), CxeGeoTaggingTrail::NotConnected);
}

// main() function
QTEST_MAIN(UnitTestCxeGeoTaggingTrail);

// end of file
