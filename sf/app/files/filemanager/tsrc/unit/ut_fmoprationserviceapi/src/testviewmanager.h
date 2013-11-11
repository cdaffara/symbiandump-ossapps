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
*     The header file of filemanager view manager
*
*/

#ifndef TESTVIEWMANAGER_H
#define TESTVIEWMANAGER_H

#include <QObject>
#include <QList>
#include <QPixmap>

class HbView;
class FmViewManager;
class FmOperationService;
class FmOperationBase;
class HbMainWindow;

/*
 * this class is used to drive the test case.
 */

class TestViewManager : public QObject
{
    Q_OBJECT

public:
    static TestViewManager *CreateTestViewManager( HbMainWindow* mainWindow );
    static TestViewManager *viewManager();
    FmOperationService *operationService();
    static void RemoveViewManager();

protected:
    explicit TestViewManager( HbMainWindow* mainWindow );
    ~TestViewManager();
    
private:
     static TestViewManager *mTestViewManager;

private:
    // main window
    HbMainWindow        *mMainWindow;
    
    // operation service, used to start operations such as copy, move, delete, format...
    FmOperationService  *mOperationService;
};

#endif



