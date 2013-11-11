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


#include <e32test.h>
#include <e32std.h>
#include <f32fsys.h>
#include <e32property.h>

#include "FBDrivePartitioner.h"


// copied from coreapplicationuisdomainpskeys.h
const TUid KPSUidCoreApplicationUIs = { 0x101F8767 };
const TUint32 KCoreAppUIsMmcRemovedWithoutEject = 0x00000109;
enum TCoreAppUIsMmcRemovedWithoutEject
    {
    ECoreAppUIsEjectCommandUsedUninitialized = 0,
    ECoreAppUIsEjectCommandNotUsed,
    ECoreAppUIsEjectCommandUsed,
    ECoreAppUIsEjectCommandUsedToDrive // Low 16-bits contain this value and high 16-bits TDriveNumber to eject
    };


// trace macros
#define TRACE(a) {_LIT( KMsg, a ); RDebug::Print(KMsg);}
#define TRACE1(a,b) {_LIT( KMsg, a ); RDebug::Print(KMsg,b);}
#define TRACE2(a,b,c) {_LIT( KMsg, a ); RDebug::Print(KMsg,b,c);}

	
// Set the partition alignment boundary
const TInt KPartitionAlignment = 0x1000; // 4kB
const TInt KMBRSize = 512;
const TInt KSectorSize = 512;
const TInt KMBRPartitionInfoOffset = 0x1BE;
const TInt KLargeFAT16Partition = 0x2000000; // 32MB
const TInt KPartitionEntrySize = 16;

// Set for the iX86BootIndicator
//const TInt KPartitionBootable = 0x80;
const TInt KPartitionNonBootable = 0x0;


class TMBRPartition
	{
public:
	TUint8 iX86BootIndicator;
	TUint8 iStartHead;
	TUint8 iStartSector;
	TUint8 iStartCylinder;
	TUint8 iPartitionType;
	TUint8 iEndHead;
	TUint8 iEndSector;
	TUint8 iEndCylinder;
	TUint32 iFirstSector;
	TUint32 iNumSectors;
	};


// --------------------------------------------------------------------------------------------

TInt FBDrivePartioner::FindLocalDriveNumber(RFs& aFs, TInt aDriveNumber, TInt& aLocalDriveNum)
    {
    TRACE1( "FBDrivePartitioner::FindLocalDriveNumber: start aDriveNumber=%d", aDriveNumber );

    TInt err(KErrNone);
    
    TDriveInfo driveInfo;
    err = aFs.Drive(driveInfo, aDriveNumber);

    TRACE1( "FBDrivePartitioner::FindLocalDriveNumber: get DriveInfo err=%d", err );


    TVolumeInfo vi;
    err = aFs.Volume(vi, aDriveNumber);

    TRACE1( "FBDrivePartitioner::FindLocalDriveNumber: get VolumeInfo err=%d", err );


    TMediaSerialNumber serialNum;
    err = aFs.GetMediaSerialNumber(serialNum, aDriveNumber);

    TRACE1( "FBDrivePartitioner::FindLocalDriveNumber: get MediaSerialNumber err=%d", err );


    TInt len = serialNum.Length();
    TInt n(0);
    for (n=0; n<len; n+=16)
        {
        TBuf16<16*3 +1> buf;
        for (TInt m=n; m<n+16; m++)
            {
            TBuf16<3> hexBuf;
            hexBuf.Format(_L("%02X "),serialNum[m]);
            buf.Append(hexBuf);
            }
        buf.Append(_L("\n"));
        }

    TBusLocalDrive drv;
    TBool chg(EFalse);
    aLocalDriveNum = -1;
    for (n=0; n<KMaxLocalDrives; n++)
        {
        TRACE1( "FBDrivePartitioner::FindLocalDriveNumber: drv.Connect drive %d", n );
        err = drv.Connect(n, chg); //for user area
        TRACE1( "FBDrivePartitioner::FindLocalDriveNumber: drv.Connect returns %d", err );

        if(err != KErrNone)
            {
            continue;
            }    

        TLocalDriveCapsV5Buf capsBuf;
        TLocalDriveCapsV5& caps = capsBuf();
        err = drv.Caps(capsBuf);
        TRACE1( "FBDrivePartitioner::FindLocalDriveNumber: drv.Caps returns %d", err );

        if(err != KErrNone)
            {
            continue;
            }    

        TPtrC8 localSerialNum(caps.iSerialNum, caps.iSerialNumLength);
        if (serialNum.Compare(localSerialNum) == 0)
            {
            TBool sizeMatch = (vi.iSize < caps.iSize)?ETrue:EFalse;
            if (sizeMatch)
                {
                aLocalDriveNum = n;
                TRACE1( "FBDrivePartitioner::FindLocalDriveNumber: sizeMatch for localDrive %d", n );
                }
            else
                {
                TRACE1( "FBDrivePartitioner::FindLocalDriveNumber: sizeMatch failed for localDrive %d", n );
                }

            }

        drv.Disconnect();
        }

    TRACE1( "FBDrivePartitioner::FindLocalDriveNumber: localDriveNum=%d", aLocalDriveNum );

    err = aLocalDriveNum == -1 ? KErrNotFound : KErrNone;
    
    TRACE1( "FBDrivePartitioner::FindLocalDriveNumber: returns %d", err );
    return err;    

    }
	
