/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Helper class for drive operations/monitoring.*
*/





#ifndef VCXMYVIDEOSDRIVEMONITOR_H
#define VCXMYVIDEOSDRIVEMONITOR_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "vcxmyvideosdriveinfo.h"
#include "vcxmyvideosdrivemonitorevent.h"

// CLASS DECLARATION

/**
* Observer for CVcxMyVideosDriveMonitor class.
*/
NONSHARABLE_CLASS(MVcxMyVideosDriveMonitorObserver)
    {
        
    public:
        /**
        * Handles drive monitor event.
        *
        * @param aEvent Event.
        */
        virtual void HandleDriveMonitorEvent( TVcxMyVideosDriveMonitorEvent& aEvent ) = 0;
    };

/**
 * Helper class for drive operations/monitoring.
 * User can get available drives (through iAvailableDrives public member)
 * and be notified when drives change (through MIptvDriveMonitorObserver interface).
 */
NONSHARABLE_CLASS(CVcxMyVideosDriveMonitor) : public CActive
    {                
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CVcxMyVideosDriveMonitor* NewL();

        /**
        * Two-phased constructor. Use this is owner object already has a file session open.
        *
        * @param aFileSession file session.
        */
        static CVcxMyVideosDriveMonitor* NewL( const RFs& aFileSession );
        
        /**
        * Destructor.
        */
        virtual ~CVcxMyVideosDriveMonitor();
        
    public:

        /**
        * Updates iAvailableDrives member variable from File server.
        * This is called in ConstructL() also.
        */ 
        void RefreshAvailableDrivesL();

        /**
        * Returns the position index of aDrive in iAvailableDrives.
        *
        * @param aDrive drive to search.
        * @return Position index of aDrive in iAvailableDrives. If not found,
        *         KErrNotFound is returned.
        */        
        TInt ArrayPos( TInt aDrive );

        /**
        * Returns the position index of aDrive in iAvailableDrives. If not
        * found, leaves with KErrNotFound code.
        *
        * @param aDrive drive to search.
        * @return Position index of aDrive in iAvailableDrives.
        */        
        TInt ArrayPosL( TInt aDrive );

        /**
        * Returns the first removable drive (= memory card drive) from iAvailableDrives
        * array. Leaves with KErrNotFound if not found.
        */        
        TInt MemoryCardDriveL();

        /**
        * Returns the first removable drive (= memory card drive) from iAvailableDrives
        * array. Returns KErrNotFound if not found.
        *
        * @return drive number, or KErrNotFound if not found.
        */        
        TInt MemoryCardDrive();

        /**
        * Returns the first mass storage drive from iAvailableDrives array.
        * Mass storege drive means the drive which is preferred for storing large
        * files in the device, the drive itself can be anything, usually not c though.
        * Leaves with KErrNotFound if not found.
        *
        * @return Drive number.
        */
        TInt MassStorageDriveL();

        /**
        * Returns the first mass storage drive from iAvailableDrives array.
        * Mass storege drive means the drive which is preferred for storing large
        * files in the device, the drive itself can be anything except c-drive.
        * Returns KErrNotFound if not found.
        *
        * @return Drive number, or KErrNotFound if not found.
        */
        TInt MassStorageDrive();
        
        /**
        * Gets the used memory from cenrep. If cenrep contains invalid drive or value is not found,
        * chooses the preferred drive in the following order: 1. mass storage 2. memory card
        * 3. EDriveC. If cenrep value was not usable, the chosen drive is written to cenrep.
        * Leaves with system-wide error code if wasn't able to get the used memory.
        * The returned memory might be locked or not present. Use GetUsedMemoryL to get drive
        * which is available for usage.
        *
        * @param aPreferredMemory The used memory drive is written here.
        */        
        void GetPreferredMemoryL( TInt& aPreferredMemory );

        /**
        * Gets a memory for usage. Tries first the preferred memory, if it is not
        * available (locked or not present), tries the next drive from iAvailable drives.
        * if suitable drive is not found, returns system drive (c-drive).
        *
        * @param aUsedMemory  Drive which should be used is written here.
        */
        void GetUsedMemoryL( TInt& aUsedMemory );


    protected: // Functions from base classes

        /**
        * From CActive, RunL.
        * Called when operation completes.
        */
        void RunL();

        /**
        * From CActive, cancels request.
        */
        void DoCancel();

        /**
        * From CActive, handles leaves from RunL.
        */
        TInt RunError( TInt aError );
         
    private:

        /**
        * C++ default constructor.
        */
        CVcxMyVideosDriveMonitor();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @param aFileSession
        */
        void ConstructL( const RFs& aFileSession );

        /**
        * Sends event to observers.
        *
        * @param aEvent event to send.
        * @param aDrive drive which caused the event.
        */
        void SendEvent( TVcxMyVideosDriveMonitorEvent::TEvent aEvent, TInt aDrive );

        /**
        * Compares iPreviousAvailableDrives and iAvailableDrives arrays and
        * sends appropriate events if they differ.
        */
        void CompareOldAndNewAvailableDrives();
        
        /**
        * Returns drive type as a descriptor. For debugging only.
        * @param aType Drive type, see valid values from e32const.h TMediaType enum.
        */
        const TDesC& DriveTypeDes( TInt aType );

        /**
        * Prints available drives. For debugging only.
        */
        void PrintAvailableDrives();

    public:
    
        /**
        * Array of currently available drives. These are updated
        * with RefreshAvailableDrivesL -method or from file
        * server event. RefreshAvailableDrivesL is also called upon creation
        * of this object.
        * iAvailableDrives array contains drives which are available for mass
        * storaging videos. If there is hard disk and memory card
        * available(both e and f-drives), then c-drive is excluded from the array.
        * Array contains also drives which don't have media present (mmc), or which
        * are locked.
        */
        RArray<TVcxMyVideosDriveInfo> iAvailableDrives;

        
    private:

        /**
        * Used to detect if refresh had any effect.
        */
        RArray<TVcxMyVideosDriveInfo> iPreviousAvailableDrives;

        /**
        * Array of observers. Pointers not owned.
        */
        RArray<MVcxMyVideosDriveMonitorObserver*> iObservers;
        
        /**
        * Session to file server.
        */
        RFs iFs;
        
        /**
        * ETrue if iFs is provided by the user of this class.
        */
        TBool iIfsIsProvidedByUser;

    };

#endif      // VCXMYVIDEOSDRIVEMONITOR_H   
            
// End of File
