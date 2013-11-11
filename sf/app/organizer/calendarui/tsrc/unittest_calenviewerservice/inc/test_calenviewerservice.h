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

#ifndef __TEST_CALENVIWRSRV_H__
#define __TEST_CALENVIWRSRV_H__

#include <QObject>
#include <xqappmgr.h>

class TestCalenViewerService : public QObject
{
	Q_OBJECT
public:
    TestCalenViewerService();
    ~TestCalenViewerService();
    
private slots:
    void init();
    void cleanup();
    
    void testOpeningVCal();
    void testOpeningICal();
    
private:
    
};

#endif      // __TEST_CALENVIWRSRV_H__

// End of file
