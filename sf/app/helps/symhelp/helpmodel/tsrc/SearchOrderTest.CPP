// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Test Help Model module to check the PlatSec search order.
// Note: Use could be made of drive A: on the emulator and not on the hardware
// by using the Macro __WINS__
// 
//

// System includes
#include <e32std.h>
#include <e32def.h>
 
#include <e32test.h>
#include <f32file.h>

// User includes
#include <bautils.h>
#include "HLPMODEL.H"
#include <coreappstest/testserver.h>


#include "SearchOrderTest.h"


// These defines and the Local method DrivesOfMediaTypeL should be moved to the 
// Agenda Test Library (agntestlibrary)
_LIT(KMediaNotPresent,	"MediaNotPresent");
_LIT(KMediaUnknown,		"MediaUnknown");
_LIT(KMediaFloppy,		"MediaFloppy");
_LIT(KMediaHardDisk,	"MediaHardDisk");
_LIT(KMediaCdRom,		"MediaCdRom");
_LIT(KMediaRam,			"MediaRam");
_LIT(KMediaFlash,		"MediaFlash");
_LIT(KMediaRom,			"MediaRom");
_LIT(KMediaRemote,		"MediaRemote");
_LIT(KMediaNANDFlash,	"MediaNANDFlash");
TPtrC MediaTypes[] = 
			{ KMediaNotPresent(),KMediaUnknown(),KMediaFloppy(),KMediaHardDisk(),KMediaCdRom(),
			KMediaRam(),KMediaFlash(),KMediaRom(),KMediaRemote(),KMediaNANDFlash()};

CSearchOrderTest::CSearchOrderTest(RTest& aTest):iTest(aTest)
{}


CSearchOrderTest::~CSearchOrderTest()
	{
	iFsSession.Close();
	}
	
