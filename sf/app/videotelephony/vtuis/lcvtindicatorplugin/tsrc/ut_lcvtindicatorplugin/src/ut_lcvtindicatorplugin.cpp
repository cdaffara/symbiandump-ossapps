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
#include "ut_lcvtindicatorplugin.h"
#include "lcvtindicatorplugin.h"
#include "qprocess.h"

#include <QtTest/QtTest>

const static QString lcVtIndicator = "com.nokia.hb.indicator.lcvtindicatorplugin/1.0";


void UT_LcVtIndicatorPlugin::initTestCase()
{

}

void UT_LcVtIndicatorPlugin::cleanupTestCase()
{
}

void UT_LcVtIndicatorPlugin::init()
{
    mLcVtIndicatorPlugin = new LcVtIndicatorPlugin();
}

void UT_LcVtIndicatorPlugin::cleanup()
{
    delete mLcVtIndicatorPlugin;
    mLcVtIndicatorPlugin = 0;
}

void UT_LcVtIndicatorPlugin::testLcVtIndicatorPlugin()
{
    QVERIFY( mLcVtIndicatorPlugin );
}

void UT_LcVtIndicatorPlugin::testIndicatorTypes()
{
	QCOMPARE(mLcVtIndicatorPlugin->indicatorTypes()[0], lcVtIndicator);
}

void  UT_LcVtIndicatorPlugin::testAccessAllowed()
{
	QVERIFY( mLcVtIndicatorPlugin->accessAllowed(QString(), QVariantMap()) );
}

void UT_LcVtIndicatorPlugin::testCreateIndicator()
{
	mLcVtIndicatorPlugin->createIndicator(lcVtIndicator);
}

void UT_LcVtIndicatorPlugin::testError()
{
    QVERIFY( !mLcVtIndicatorPlugin->error() );
}


void UT_LcVtIndicatorPlugin::testHandleInteraction()
{
    QSignalSpy spy(mLcVtIndicatorPlugin, SIGNAL(deactivate()));

    //No interation requered
    QVERIFY( !mLcVtIndicatorPlugin->handleInteraction(
                 HbIndicatorInterface::NoInteraction) );
    QCOMPARE( spy.count(), 0 );
    
    //Indicator activated, no VT running => deactivating    
    QVERIFY( !mLcVtIndicatorPlugin->handleInteraction(
                     HbIndicatorInterface::InteractionActivated) );
    QCOMPARE( spy.count(), 1 );
    
    //Indicator activated, VT running => bringing to FG    
    spy.clear();
    QVERIFY( mLcVtIndicatorPlugin->bringVtToForeground() );    
    QVERIFY( QProcess::isRunning );    
    QCOMPARE( spy.count(), 0 );   
}

void UT_LcVtIndicatorPlugin::testIndicatorData()
{
    //No params defined
    QVERIFY( mLcVtIndicatorPlugin->indicatorData(
                  HbIndicatorInterface::DecorationNameRole) == QVariant(QString()) );
    QVERIFY( mLcVtIndicatorPlugin->indicatorData(
                  HbIndicatorInterface::MonoDecorationNameRole) == QVariant(QString()) );
    QVERIFY( mLcVtIndicatorPlugin->indicatorData(
                  HbIndicatorInterface::PrimaryTextRole) == QVariant(QString()) );
    QVERIFY( mLcVtIndicatorPlugin->indicatorData(
                  HbIndicatorInterface::SecondaryTextRole) == QVariant() );
    //Non-existing param role
    QVERIFY( mLcVtIndicatorPlugin->indicatorData(999) == QVariant() );
    
    //Parameters defined
    QVariantMap parameters;
    parameters.insert(
                QVariant(HbIndicatorInterface::DecorationNameRole).toString(),
                QVariant("decoration") );
    parameters.insert(
                QVariant(HbIndicatorInterface::MonoDecorationNameRole).toString(),
                QVariant("mono_decoration") );
    parameters.insert(
                QVariant(HbIndicatorInterface::PrimaryTextRole ).toString(),
                QVariant("primary_text") );
    parameters.insert(
                QVariant(HbIndicatorInterface::SecondaryTextRole ).toString(),
                QVariant("secondary_text") );
    
    mLcVtIndicatorPlugin->mParameter = parameters;
    
    QVERIFY( mLcVtIndicatorPlugin->indicatorData(
                                  HbIndicatorInterface::DecorationNameRole)
              == QVariant("decoration") );
    QVERIFY( mLcVtIndicatorPlugin->indicatorData(
                                  HbIndicatorInterface::MonoDecorationNameRole)
              == QVariant("mono_decoration") );
    QVERIFY( mLcVtIndicatorPlugin->indicatorData(
                                  HbIndicatorInterface::PrimaryTextRole) 
              == QVariant("primary_text") );
    QVERIFY( mLcVtIndicatorPlugin->indicatorData(
                                  HbIndicatorInterface::SecondaryTextRole)
              == QVariant() );
}

void UT_LcVtIndicatorPlugin::testHandleClientRequest()
{
    QVariantMap parameters;
    parameters.insert("key", QVariant("value"));
    mLcVtIndicatorPlugin->mParameter = parameters;
    QSignalSpy spy(mLcVtIndicatorPlugin, SIGNAL(dataChanged()));
    
    // Deactivate, params cleareds
    QVERIFY( !mLcVtIndicatorPlugin->mParameter.isNull() );
    QVERIFY( !mLcVtIndicatorPlugin->handleClientRequest(
                HbIndicatorInterface::RequestDeactivate, QVariant()) );
    QCOMPARE( spy.count(), 0 );
    QVERIFY( mLcVtIndicatorPlugin->mParameter.isNull() );
    
    // Activate request, parameters changed
    parameters.insert("key", QVariant("value"));
    QVERIFY( mLcVtIndicatorPlugin->handleClientRequest(
                    HbIndicatorInterface::RequestActivate, QVariant(parameters)) );
    QCOMPARE( spy.count(), 1 );

    // Activate request, parameters not changed
    spy.clear();
    QVERIFY( mLcVtIndicatorPlugin->handleClientRequest(
                    HbIndicatorInterface::RequestActivate, QVariant(parameters)) );
    QCOMPARE( spy.count(), 0 );
}

void UT_LcVtIndicatorPlugin::testBringVtToForeground()
{    
    QVERIFY( !mLcVtIndicatorPlugin->bringVtToForeground() );
    QVERIFY( QProcess::isRunning );    
}

