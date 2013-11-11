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


//  Include Files  


#include <e32base.h>
#include <e32std.h>
#include <f32file.h>
#include "AppRegExec.h"
#include "NotifyChange.h"

//  Constants
//  Global Variables
//  Local Functions

LOCAL_C void MainL()
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    TInt driveNumber=EDriveA; 
    TChar driveLetter; 
    TDriveList drivelist; 
    User::LeaveIfError(fs.DriveList(drivelist));
    TInt count = 0; 
    CNotifyChange* notify;
    for (driveNumber=EDriveA; driveNumber<=EDriveZ;driveNumber++)
        {
        // if drive-list entry non-zero, drive is available
        if (drivelist[driveNumber]) 
            {
            User::LeaveIfError(fs.DriveToChar(driveNumber,driveLetter));
            notify = CNotifyChange::NewL(driveLetter);
            CleanupStack::PushL(notify);
            count ++;
            notify->StartFilesystemMonitor();
            }
        }
    CActiveScheduler::Start();
    fs.Close();
    CleanupStack::PopAndDestroy(count);
    CleanupStack::PopAndDestroy(&fs);
    }

LOCAL_C void DoStartL()
    {
    // Create active scheduler (to run active objects)
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);

    MainL();

    // Delete active scheduler
    CleanupStack::PopAndDestroy(scheduler);
    }

//  Global Functions

GLDEF_C TInt E32Main()
    {
    // Create cleanup stack
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    // Run application code inside TRAP harness, wait keypress when terminated
    TRAPD(mainError, DoStartL());
    if(mainError != KErrNone)
        {
        OstTrace1( TRACE_FATAL, __E32MAIN_DOSTARTL_FAIL, "AppRegExec::E32Main - DoStartL failed;mainError=%d", mainError );
        }
    delete cleanup;
    __UHEAP_MARKEND;
    return mainError;
    }