// --------------------------------------------------------------------------------------------

TInt FBDrivePartioner::EraseMBR(RFs& aFs, TInt aDriveNumber)
	{
    TRACE1( "FBDrivePartitioner::EraseMBR: start aDriveNumber=%d", aDriveNumber );

    // Prevent SysAp shutting down applications
    RProperty::Set(
        KPSUidCoreApplicationUIs,
        KCoreAppUIsMmcRemovedWithoutEject,
        ECoreAppUIsEjectCommandUsed );

	TInt err(KErrNone);

	TInt localDriveNumber;
	err = FindLocalDriveNumber(aFs, aDriveNumber, localDriveNumber);
	
	if (err != KErrNone)
	    {
    	TRACE1("FBDrivePartitioner::EraseMBR: FindLocalDriveNumber err=%d returning", err); 
	    return err;
	    }
	
	TRACE1("FBDrivePartitioner::EraseMBR: connecting to local drive %d", localDriveNumber); 

	TBusLocalDrive drv;
	TBool chg(EFalse);
	err = drv.Connect(localDriveNumber, chg); //for user area

	TRACE1("FBDrivePartitioner::EraseMBR: drv.Connect %d", err);

	
	TRACE("FBDrivePartitioner::EraseMBR: Now start to invalidate MBR"); 

	// create a buffers of zeros
	TBuf8<KMBRSize> mbrBuf;
	mbrBuf.Fill(0, mbrBuf.MaxLength());

/*	
	// write the next 16 sectors with the buffer. Usually you only write the first one, but this one should erase any following invalid sectors
	for (TInt i=0; i<16; i++)
	    {
    	err = drv.Write(i*KMBRSize, KMBRSize, &mbrBuf, -1 , 0, 0x40000000);  // RLocalDrive::ELocDrvWholeMedia==0x40000000
       	TRACE2("FBDrivePartitioner::EraseMBR: sector %d, write returns %d", i+1, err); 
	    }
*/

    // erase the MBR
	err = drv.Write(0, KMBRSize, &mbrBuf, -1 , 0, 0x40000000);  // RLocalDrive::ELocDrvWholeMedia==0x40000000
   	TRACE1("FBDrivePartitioner::EraseMBR: write returns %d", err); 

	drv.Disconnect();

	// force a remount, if this is the protected area, then it will have to be remounted
    TRACE1( "FBDrivePartitioner::EraseMBR: Remounting drive number %d", aDriveNumber );
	TInt err2 = aFs.RemountDrive(aDriveNumber, NULL, 0);
    TRACE1( "FBDrivePartitioner::EraseMBR: RemountDrive err2=%d", err2 );

    TRACE1( "FBDrivePartitioner::EraseMBR: returns %d", err );
    return err;
    }

// --------------------------------------------------------------------------------------------

