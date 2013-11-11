/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef T_RADIOPRESETSTORAGE_H_
#define T_RADIOPRESETSTORAGE_H_


// INCLUDES
#include <QtTest/QtTest>
#include "radiopresetstorage.h"


class RadioPresetStorage;

class TestRadioPresetStorage : public QObject
{
    Q_OBJECT    
    
public:

    TestRadioPresetStorage();
    ~TestRadioPresetStorage();    

public slots:
    
private slots:
    // test framework called slots 
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();
    void testSavePreset();
    void testReadPreset();
    void testDeletePreset();
    void testStressTest();
    
private:
    
private:
	QScopedPointer<RadioPresetStorage>  mPresetStorage;
};

#endif /* T_RADIOPRESETSTORAGE_H_ */
