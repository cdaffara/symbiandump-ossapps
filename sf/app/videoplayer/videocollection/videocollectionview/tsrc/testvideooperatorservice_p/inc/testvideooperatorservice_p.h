/**
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
* Description: tests for VideoOperatorService
*
*/

#ifndef TESTVIDEOOPERATORSERVICE_P_H
#define TESTVIDEOOPERATORSERVICE_P_H

// INCLUDE FILES
#include <qttest/qttest>
#include "apgclistub.h"
#include "apaidstub.h"
#include "coemainstub.h"
#include "apgtaskstub.h"

// FORWARD DECLARATIONS
class VideoOperatorServicePrivate;

class TestVideoOperatorServicePrivate : public QObject
{
    Q_OBJECT

public:
    TestVideoOperatorServicePrivate();
    virtual ~TestVideoOperatorServicePrivate();
    
private slots: // test functions for the test framework
    void init();
        
    void cleanup();
    
private slots:
    
    void testLoad();
    void testTitle();
    void testIconResource();
    void testLaunchService();
    void testSlots();
    
signals:
    
private:
    /** class under test */
    VideoOperatorServicePrivate *mTestObject;

};

#endif//TESTVIDEOOPERATORSERVICE_P_H