TInt FBDrivePartioner::CreatePartitions(RFs& aFs, TInt aDriveNumber, TInt aPartCount)
	{
    TRACE2( "FBDrivePartitioner::CreatePartitions: start aDriveNumber=%d aPartCount=%d", aDriveNumber, aPartCount );
	TInt err(KErrNone);

    TUint8 MBRPartitionInfo[ KPartitionEntrySize*KMBRMaxPrimaryPartitions ];

    // Prevent SysAp shutting down applications
    RProperty::Set(
        KPSUidCoreApplicationUIs,
        KCoreAppUIsMmcRemovedWithoutEject,
        ECoreAppUIsEjectCommandUsed );

	// Force the system to read the memory card erased MBR
	err = aFs.RemountDrive( aDriveNumber );
	TRACE1( "FBDrivePartitioner::CreatePartitions: Remount result %d", err );
	
	// Get the number of partitions from user
    TUint8 partitionCount(aPartCount);
	TBool change = EFalse;
	TInt mcDriveNbr(2);
	TLocalDriveCapsV4 dc;
	TInt64 diskSpace(0);

/*
	err = FindLocalDriveNumber(aFs, aDriveNumber, mcDriveNbr);
	
	if (err != KErrNone)
	    {
    	TRACE1("FBDrivePartitioner::CreatePartitions: FindLocalDriveNumber err=%d returning", err); 
	    return err;
	    }
*/

	// Seek through the system drives to find memory card drive
	for (TInt i = 0; i < KMaxLocalDrives; ++i)
		{
		RLocalDrive	d;
		change = EFalse;
		err = d.Connect(i, change);
		if (err == KErrNone)
			{
			TPckg<TLocalDriveCapsV4> capsPack(dc);
			
			if (d.Caps(capsPack) == KErrNone)
				{
				if (dc.iType == EMediaHardDisk)
					{
					mcDriveNbr = i;
					diskSpace  = dc.iSize;
                 	TRACE1( "FBDrivePartitioner::CreatePartitions: found hard drive %d:", mcDriveNbr );
					}
				}
			d.Close();
			}
		}

	// Connect to the local drive we found
	RLocalDrive	localDrive;
	change = EFalse;
    err = localDrive.Connect( mcDriveNbr, change );
 	TRACE2( "FBDrivePartitioner::CreatePartitions: localDrive.Connect %d: %d", mcDriveNbr, err );
	TRACE1( "FBDrivePartitioner::CreatePartitions: diskSpace in sectors 0x%x, %d", diskSpace / KSectorSize );

	// Let's read the MBR by using RLocalDrive
	TUint8 data[KMBRSize];
	TPtr8 buf( &data[0], KMBRSize );
	err = localDrive.Read( 0, KMBRSize, buf );
	TRACE1( "FBDrivePartitioner::CreatePartitions: localDrive.Read %d", err );
	// Let's check the current drive layout here
	memcpy( MBRPartitionInfo, &data[KMBRPartitionInfoOffset],(sizeof(TMBRPartition)<<2)); 
	TMBRPartition *partitionInfo = (TMBRPartition*) MBRPartitionInfo;

	// Clean the rest of the MBR buffer
	buf.FillZ();

	// Print out the partition info
	for( TInt i = 0; i<KMBRMaxPrimaryPartitions; i++ )
		{
		TRACE1( "FBDrivePartitioner::CreatePartitions: Partition %d", i+1 );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iX86BootIndicator %x", partitionInfo[i].iX86BootIndicator );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iStartHead        %x", partitionInfo[i].iStartHead );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iStartSector      %x", partitionInfo[i].iStartSector );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iStartCylinder    %x", partitionInfo[i].iStartCylinder );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iPartitionType    %x", partitionInfo[i].iPartitionType );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iEndHead          %x", partitionInfo[i].iEndHead );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iEndSector        %x", partitionInfo[i].iEndSector );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iEndCylinder      %x", partitionInfo[i].iEndCylinder );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iFirstSector      %x", partitionInfo[i].iFirstSector );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iNumSectors       %x", partitionInfo[i].iNumSectors );
		}

	// Calculate partition size	
	TRACE( "FBDrivePartitioner::CreatePartitions: Calculating partition size" );
	TInt64 partitionSize = (diskSpace / partitionCount) - KPartitionAlignment;
	//TRACE1( "FBDrivePartitioner::CreatePartitions: PartitionSize in sectors 0x%x", partitionSize/KSectorSize );
	TRACE1( "FBDrivePartitioner::CreatePartitions: Size of the partition is %Ld bytes", partitionSize );

	// Activate the specified amount of partitions 
	for( TInt i = 0; i < KMBRMaxPrimaryPartitions; i++ )
		{
		if( i < partitionCount )
			{
			if( partitionSize <= KLargeFAT16Partition )
				{
				partitionInfo[i].iPartitionType = KPartitionTypeFAT16small;
				}
			else
				{
				partitionInfo[i].iPartitionType = KPartitionTypeFAT16;
				}
			partitionInfo[i].iX86BootIndicator = KPartitionNonBootable;
			}
		else
			{
			partitionInfo[i].iX86BootIndicator = 0;
			partitionInfo[i].iPartitionType = 0;
			}
		}

	TRACE( "FBDrivePartitioner::CreatePartitions: Set first partition start address and change to sector addresses" );

	// Set first partition start address
	TInt64 offSet = KPartitionAlignment;
	// Change to sector addresses
	offSet = offSet / KSectorSize;

	// Align and set the partitions
	for( TInt i = 0; i < KMBRMaxPrimaryPartitions; i++ )
		{
		if( i < partitionCount )
			{
			// Start sector
			partitionInfo[i].iFirstSector = offSet;
			// Number of sectors in a partition
			partitionInfo[i].iNumSectors = partitionSize / KSectorSize;
			// Start of the next partition
			offSet += ((partitionSize &~ (KPartitionAlignment - 1)) / KSectorSize);
			}
		else
			{
			partitionInfo[i].iFirstSector = 0;
			partitionInfo[i].iNumSectors = 0;
			}
		// Start address HDD
		partitionInfo[i].iStartHead = 0;
		partitionInfo[i].iStartSector = 0;
		partitionInfo[i].iStartCylinder = 0;
		// End address HDD
		partitionInfo[i].iEndHead = 0;
		partitionInfo[i].iEndSector = 0;
		partitionInfo[i].iEndCylinder = 0;

		}

	// Print out the new partition boot record
 	TRACE( "FBDrivePartitioner::CreatePartitions: Partitions created" );
	for( TInt i = 0; i<KMBRMaxPrimaryPartitions; i++ )
		{
		TRACE1("FBDrivePartitioner::CreatePartitions: Partition %d", i+1 );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iX86BootIndicator %x", partitionInfo[i].iX86BootIndicator );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iStartHead        %x", partitionInfo[i].iStartHead );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iStartSector      %x", partitionInfo[i].iStartSector );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iStartCylinder    %x", partitionInfo[i].iStartCylinder );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iPartitionType    %x", partitionInfo[i].iPartitionType );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iEndHead          %x", partitionInfo[i].iEndHead );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iEndSector        %x", partitionInfo[i].iEndSector );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iEndCylinder      %x", partitionInfo[i].iEndCylinder );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iFirstSector      %x", partitionInfo[i].iFirstSector );
		TRACE1( "FBDrivePartitioner::CreatePartitions: iNumSectors       %x", partitionInfo[i].iNumSectors );
		}

	// Copy the new layout to MBR buffer
 	TRACE( "FBDrivePartitioner::CreatePartitions: Copy the new layout to MBR buffer" );
	memcpy( &data[KMBRPartitionInfoOffset], MBRPartitionInfo, (sizeof(TMBRPartition)<<2) ); 

	// Make sure we have a valid signature
 	TRACE( "FBDrivePartitioner::CreatePartitions: Make sure we have a valid signature" );
	*(TUint16*)(&data[KMBRSignatureOffset]) = 0xAA55;

	// Now flush the new partition layout
 	TRACE( "FBDrivePartitioner::CreatePartitions: Flush the new partition layout" );
	err = localDrive.Write( 0, buf );
	TRACE1( "FBDrivePartitioner::CreatePartitions: localDrive.Write err=%d", err );
	localDrive.Close();

    TRACE1( "FBDrivePartitioner::CreatePartitions: returns %d", err );
    
    return err;
    }

