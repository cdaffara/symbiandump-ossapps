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

#ifndef CMMCMONITOR_H
#define CMMCMONITOR_H

//  INCLUDES 
#include <e32base.h>
#include <e32property.h>
#include <driveinfo.h>
#include <common.h>
#include "fastfindfileserverpluginclient.h"

// FORWARD DECLARATIONS
class CFilePlugin;

/**
 * Mmc monitoring
 * Implements CActive
 */
class CMMCMonitor : public CActive
    {
    public:
        /**
         * NewL
         * @param CFilePlugin& aFilePlugin File plugin handle
         * @param RFs* aFsSession File server handle
         * @return CMMCMonitor Monitor item
         */
        static CMMCMonitor* NewL( CFilePlugin& aFilePlugin, RFs* aFsSession );

        /**
         * Destructor
         */
        virtual ~CMMCMonitor();

        /**
         * StartMonitoring
         * Start monitoring mmc
         * This function calls CMMCMonitor::RunL() to initiate the MMC status 
         * checking.
         * @return TBool success status
         */
        TBool StartMonitoring();

        /**
         * MmcStatus
         * Utility function that checks the DriveInfo properties of drive
         * aDriveNumber to decide if aDriveNumber is an MMC drive or not.
         * @param aDriveNumber, the drive of the Mmc.
         * @return ETrue indicates that aDriveNumber is a MMC drive.
         */
        TBool MmcStatus( TInt aDriveNumber );

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
         * Constructor
         * @param CFilePlugin& aFilePlugin File plugin
         */
        CMMCMonitor( CFilePlugin& aFilePlugin );

        /**
         * 2nd phase construction
         * @param RFs* aFsSession File server session
         */
        void ConstructL( RFs* aFsSession );

    private: // data members
        // handle to file plugin main class
        CFilePlugin& iFilePlugin;
        // file monitor client engine
        RFastFindFSPEngine iEngine;
        // File system handle
        RFs* iFsSession;
        // inserted/ejected status of mmc
        TBool iMmcStatus;
        // MMC Inserted/ejected property
        RProperty iProperty;
        //for unit testing.
        #ifdef HARVESTERPLUGINTESTER_FRIEND
            friend class CHarvesterPluginTester;
        #endif
    };

#endif // CMMCMONITOR_H

