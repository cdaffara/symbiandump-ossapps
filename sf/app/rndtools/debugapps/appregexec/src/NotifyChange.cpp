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


// System includes
#include <swi/sisregistrysession.h>
#include <bautils.h>
//User Includes
#include "NotifyChange.h"

// Assuming that the path of registration resources will be @ c:\private\10003a3f\imports\apps
_LIT(KDirPath,":\\private\\10003a3f\\import\\apps\\");
// Wildcard to get only registration resources from above Path
_LIT(KWildName,"*_reg.rsc");

/*
 *NewL- Wrapper funtion to invoke the constructor of CNotifyChange class. 
 */
CNotifyChange* CNotifyChange::NewL(TChar aDriveLetter)
    {
    CNotifyChange* self=new (ELeave) CNotifyChange();
    CleanupStack::PushL(self);
    self->ConstructL(aDriveLetter);
    CleanupStack::Pop(self);
    return self;
    }

/*
 * Second Phase Constructor 
 */
void CNotifyChange::ConstructL(TChar aDriveLetter) // second-phase constructor
    {
    User::LeaveIfError(iFs.Connect());
    TBuf<KMaxFileName> aPath;
    aPath.Append(aDriveLetter);
    aPath.Append(KDirPath);
    iPath = aPath.AllocL();
    }

/*
 * Constructor
 */
CNotifyChange::CNotifyChange():CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

/*
 * StartFilesystemMonitor is used for issuing a Notify Change request to FileSystem for particular path
 */
void CNotifyChange::StartFilesystemMonitor()
    {
    TRequestStatus status;
    iFs.NotifyChange(ENotifyWrite,status,*iPath);
    SetActive();
    RDebug::Print(_L("*********** Isuued Notify Change Request ************ "));
    OstTrace0( TRACE_NORMAL, __STARTFILESYSTEMMONITOR, "AppRegExec::StartFilesystemMonitor - Isuued Notify Change Request");
    }

/*
 * RunL - Override function of CActive Class . will be invoked once FileSystem completes the request.
 */
void CNotifyChange::RunL()
    {
    TInt status = iStatus.Int();
    if ( status == KErrCancel )
        {
        OstTrace1( TRACE_FATAL, __RUNL, "AppRegExec::RunL - Cancelled;status=%d",status);
        return;
        }
    
    GetFilesFromDirL();
    // Issue notify Request Again
    StartFilesystemMonitor();
    }

/*
 * DoCancel - Override function of CAtive to Cancel any out standing request.
 */
void CNotifyChange::DoCancel()
    {
    // Cancel the outstanding file system request.
    iFs.NotifyChangeCancel(iStatus);
    }

/*
 * RunError- Override function of CActive class will be used to handle the error if any occurs in RunL .
 */
TInt CNotifyChange::RunError(TInt aError)
    { 
    OstTrace1( TRACE_FATAL, __RUNERROR, "AppRegExec::RunError;aError=%d",aError);
    return (KErrNone);                 // Error has been handled.
    }
 
/*
 * Destructor
 */
CNotifyChange::~CNotifyChange()
    {
    Cancel();
    if(iPath)
        {
        delete iPath;
        }
    iFs.Close();
    }

/*
 * GetFileFromDir- will be used to get the list of registration resources from a specified path.
 */
void CNotifyChange::GetFilesFromDirL()
    {
    iFs.SetSessionPath(iPath->Des());
    CDir* fileList;
    User::LeaveIfError(iFs.GetDir(KWildName,KEntryAttNormal,ESortByDate,fileList));
    TInt count = fileList->Count();
    if(count > 0)
        {
        TEntry entry = (*fileList)[count -1];
        TPtrC filename = entry.iName;
        TBuf<KMaxFileName> buf;
        buf.Append(iPath->Des());
        buf.Append(filename);
        TBool ret = iFs.IsValidName(buf);
        if(ret)
            {
            OstTrace0( TRACE_NORMAL, __GETFILESFROMDIR, "AppRegExec::GetFilesFromDir - Registrating Resources ...");
            RegisterwithApparc(buf);
            }
        else
            {
            OstTrace1( TRACE_FATAL, __GETFILESFROMDIR_NOTVALID, "AppRegister::GetFilesFromDir - File doesn't Exist;ret=%d",ret);
            }
        }
    }

/*
 * RegisterwithApparc - will be used to register the resource with APPARC using SWI API's
 */
void CNotifyChange::RegisterwithApparc(TDesC& aFileName)
    {
    Swi::RSisRegistrySession rServer;
    TInt ret = rServer.Connect();
    OstTrace1( TRACE_FATAL, __REGISTERWITHAPPARC_CONNECTION, "AppRegExec::RegisterwithApparc - Connection status;ret=%d",ret);
    CleanupClosePushL( rServer );
    TRAP(ret, rServer.AddAppRegInfoL(aFileName));
    RDebug::Print(aFileName);
    OstTrace1( TRACE_FATAL, __REGISTERWITHAPPARC_REGITERED, "AppRegExec::RegisterwithApparc - Registered Resource;ret= %d",ret);
    CleanupStack::PopAndDestroy(); //rServer*/
         
    }

