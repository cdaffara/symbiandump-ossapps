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


#ifndef __FBFILEOPCLIENT_H__
#define __FBFILEOPCLIENT_H__

// INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "FBFileOpClientServer.h" // TFileOpArgs

// FORWARD DECLARATIONS

// CLASS DECLARATIONS

class RFBFileOpServerSession : public RSessionBase
    {
public:
    TInt Connect();
	
    void Copy(const TDesC& aSourceFullName, const TDesC& aTargetFullName, TUint aSwitch, TRequestStatus& aStatus);
    TInt Rename(const TDesC& anOld, const TDesC& aNew, TUint aSwitch); 
    TInt Attribs(const TDesC& aName, TUint aSetMask, TUint aClearMask, const TTime& aTime, TUint aSwitch); 
    void RmDir(const TDesC& aDirName, TUint aSwitch, TRequestStatus& aStatus); 
    void Delete(const TDesC& aName, TUint aSwitch, TRequestStatus& aStatus); 
    TInt MkDirAll(const TDesC& aPath); 
    TInt CreateEmptyFile(const TDesC& aName); 
    TInt EraseMBR(TUint aDriveNumber); 
    TInt PartitionDrive(TUint aDriveNumber, TUint aNumberOfPartitions);
    TInt CancelOp();

private:
    TVersion Version() const;
    TPckgBuf<TFileOpArgs> iPckgBuf;
    };


class CFBFileOpClient : public CBase
    {
public:
    IMPORT_C static CFBFileOpClient* NewL();
    IMPORT_C ~CFBFileOpClient();
    IMPORT_C TInt Copy(const TDesC& aSourceFullName, const TDesC& aTargetFullName, TUint aSwitch);
    IMPORT_C TInt Rename(const TDesC& anOld, const TDesC& aNew, TUint aSwitch); 
    IMPORT_C TInt Attribs(const TDesC& aName, TUint aSetMask, TUint aClearMask, const TTime& aTime, TUint aSwitch); 
    IMPORT_C TInt RmDir(const TDesC& aDirName, TUint aSwitch); 
    IMPORT_C TInt Delete(const TDesC& aName, TUint aSwitch); 
    IMPORT_C TInt MkDirAll(const TDesC& aPath); 
    IMPORT_C TInt CreateEmptyFile(const TDesC& aName); 
    IMPORT_C TInt EraseMBR(TUint aDriveNumber); 
    IMPORT_C TInt PartitionDrive(TUint aDriveNumber, TUint aNumberOfPartitions); 
    IMPORT_C void CancelOp();
    
private:
    CFBFileOpClient();
    void ConstructL();
    
 private :
    RFBFileOpServerSession  iFBFileOpServerSession;
    };


/**
 * Similar class as CAsyncWaiter defined in FBUtils.h, except
 * CAsyncWaiterClient uses iWait.AsyncStop() instead of   
 * CAknEnv::StopSchedulerWaitWithBusyMessage( iWait )
 * to stop the nested loop. This class can be utilized
 * if CAknEnv is not running.
 */
class CAsyncWaiterClient : public CActive
    {
public:
    static CAsyncWaiterClient* NewL( TInt aPriority = EPriorityStandard );
    static CAsyncWaiterClient* NewLC( TInt aPriority = EPriorityStandard );
    ~CAsyncWaiterClient();
    
    void StartAndWait();
    TInt Result() const;
    
private:
    CAsyncWaiterClient( TInt aPriority );
    
    // from CActive
    void RunL();
    void DoCancel();
    
private:
    CActiveSchedulerWait iWait;
    TInt iError;
    };

#endif

