/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*       test application for qt control panel public apis.
*/

#ifndef UT_FMINTERNALDRIVEMODEL_H_
#define UT_FMINTERNALDRIVEMODEL_H_

#include <QObject>
#include <QTranslator>
#include <QStringList>
class TestFmInternalDriveModel :public QObject
{
    Q_OBJECT
private slots:    
    void initTestCase();    
    void testConstructor(); //test the constructor function. 
    void testRowCount(); // test the rowCount() function.
    void testColumnCount(); // test the columnCount() function.
    void testData();
    void testIndexValid(); // test the indexValid() function.
    void testHeaderData(); // test the headerData() function.
    
    void testDisplayStringWithVolume(); // test the displayString() function.
    void testDisplayStringWithoutVolume();
    void testDisplayStringEmpty();
    
    void testDriveName(); // test the driveName() function.        
    void cleanupTestCase();    
private:
    QTranslator mTranslator;
    QStringList mDriverList;
};
#endif /* UT_FMINTERNALDRIVEMODEL_H_ */
