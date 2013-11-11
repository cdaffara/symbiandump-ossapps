/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Monitor class for the drive that contains the message store DB.
*
*/


#ifndef _C_DRIVE_MONITOR_H
#define _C_DRIVE_MONITOR_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>

//CLASS DECLARATION
class MStoreDriveStateObserver
    {
    public:
        /**
         * aState will be true if drive is mounted and false otherwise
         */
        virtual void DriveStateChangedL( TBool aState ) = 0;
    };

/**
 * This active object waits for an indication that the media has changed
 */
NONSHARABLE_CLASS( CStoreDriveMonitor ) : public CActive
	{
public:

    /**
    * Two-phased constructor.
    */
	static CStoreDriveMonitor* NewL( MStoreDriveStateObserver& aDriveStateObserver );

    /**
    * Two-phased constructor.
    */
	static CStoreDriveMonitor* NewLC( MStoreDriveStateObserver& aDriveStateObserver );
	
    /**
    * Destructor
    */
	~CStoreDriveMonitor();
	
    /**
    * Make notification request
    */
    void WaitForChange();
    
    /**
     * Return true if drive is present and false otherwise.
     */
    TBool IsDrivePresent();
    
    /**
     * Set drive to monitor.
     */
    void SetDriveL( TDriveNumber aDrive );

	
private:
	
    /**
    * C++ default constructor
    */
	CStoreDriveMonitor( MStoreDriveStateObserver& aDriveStateObserver );

    /**
    * By default Symbian OS constructor is private.
    */
	void ConstructL();	
	
    /**
    * This method checks if the drive is mounted
    */
	void MediaPresentL();
		
// from CActive

    /**
    * This method is used when cancelling request
    */
	void DoCancel();

    /**
    * This method is used when changes in drive happens
    */
	void RunL();
	
	/**
	 * This is applied when a leave occur in RunL method.
	 */
	TInt RunError(TInt aError);

private:

    /**
     * File server session.
     */
	RFs iFs;
	
	/**
	 * Drive state observer.
	 */
	MStoreDriveStateObserver& iDriveStateObserver;
	
	/**
	 * Drive to monitor.
	 */
	TDriveNumber iDrive;
	
	/**
	 * Drive letter determined from drive number.
	 */
	HBufC* iDriveLetter;
	
	};


#endif      // _C_DRIVE_MONITOR_H