CSearchOrderTest* CSearchOrderTest::NewL( RTest& aTest)
	{
	CSearchOrderTest* self = new (ELeave) CSearchOrderTest(aTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CArrayFix<TInt>* CSearchOrderTest::DrivesOfMediaTypeL( TMediaType aMedia, RFs& aFs, TBool aStopAtFirst )
	{
	TDriveInfo info;
	TInt error = KErrNone;
	TDriveList driveList;
	CArrayFix<TInt>* array = new (ELeave) CArrayFixFlat<TInt>(10);
	CleanupStack::PushL( array );
	User::LeaveIfError(aFs.DriveList(driveList));
	for(TInt drive=EDriveA;drive<=EDriveZ;drive++)
		{
		if( driveList[drive] )
			{
			error = aFs.Drive(info, drive);
			if	(error != KErrNone)
				{
				continue;					
				}
			if	( info.iType == aMedia )
				{
				array->AppendL( drive );
				if( aStopAtFirst )
					{ // return if only the first drive of the required type.
					break;									
					}
				}
			}
		}
	CleanupStack::Pop( array );
	return array;
	}

/**
	MMCDriveL
	
	Determines a list of drives of type EMediaHardDisk.
	The first drive in this list is returned and used
	by the rest of the test.
*/
TDriveUnit CSearchOrderTest::MMCDriveL()
	{
	CArrayFix<TInt>* drives = DrivesOfMediaTypeL(EMediaHardDisk,iFsSession,EFalse);
	CleanupStack::PushL(drives);
	TInt drivesTInt = drives->Count();
	if( drivesTInt )
		{
		iTest.Printf(_L("Found the following drives of type %S\n"),&MediaTypes[EMediaHardDisk]);
		for(TInt i=0;i<drivesTInt;i++)
			{
			TDriveUnit drv(drives->At(i));
			TPtrC drvPtr(drv.Name());
			iTest.Printf(_L("%S\n"),&drvPtr);				
			}
		}
	else
		{
		iTest.Printf( _L("No drives found of type %S\n"),&MediaTypes[EMediaHardDisk]);
		User::LeaveIfError(KErrHardwareNotAvailable);		
		}
	TDriveUnit mmcDrive(drives->At(0));
	CleanupStack::PopAndDestroy(drives);
	// Use the first drive in the list for the rest of the test.
	return mmcDrive;
	}

void CSearchOrderTest::ConstructL()
	{
	User::LeaveIfError(iFsSession.Connect());
	iMMCDrive = MMCDriveL().Name();
	iTest.Printf(_L("%S Chosen for the test.\n"), &iMMCDrive);
	iHelpPathFileMMC.Copy(iMMCDrive);
	iHelpPathFileMMC.Append(KHelpPathFileMMC);
	}
	

/**
	MoveHelpFileL
	Deletes and Moves the help files from the anticipated destination drive,
	dependent upon the Operation defined.
*/
void CSearchOrderTest::MoveHelpFileL( TInt aOperation )
	{
	switch( aOperation )
		{
		case EInitial_DeleteAll:
			iTest.Printf(_L("Removing file PlatSecSearchTest.hlp from drives\n"));
			DeleteAllHelpFilesL();
			break;
/*	Commented out because not supported on the hardware.		
		case EMoveToA:
			iTest.Printf(_L("Copying SearchTest.hlp to drives A:"));
			CopyHlpFile( KHelpPathFileAOnZ, KHelpPathFileA );
			break;
*/
		case EMoveToC:
			iTest.Printf(_L("Copying PlatSecSearchTest.hlp to drives C:\n"));
			CopyHlpFileL( KHelpPathFileCOnZ, KHelpPathFileC );
			break;
			
		case EMoveToMMC:
			iTest.Printf(_L("Copying PlatSecSearchTest.hlp to MMC drive\n"));
			CopyHlpFileL( KHelpPathFileFOnZ, iHelpPathFileMMC );
			break;
			
		case EFinal_DeleteAll:
			iTest.Printf(_L("Removing file PlatSecSearchTest.hlp from drives\n"));
			DeleteAllHelpFilesL();
			break;
			
		default:
			// Error condition so leave
			User::Leave(KErrNotFound);
		}

	return;	
	}

/**
	DeleteAllHelpFiles
	Removes help files from A, C & MMC, Z help file will be exported in the
	build process.
*/
void CSearchOrderTest::DeleteAllHelpFilesL()
	{
	// The help file for Z should be exported in the build process.
		
	RPIMTestServer serv;
	User::LeaveIfError(serv.Connect());
//	TRAPD( err, serv.DeleteFileL( KHelpPathFileA ));
	TRAPD( err, serv.DeleteFileL( KHelpPathFileC ));
	TRAP( err, serv.DeleteFileL( iHelpPathFileMMC ));
	serv.Close();
	}
	
void CSearchOrderTest::CopyHlpFileL(const TDesC& aFileName, const TDesC& aToFileName)
	{
	RPIMTestServer serv;
	User::LeaveIfError(serv.Connect());
	serv.CopyFileL(aFileName, aToFileName);
	serv.Close();
	}	

TBool CSearchOrderTest::CheckCategoryListL(TInt aOperation, CDesCArray* aCatList ) const
	{
	TInt mxCount = aCatList->Count();
	iTest.Printf(_L("Category List: \n"));
	for(TInt index=0;index<mxCount;index++)
		{
		TPtrC cat(aCatList->MdcaPoint(index));
		iTest.Printf(_L("%S\n"),&cat);	
		}
	
	TInt pntr = 0;
	switch(aOperation)
		{
		case EInitial_DeleteAll:
			
			iTest.Next(_L("Looking for category 'Drive Z:' in search order.\n"));
			// Check that we Find Drive Z: last in search order.
			return ((aCatList->Find( KDriveZ,	pntr  ) == 0) &&
				   !(aCatList->Find( KDriveC,	pntr  ) == 0) &&
				   !(aCatList->Find( KDriveF, 	pntr  ) == 0));
/* 	Commented out because not supported on the hardware.
		case EMoveToA:
			
			iTest.Next(_L("Looking for Drive A: in search order.\n"));
			// Check that we Find Drive A: last in search order.
			return (aCatList->Find( KDriveA, pntr  ) == 0);
*/		
		case EMoveToC:
			
			iTest.Next(_L("Looking for category 'Drive C:' in search order.\n"));
			// Check that we Find Drive C: last in search order.
			return ((aCatList->Find( KDriveC,	pntr  ) == 0) &&
				   !(aCatList->Find( KDriveZ,	pntr  ) == 0) &&
				   !(aCatList->Find( KDriveF, 	pntr  ) == 0));
			
		case EMoveToMMC:
			
			iTest.Next(_L("Looking for category 'Drive F:' in search order.\n"));
			// Check that we Find Drive F: last in search order.
			return ((aCatList->Find( KDriveF, 	pntr  ) == 0) &&
				   !(aCatList->Find( KDriveC,	pntr  ) == 0) &&
				   !(aCatList->Find( KDriveZ,	pntr  ) == 0));
			
		case EFinal_DeleteAll:
			
			iTest.Next(_L("Looking for category 'Drive Z:' in search order.\n"));
			// Check that we Find Drive Z: last in search order.
			return ((aCatList->Find( KDriveZ,	pntr  ) == 0) &&
				   !(aCatList->Find( KDriveC,	pntr  ) == 0) &&
				   !(aCatList->Find( KDriveF, 	pntr  ) == 0));
			
		default:
			// Error condition so leave
			User::Leave(KErrNotFound);
		}
		
	return EFalse;
	}