// --------------------------------------------------------------------------------------------

TInt FBDrivePartioner::FormatPartitions(RFs& aFs, TInt aDriveNumber, TInt aPartCount)
	{
    TRACE2( "FBDrivePartitioner::FormatPartitions: aDriveNumber=%d aPartCount=%d", aDriveNumber, aPartCount );
    TInt err(KErrNone);

    for( TInt i = 0; i < aPartCount; i++ )
        {
        TDriveName name(TDriveUnit(aDriveNumber + i).Name());
        TRACE1( "FBDrivePartitioner::FormatPartitions: Format %S start", &name );
        RFormat fmt;
        TInt count(0);
        err = fmt.Open(aFs, name, EQuickFormat, count);
        TRACE1( "FBDrivePartitioner::FormatPartitions: Format open=%d", err );
        while (count > 0)
            {
            err = fmt.Next(count);
            if (err != KErrNone)
                {
                TRACE1( "FBDrivePartitioner::FormatPartitions: Format error=%d", err );
                break;
                }
            }
        fmt.Close();
        TRACE1( "FBDrivePartitioner::FormatPartitions: Format %S end", &name );
	    }

    TRACE1("FBDrivePartitioner::FormatPartitions returns %d", err);
    
    return err;
    }

// --------------------------------------------------------------------------------------------
