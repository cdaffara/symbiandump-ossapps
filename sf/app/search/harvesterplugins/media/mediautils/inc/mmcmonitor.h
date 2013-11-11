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
* Description:  Mmc monitor
*
*/



#ifndef MMCMONITORUTIL_H
#define MMCMONITORUTIL_H

//  INCLUDES 
#include <e32base.h>
#include <e32property.h>
#include <driveinfo.h>
#include <common.h>
#include <f32file.h>
#include "mmceventobserver.h"

/**
 * Mmc monitoring
 * Implements CActive
 */
class CMMCMonitorUtil : public CActive
    {
public:
    /**
     * NewL
     * @param MMMCEventObserver* aObserver event observer
     * @return CMMCMonitorUtil Monitor item
     */
    static CMMCMonitorUtil* NewL( MMMCEventObserver* aObserver);

    /**
     * Destructor
     */
    virtual ~CMMCMonitorUtil();

    /**
     * StartMonitoring
     * Start monitoring mmc
     * This function calls CMMCMonitorUtil::RunL() to initiate the MMC status 
     * checking.
     * @return TBool success status
     */
    TBool StartMonitoring();
protected: // CActive
    /**
     * From CActive.
     */
    void DoCancel();

    /**
     * From CActive.
     * 
     */
    void RunL();

    /**
     * From CActive.
     * @param aError Leave code from RunL()
     * @return hard-coded KErrNone
     */
    TInt RunError( TInt aError );
private:
    /**
     * MmcStatus
     * Utility function that checks the DriveInfo properties of drive
     * aDriveNumber to decide if aDriveNumber is an MMC drive or not.
     * @param aDriveNumber, the drive of the Mmc.
     * @return ETrue indicates that aDriveNumber is a MMC drive.
     */
    TBool MmcStatus( TInt aDriveNumber );
    
    /**
     * Constructor
     * @param MMMCEventObserver* aObserver event observer
     */
    CMMCMonitorUtil( MMMCEventObserver* aObserver );

    /**
     * 2nd phase construction
     * @param RFs* aFsSession File server session
     */
    void ConstructL();
private: // data members
    // File system handle
    RFs iFsSession;
    // inserted/ejected status of mmc
    TBool iMmcStatus;
    // MMC Inserted/ejected property
    RProperty iProperty;
    //Notify MMC event back to observer
    MMMCEventObserver* iObserver;
    };
#endif // MMCMONITORUTIL_H

