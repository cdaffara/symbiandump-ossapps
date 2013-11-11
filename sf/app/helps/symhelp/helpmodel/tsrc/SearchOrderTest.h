// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __CSEARCHORDERTEST__
#define __CSEARCHORDERTEST__

// Source
//_LIT(KHelpPathFileAOnZ, "Z:\\System\\Help\\SearchTestA.hlp");
_LIT(KHelpPathFileCOnZ, "z:\\System\\Data\\THlpmodel\\PlatSecSearchTestC.hlp");
_LIT(KHelpPathFileFOnZ, "z:\\System\\Data\\THlpmodel\\PlatSecSearchTestF.hlp");
//_LIT(KHelpPathFileZOnZ, "Z:\\Resource\\Help\\SearchTestZ.hlp");

// Destination
//_LIT(KHelpPathFileA, "A:\\Resource\\Help\\PlatSecSearchTest.hlp");
_LIT(KHelpPathFileC, "C:\\Resource\\Help\\PlatSecSearchTest.hlp");
_LIT(KHelpPathFileMMC, "\\Resource\\Help\\PlatSecSearchTest.hlp");
//_LIT(KHelpPathFileZ, "Z:\\Resource\\Help\\SearchTest.hlp");

// Catagorise Define for Catagory UID 0x01007000
_LIT(KDriveZ,"Drive Z:");
//_LIT(KDriveA,"Drive A:");
_LIT(KDriveC,"Drive C:");
_LIT(KDriveF,"Drive F:");


class CSearchOrderTest : public CBase
{
public:
	enum TOperation
		{
		EInitial_DeleteAll,
//		EMoveToA,	This operation is ignored as A: is not supported on the hardware.
		EMoveToC,
		EMoveToMMC,
		EFinal_DeleteAll,
		};	
		
	static CSearchOrderTest* NewL( RTest& aTest );
	void ConstructL();
	~CSearchOrderTest();

	void MoveHelpFileL( TInt aOperation );
	TBool CheckCategoryListL(TInt aEvent, CDesCArray* aCatList ) const;
	
private:
	CSearchOrderTest( RTest& aTest );

	void DeleteAllHelpFilesL();
	void CopyHlpFileL(const TDesC& aFileName, const TDesC& aToFileName);

	TDriveUnit MMCDriveL();
	CArrayFix<TInt>* DrivesOfMediaTypeL( TMediaType aMedia, RFs& aFs, TBool aStopAtFirst );
// Some data
	RFs 			iFsSession;
	RTest&			iTest;
	// First drive encountered mapped to MMC card.
	TDriveName		iMMCDrive;
	// Destination path for Help file on the MMC card.
	TFileName		iHelpPathFileMMC;
};
#endif
