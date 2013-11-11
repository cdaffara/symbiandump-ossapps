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
#ifndef UT_LOGSTHUMBNAILMANAGER_H  
#define UT_LOGSTHUMBNAILMANAGER_H  

#include <QObject>
#include <QIcon>

class LogsThumbIconManager;


class UT_LogsThumbnailManager  : public QObject
{
Q_OBJECT

private slots:
	void initTestCase();
	void cleanupTestCase();
    void init();
    void cleanup();
	  
private slots: //test methods    
	void testConstructor();
    void testNonExistingIcon();
    void testOneExistingIcon();
    void testCancel();
 
	
private:
	
    LogsThumbIconManager* mIconMgr; // SUT 
    QIcon* mIcon;
  
};

#endif //UT_LOGSTHUMBNAILMANAGER_H  
