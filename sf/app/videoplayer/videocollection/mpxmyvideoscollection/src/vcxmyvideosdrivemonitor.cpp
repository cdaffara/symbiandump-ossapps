/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/





// INCLUDE FILES
#include <mpxlog.h>
#include <vcxmyvideosdefs.h>
#include "vcxmyvideosdrivemonitor.h"
#include "vcxmyvideosdrivemonitorevent.h"
#include <bldvariant.hrh>
#include <driveinfo.h> //multiple drive stuff
#include <centralrepository.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
#ifdef _DEBUG
_LIT( KVcxMyVideosMediaNotPresentDes, "EMediaNotPresent");
_LIT( KVcxMyVideosMediaUnknownDes, "EMediaUnknown");
_LIT( KVcxMyVideosMediaFloppyDes, "EMediaFloppy");
_LIT( KVcxMyVideosMediaHardDiskDes, "EMediaHardDisk");
_LIT( KVcxMyVideosMediaCdRomDes, "EMediaCdRom");
_LIT( KVcxMyVideosMediaRamDes, "EMediaRam");
_LIT( KVcxMyVideosMediaFlashDes, "EMediaFlash");
_LIT( KVcxMyVideosMediaRomDes, "EMediaRom");
_LIT( KVcxMyVideosMediaRemoteDes, "EMediaRemote");
_LIT( KVcxMyVideosMediaNANDFlashDes, "EMediaNANDFlash");
#endif


// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::CVcxMyVideosDriveMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVcxMyVideosDriveMonitor::CVcxMyVideosDriveMonitor()
: CActive( EPriorityStandard )
    {
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVcxMyVideosDriveMonitor::ConstructL( const RFs& aFileSession )
    {
    iFs                  = aFileSession;
    iIfsIsProvidedByUser = ETrue;
   
    ConstructL();    
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVcxMyVideosDriveMonitor::ConstructL()
    {
    iObservers.Reset();
    iAvailableDrives.Reset();
    iPreviousAvailableDrives.Reset();
    
    if ( !iIfsIsProvidedByUser )
        {
        User::LeaveIfError( iFs.Connect() );
        }
        
    CActiveScheduler::Add( this );
    RefreshAvailableDrivesL();

#if 0
    MPX_DEBUG2("CVcxMyVideosDriveMonitor:: PhoneMemoryDrive() returns %d",
        PhoneMemoryDrive());
#endif
    MPX_DEBUG2("CVcxMyVideosDriveMonitor:: MemoryCardDrive() returns %d",
        MemoryCardDrive());
    MPX_DEBUG2("CVcxMyVideosDriveMonitor:: MassStorageDrive() returns %d",
        MassStorageDrive());
#if 0
    MPX_DEBUG2("CVcxMyVideosDriveMonitor:: FixedDrive(c-drive excluded) returns %d",
        FixedDrive( ETrue ));
#endif
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVcxMyVideosDriveMonitor* CVcxMyVideosDriveMonitor::NewL()
    {
    CVcxMyVideosDriveMonitor* self = new( ELeave ) CVcxMyVideosDriveMonitor();
        
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVcxMyVideosDriveMonitor* CVcxMyVideosDriveMonitor::NewL( const RFs& aFileSession )
    {
    CVcxMyVideosDriveMonitor* self = new( ELeave ) CVcxMyVideosDriveMonitor();
        
    CleanupStack::PushL( self );
    self->ConstructL( aFileSession );
    CleanupStack::Pop(self);

    return self;
    }
    
// Destructor
CVcxMyVideosDriveMonitor::~CVcxMyVideosDriveMonitor()
    {
    iObservers.Close();
    iAvailableDrives.Close();
    iPreviousAvailableDrives.Close();
    Cancel();
    
    if ( !iIfsIsProvidedByUser )
        {
        iFs.Close();
        }
    }

/*
N96 without memory card:
26/09/2007	11:37:22	CIptvDriveMonitor:: drive[2]: type = EMediaNANDFlash
26/09/2007	11:37:22	CIptvDriveMonitor::           MediaAtt flags = 104 DriveAtt flags = 11
26/09/2007	11:37:22	CIptvDriveMonitor:: drive[3]: type = EMediaRam
26/09/2007	11:37:22	CIptvDriveMonitor::           MediaAtt flags = 5 DriveAtt flags = 11
26/09/2007	11:37:22	CIptvDriveMonitor:: drive[4]: type = EMediaHardDisk
26/09/2007	11:37:22	CIptvDriveMonitor::           MediaAtt flags = 14 DriveAtt flags = 21
26/09/2007	11:37:22	CIptvDriveMonitor:: drive[5]: type = EMediaNotPresent
26/09/2007	11:37:22	CIptvDriveMonitor::           MediaAtt flags = 0 DriveAtt flags = 21
26/09/2007	11:37:22	CIptvDriveMonitor:: drive[25]: type = EMediaRom
26/09/2007	11:37:22	CIptvDriveMonitor::           MediaAtt flags = 8 DriveAtt flags = 12

N96 with memory card:
27/09/2007	6:01:07	CIptvDriveMonitor:: drive[2]: type = EMediaNANDFlash
27/09/2007	6:01:07	CIptvDriveMonitor::           MediaAtt flags = 104 DriveAtt flags = 11
27/09/2007	6:01:07	CIptvDriveMonitor:: drive[3]: type = EMediaRam
27/09/2007	6:01:07	CIptvDriveMonitor::           MediaAtt flags = 5 DriveAtt flags = 11
27/09/2007	6:01:07	CIptvDriveMonitor:: drive[4]: type = EMediaHardDisk
27/09/2007	6:01:07	CIptvDriveMonitor::           MediaAtt flags = 14 DriveAtt flags = 21
27/09/2007	6:01:07	CIptvDriveMonitor:: drive[5]: type = EMediaHardDisk
27/09/2007	6:01:07	CIptvDriveMonitor::           MediaAtt flags = 14 DriveAtt flags = 21
27/09/2007	6:01:07	CIptvDriveMonitor:: drive[25]: type = EMediaRom
27/09/2007	6:01:07	CIptvDriveMonitor::           MediaAtt flags = 8 DriveAtt flags = 12

N95:

N95 8GB:

*/

// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::RefreshAvailableDrivesL
// -----------------------------------------------------------------------------
//
void CVcxMyVideosDriveMonitor::RefreshAvailableDrivesL()
    {
    MPX_FUNC("CVcxMyVideosDriveMonitor::RefreshAvailableDrivesL()");
    
    TDriveList driveList;
    User::LeaveIfError( iFs.DriveList( driveList ) );

    iPreviousAvailableDrives.Reset();
    TInt i;
    for ( i = 0; i < iAvailableDrives.Count(); i++ )
        {
        iPreviousAvailableDrives.Append( iAvailableDrives[i] );
        }
            
    iAvailableDrives.Reset();
    
    for ( i = 0; i < driveList.Length(); i++ )
        {
        if ( driveList[i] != 0 && i != EDriveY ) // N95 8GB has a 1MB small flash drive at y-drive, rule it out
            {
            TDriveInfo driveInfo;
            User::LeaveIfError( iFs.Drive( driveInfo, i ) );
            
            /*
            From e32cost.h:
            const TUint KMediaAttVariableSize   = 0x01;
            const TUint KMediaAttDualDensity    = 0x02;
            const TUint KMediaAttFormattable    = 0x04;
            const TUint KMediaAttWriteProtected = 0x08;
            const TUint KMediaAttLockable       = 0x10;
            const TUint KMediaAttLocked         = 0x20;
            const TUint KMediaAttHasPassword    = 0x40;
            const TUint KMediaAttReadWhileWrite = 0x80;
            const TUint KMediaAttDeleteNotify   = 0x100;
            
            The drive attributes.    
            
            const TUint KDriveAttLocal          = 0x01;
            const TUint KDriveAttRom            = 0x02;
            const TUint KDriveAttRedirected     = 0x04;
            const TUint KDriveAttSubsted        = 0x08;
            const TUint KDriveAttInternal       = 0x10;
            const TUint KDriveAttRemovable      = 0x20;
            const TUint KDriveAttRemote         = 0x40;
            const TUint KDriveAttTransaction    = 0x80;
            */

            MPX_DEBUG3("CVcxMyVideosDriveMonitor:: drive[%d]: type = %S",
                    i, &DriveTypeDes( driveInfo.iType ));

            MPX_DEBUG3(
                "CVcxMyVideosDriveMonitor::           MediaAtt flags = %x DriveAtt flags = %x",
                    driveInfo.iMediaAtt,
                    driveInfo.iDriveAtt );

#ifdef __WINSCW__
            // (2) c-drive is EMediaRam
            // (3) d-drive is EMediaHardDisk
            // (4) e-drive is EMediaHardDisk or EMediaNotPresent
            // (25) z-drive is EMediaRom
            if ( (driveInfo.iType == EMediaRam) ||                                     // c
                 ((driveInfo.iType == EMediaHardDisk) && i != EDriveD /* skip d */) || // e
                 (driveInfo.iType == EMediaNANDFlash) ||                               // nothing
                 (driveInfo.iType == EMediaNotPresent) )                               // e
#else
            // (2) c-drive is EMediaNANDFlash
            // (3) d-drive is EMediaRam
            // (4) e-drive is EMediaHardDisk or EMediaNotPresent
            // (5) f-drive is EMediaHardDisk or EMediaNotPresent
            // (25) z-drive is EMediaRom
            if ( (driveInfo.iType == EMediaHardDisk) ||  // e,f
                 (driveInfo.iType == EMediaNANDFlash) || // c
                 (driveInfo.iType == EMediaNotPresent) ) // e,f
#endif // __WINSCW__
                {                
                if ( !(driveInfo.iMediaAtt & KMediaAttWriteProtected) &&
                      (driveInfo.iDriveAtt & KDriveAttLocal) )
                    {
                    TVcxMyVideosDriveInfo iptvDriveInfo;
                    TVolumeInfo volumeInfo;
                    TChar ch;

                    iFs.DriveToChar( i, ch );

                    iptvDriveInfo.iDrive     = i;
                    iptvDriveInfo.iDriveChar = ch;
                    iptvDriveInfo.iType      = driveInfo.iType;

                    TUint driveStatus;
                    TInt err = DriveInfo::GetDriveStatus( iFs, i, driveStatus );
                    if ( err == KErrNone )
                        {
                        if ( driveStatus & DriveInfo::EDriveRemovable )
                            {
                            iptvDriveInfo.iFlags |= TVcxMyVideosDriveInfo::ERemovable;
                            }
                        }

                    TInt massStorageDrive;
                    err = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage,
                                                      massStorageDrive );
                    if ( err == KErrNone )
                        {
                        if ( i == massStorageDrive )
                            {
                            iptvDriveInfo.iFlags |= TVcxMyVideosDriveInfo::EMassStorage;
                            }                        
                        }

                    if ( driveInfo.iMediaAtt & KMediaAttLocked )
                        {
                        iptvDriveInfo.iFlags |= TVcxMyVideosDriveInfo::ELocked;
                        }
                    if ( driveInfo.iType == EMediaNotPresent )
                        {
                        iptvDriveInfo.iFlags |= TVcxMyVideosDriveInfo::EMediaNotPresent;
                        }

                    // When mass storage mode is activated while application is running,
                    // the only hint about that is the TVolumeInfo not being available. 
                    // Therefore we mark drive as EMediaNotPresent if that structure
                    // is not available.
                    if ( iFs.Volume( volumeInfo, i ) != KErrNone )
                        {
                        iptvDriveInfo.iFlags |= TVcxMyVideosDriveInfo::EMediaNotPresent;
                        iptvDriveInfo.iVolumeName.Zero();
                        }
                    else
                        {
                        iptvDriveInfo.iVolumeName = volumeInfo.iName;
                        }

                    if ( iptvDriveInfo.iFlags & TVcxMyVideosDriveInfo::EMediaNotPresent )
                        {
                        iptvDriveInfo.iFlags |= TVcxMyVideosDriveInfo::ERemovable;
                        }

                    iAvailableDrives.AppendL( iptvDriveInfo );
                    }
                }
            }
        } // for

    // if e and f-drives are available, then c-drive is removed from the available drives list
    if ( ArrayPos( EDriveE ) != KErrNotFound &&
         ArrayPos( EDriveF ) != KErrNotFound )
        {
        TInt cDrivePos = ArrayPos( EDriveC );
        if ( cDrivePos != KErrNotFound )
            {
            MPX_DEBUG1(
                "CIptvDriveMonitor:: e & f-drives available -> removing c from available drives");
            iAvailableDrives.Remove( cDrivePos );
            iAvailableDrives.Compress();
            }
        }    
    }
    
// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::ArrayPos
// -----------------------------------------------------------------------------
//
TInt CVcxMyVideosDriveMonitor::ArrayPos( TInt aDrive )
    {
    TInt i;
    for (i = 0; i < iAvailableDrives.Count(); i++ )
        {
        if ( iAvailableDrives[i].iDrive == aDrive )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::ArrayPosL
// -----------------------------------------------------------------------------
//
TInt CVcxMyVideosDriveMonitor::ArrayPosL( TInt aDrive )
    {
    TInt pos;
    User::LeaveIfError( pos = ArrayPos( aDrive ) );
    return pos;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::MemoryCardDriveL
// -----------------------------------------------------------------------------
//
TInt CVcxMyVideosDriveMonitor::MemoryCardDriveL()
    {
    TInt drive;
    User::LeaveIfError( drive = MemoryCardDrive() );
    return drive;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::MemoryCardDrive
// -----------------------------------------------------------------------------
//
TInt CVcxMyVideosDriveMonitor::MemoryCardDrive()
    {
    TInt i;
    for ( i = 0; i < iAvailableDrives.Count(); i++ )
        {
        if ( iAvailableDrives[i].iFlags & TVcxMyVideosDriveInfo::ERemovable )
            {
            return iAvailableDrives[i].iDrive;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::MassStorageDriveL
// -----------------------------------------------------------------------------
//
TInt CVcxMyVideosDriveMonitor::MassStorageDriveL()
    {
    TInt drive;
    User::LeaveIfError( drive = MassStorageDrive() );
    return drive;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::MassStorageDrive
// -----------------------------------------------------------------------------
//
TInt CVcxMyVideosDriveMonitor::MassStorageDrive()
    {
    TInt i;
    for ( i = 0; i < iAvailableDrives.Count(); i++ )
        {
        if ( iAvailableDrives[i].iFlags & TVcxMyVideosDriveInfo::EMassStorage )
            {
            return iAvailableDrives[i].iDrive;
            }
        }
    return KErrNotFound;
    }
    
// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::RunL()
// -----------------------------------------------------------------------------
//
void CVcxMyVideosDriveMonitor::RunL()
    {
    MPX_FUNC("CVcxMyVideosDriveMonitor::RunL()");
    
    if ( iStatus == KErrNone )
        {        
        iFs.NotifyChange( ENotifyDisk, iStatus );
        
        TRAPD( err, RefreshAvailableDrivesL() );
        if ( err != KErrNone )
            {
            MPX_DEBUG2("CVcxMyVideosDriveMonitor:: RefreshAvailableDrivesL leaved with error code %d, ignoring", err);
            } 
        CompareOldAndNewAvailableDrives();
        SetActive();
        }
    else
        {
        iFs.NotifyChange( ENotifyDisk, iStatus );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::DoCancel()
// -----------------------------------------------------------------------------
//
void CVcxMyVideosDriveMonitor::DoCancel()
    {
    iFs.NotifyChangeCancel( iStatus );
    }       

// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::RunError()
// -----------------------------------------------------------------------------
//
TInt CVcxMyVideosDriveMonitor::RunError( TInt /* aError */ )
    {
    MPX_DEBUG1("CVcxMyVideosDriveMonitor::RunL() leaved (should never happen).");
    return KErrNone;
    }       

// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::SendEvent()
// -----------------------------------------------------------------------------
//
void CVcxMyVideosDriveMonitor::SendEvent( TVcxMyVideosDriveMonitorEvent::TEvent aEvent,
    TInt aDrive )
    {
    MPX_DEBUG4("CVcxMyVideosDriveMonitor:: Sending event: %S(%d), drive: %d",
            &TVcxMyVideosDriveMonitorEvent::EventDes( aEvent ),
            aEvent,
            aDrive );

    TVcxMyVideosDriveMonitorEvent event;
        
    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        event.iEvent = static_cast<TInt>(aEvent);
        event.iDrive = aDrive;
        iObservers[i]->HandleDriveMonitorEvent( event );
        }

#ifdef _DEBUG
    PrintAvailableDrives();
#endif
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::CompareOldAndNewAvailableDrives
// -----------------------------------------------------------------------------
//
void CVcxMyVideosDriveMonitor::CompareOldAndNewAvailableDrives()
    {
    // Run through old drives list to detect changes
    TInt i;
    for ( i = 0; i < iPreviousAvailableDrives.Count(); i++ )
        {
        TInt pos = ArrayPos( iPreviousAvailableDrives[i].iDrive );
        if ( pos == KErrNotFound )
            {
            // drive was dismounted
            SendEvent( TVcxMyVideosDriveMonitorEvent::EDriveDismounted,
                iPreviousAvailableDrives[i].iDrive );
            }
        else 
            {
            // drive still present
            if ( iPreviousAvailableDrives[i].iFlags != iAvailableDrives[pos].iFlags )
                {
                // drive was changed
                if ( (iPreviousAvailableDrives[i].iFlags & TVcxMyVideosDriveInfo::EMediaNotPresent) && 
                     !(iAvailableDrives[pos].iFlags & EMediaNotPresent) )
                    {
                    SendEvent( TVcxMyVideosDriveMonitorEvent::EMediaInserted,
                        iAvailableDrives[pos].iDrive );
                    }
                    
                if ( !(iPreviousAvailableDrives[i].iFlags & TVcxMyVideosDriveInfo::EMediaNotPresent) && 
                     (iAvailableDrives[pos].iFlags & TVcxMyVideosDriveInfo::EMediaNotPresent) )
                    {
                    SendEvent( TVcxMyVideosDriveMonitorEvent::EMediaRemoved,
                        iAvailableDrives[pos].iDrive );
                    }

                if ( !(iPreviousAvailableDrives[i].iFlags & TVcxMyVideosDriveInfo::ELocked) && 
                     (iAvailableDrives[pos].iFlags & TVcxMyVideosDriveInfo::ELocked) )
                    {
                    SendEvent( TVcxMyVideosDriveMonitorEvent::EDriveLocked,
                        iAvailableDrives[pos].iDrive );
                    }

                if ( (iPreviousAvailableDrives[i].iFlags & TVcxMyVideosDriveInfo::ELocked) && 
                     !(iAvailableDrives[pos].iFlags & TVcxMyVideosDriveInfo::ELocked) )
                    {
                    SendEvent( TVcxMyVideosDriveMonitorEvent::EDriveUnlocked,
                        iAvailableDrives[pos].iDrive );
                    }
                    
                }
            // drive volume name was changed
            if ( iPreviousAvailableDrives[i].iVolumeName.Compare( 
                 iAvailableDrives[pos].iVolumeName ) != 0 )
                {
                SendEvent( TVcxMyVideosDriveMonitorEvent::EDriveVolumeNameChanged, 
                    iAvailableDrives[pos].iDrive ); 
                }
            }
        }

    // Run through new drives list to detect mounted drives
    for ( i = 0; i < iAvailableDrives.Count(); i++ )
        {
        TBool found = EFalse;
        for ( TInt j = 0; j < iPreviousAvailableDrives.Count(); j++ )
            {
            if ( iPreviousAvailableDrives[j].iDrive == iAvailableDrives[i].iDrive )
                {
                found = ETrue;
                break;
                }
            }
        if ( !found )
            {
            SendEvent( TVcxMyVideosDriveMonitorEvent::EDriveMounted,
                iAvailableDrives[i].iDrive );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::DriveType()
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG 
const TDesC& CVcxMyVideosDriveMonitor::DriveTypeDes( TInt aType )
    {
    switch (aType)
        {     
        case EMediaNotPresent:
            return KVcxMyVideosMediaNotPresentDes;
	      case EMediaUnknown:
            return KVcxMyVideosMediaUnknownDes;
        case EMediaFloppy:
            return KVcxMyVideosMediaFloppyDes;
        case EMediaHardDisk:
            return KVcxMyVideosMediaHardDiskDes;
        case EMediaCdRom:
            return KVcxMyVideosMediaCdRomDes;
        case EMediaRam:
            return KVcxMyVideosMediaRamDes;
        case EMediaFlash:
            return KVcxMyVideosMediaFlashDes;
        case EMediaRom:
            return KVcxMyVideosMediaRomDes;
        case EMediaRemote:
            return KVcxMyVideosMediaRemoteDes;
        case EMediaNANDFlash:
            return KVcxMyVideosMediaNANDFlashDes;
        default:
            return KVcxMyVideosMediaUnknownDes;
        }
    }
#else
const TDesC& CVcxMyVideosDriveMonitor::DriveTypeDes( TInt /*aType*/ )
    {
    return KNullDesC;
    }
#endif
    
// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::PrintAvailableDrives()
// -----------------------------------------------------------------------------
//
void CVcxMyVideosDriveMonitor::PrintAvailableDrives()
    {
#ifdef _DEBUG 
    for ( TInt i = 0; i < iAvailableDrives.Count(); i++ )
        {
        MPX_DEBUG3("CVcxMyVideosDriveMonitor:: iAvailableDrives[%d].iDrive = %d",
            i, iAvailableDrives[i].iDrive);
        MPX_DEBUG2("    ERemovableDrive  = %d",
            !!(iAvailableDrives[i].iFlags & TVcxMyVideosDriveInfo::ERemovable));
        MPX_DEBUG2("    EMassStorage     = %d",
            !!(iAvailableDrives[i].iFlags & TVcxMyVideosDriveInfo::EMassStorage));
        MPX_DEBUG2("    ELockedDrive     = %d",
            !!(iAvailableDrives[i].iFlags & TVcxMyVideosDriveInfo::ELocked));
        MPX_DEBUG2("    EMediaNotPresent = %d",
            !!(iAvailableDrives[i].iFlags & TVcxMyVideosDriveInfo::EMediaNotPresent));
        }
#endif
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::GetPreferredMemoryL
// Gets the preferred memory, this doesn't mean yet that it can be actually used.
// Media might not be present or it might be locked.
// -----------------------------------------------------------------------------
//
void CVcxMyVideosDriveMonitor::GetPreferredMemoryL( TInt& aPreferredMemory )
    {
    MPX_FUNC("CVcxMyVideosDriveMonitor::GetPreferredMemoryL()");
    
    TInt cenRepMemory = KErrNotFound; //init to some non valid drive value, in case that cenrep wont change it at all
    
    TUid uid;
    uid.iUid = KVcxMyVideosCenRepUid;
    CRepository* cenRep = CRepository::NewL( uid );
    CleanupStack::PushL( cenRep ); // 1->

    TInt cenRepError = cenRep->Get( KVcxMyVideosCenRepPreferredMemoryKey, cenRepMemory );
                
    aPreferredMemory = EDriveC;
        
    if ( ArrayPos( cenRepMemory ) == KErrNotFound
        || cenRepError != KErrNone )
        {
        MPX_DEBUG1(
            "CVcxMyVideosDriveMonitor:: cenrep value for used memory was not valid or not found.");

        // Non existing drive or no cen rep value -> figure out the default drive
        TInt massStorageDrive = MassStorageDrive();
        if ( massStorageDrive != KErrNotFound )
            {
            MPX_DEBUG1("CVcxMyVideosDriveMonitor:: using mass storage");
            aPreferredMemory = massStorageDrive;
            }
        else
            {    
            TInt memoryCardDrive  = MemoryCardDrive();
            if ( memoryCardDrive != KErrNotFound )
                {
                MPX_DEBUG1("CVcxMyVideosDriveMonitor:: using memory card");
                aPreferredMemory = memoryCardDrive;
                }
            }

        MPX_DEBUG2(
            "CVcxMyVideosDriveMonitor:: writing new value to cenrep (%d)", aPreferredMemory);

        cenRep->Set( KVcxMyVideosCenRepPreferredMemoryKey, aPreferredMemory );
        }
    else
        {
        //Cenrep value was valid, use it
        MPX_DEBUG1(
            "CVcxMyVideosDriveMonitor:: cenrep value for used memory was valid, using it.");
        aPreferredMemory = cenRepMemory;
        }

    CleanupStack::PopAndDestroy( cenRep ); // <-1
            
    MPX_DEBUG2("CVcxMyVideosDriveMonitor:: preferred memory: %d", aPreferredMemory);
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosDriveMonitor::GetUsedMemoryL
// Gets the memory for usage. Tries first the preferred memory, if it is not
// available (locked or not present), tries the next drive from iAvailable drives.
// If suitable drive is not found, returns system drive (c-drive).
// -----------------------------------------------------------------------------
//
void CVcxMyVideosDriveMonitor::GetUsedMemoryL( TInt& aUsedMemory )
    {
    TInt preferredDrive;
    GetPreferredMemoryL( preferredDrive );
    
    TInt startPos   = ArrayPos( preferredDrive );
    TInt currentPos = startPos;
    TUint32 flags;
    TBool found = EFalse;
    while ( !found )
        {
        flags = iAvailableDrives[currentPos].iFlags;
    
        if ( (flags & TVcxMyVideosDriveInfo::ELocked) ||
            (flags & TVcxMyVideosDriveInfo::EMediaNotPresent) )
            {
            MPX_DEBUG1("CVcxMyVideosDownloadUtil:: drive was locked or not present -> trying next available drive");
            currentPos++;
            if ( currentPos > (iAvailableDrives.Count()-1) )
                {
                currentPos = 0;
                }
            if ( currentPos == startPos )
                {
                // we went full round and couldnt find drive -> use c-drive
                MPX_DEBUG1("CVcxMyVideosDownloadUtil:: no suitable drive found from iAvailable drives, using c-drive");
                aUsedMemory = iFs.GetSystemDrive();
                found       = ETrue;
                }
            }
        else
            {
            aUsedMemory = iAvailableDrives[currentPos].iDrive;
            found       = ETrue;
            }
        }
    MPX_DEBUG2("CVcxMyVideosDownloadUtil::GetUsedMemoryL() returns %d", aUsedMemory);
    }
        
// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File  
