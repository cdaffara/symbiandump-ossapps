/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains the CRsDpofEngine & TRsPhoneImage class definitions and
* 				 the MDpofEngineObserver interface definition.
*
*/


#include <e32std.h>
#include <s32file.h>
#include <pathinfo.h>
#include <sysutil.h>
#include <driveinfo.h>

#include "crsdpofengine.h"
#include "rsutils.h"
#include "imgprintkonst.h"
#include "clog.h"
#include "cfilemanager.h"

//  CONSTANTS
namespace
	{
	_LIT(KBackslash, "\\");
	const TInt KCopyPercentage = 90;
	const TInt KAutoPrintFileAddon=2048;

	_LIT(KDpofAutoPrintFilename,	"\\MISC\\AUTPRINT.MRK");
	_LIT(KDpofAutoPrintTempFolder,	"\\DPOF\\");
	_LIT8(KDoubleQuote,				"\"");

	_LIT8(KCrLf,					"\r\n");
	_LIT8(KDpofHeader,				"[HDR]\r\nGEN REV = 01.10\r\nGEN CRT = \"ImagePrint\" -02.10\r\nGEN DTM = ");
	_LIT8(KDpofJobAndPid,			"[JOB]\r\nPRT PID = ");
	_LIT8(KDpofPrtTypSizeSmall,		"PRT TYP = SIZ -03\r\n");
	_LIT8(KDpofPrtTypSizeMedium,	"PRT TYP = STD\r\n");
	_LIT8(KDpofPrtTypSizeLarge,		"PRT TYP = SIZ -05\r\n");
	_LIT8(KDpofPrtTypSize4x6,		"PRT TYP = STD\r\n");
	_LIT8(KDpofPrtTypSize2pp,		"PRT TYP = MUL -02\r\n");
	_LIT8(KDpofPrtTypSize4pp,		"PRT TYP = MUL -04\r\n");
	_LIT8(KDpofPrtTypSize6pp,		"PRT TYP = MUL -06\r\n");
	_LIT8(KDpofPrtTypSize9pp,		"PRT TYP = MUL -09\r\n");
	_LIT8(KDpofPrtTypSize12pp,		"PRT TYP = MUL -12\r\n");
	_LIT8(KDpofPrtTypSize16pp,		"PRT TYP = MUL -16\r\n");
	_LIT8(KDpofPrtQtyAndImgFmt,		"PRT QTY = 001\r\nIMG FMT = EXIF2 -J\r\n");
	_LIT8(KDpofImgSrc,				"IMG SRC = ");

	_LIT8(KDateFormat,"%4d:%02d:%02d:%02d:%02d:%02d");
	_LIT( KDotDot, ".." );
	_LIT( KSlash, "/" );
	const TInt KDpofDelay = 500000;	
	}




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRsDpofEngine* CRsDpofEngine::NewL(MDpofEngineObserver& aObs)
	{
    CRsDpofEngine* self = new (ELeave) CRsDpofEngine(aObs);
	CleanupStack::PushL(self);
	self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}


void CRsDpofEngine::ConstructL()
	{	    
	LOG("CRsDpofEngine::ConstructL begin");
	User::LeaveIfError( iFs.Connect() );
	iFileManager = CFileManager::NewL( iFs );
	LOG("CRsDpofEngine::ConstructL end");
	}

CRsDpofEngine::CRsDpofEngine(MDpofEngineObserver& aObs) : 
											CActive(CActive::EPriorityStandard),
											iHasMmc(EFalse),
											iImageLayout(0),
											iObs(aObs),
											iShouldCancel(EFalse)
	{
	CActiveScheduler::Add(this);
	}

CRsDpofEngine::~CRsDpofEngine()
	{
	Cancel();
	if(iDpofPeriodic) iDpofPeriodic->Cancel();
	delete iDpofPeriodic;	
	delete iMmcDrive;
	delete iAutoPrintFile;
	delete iTempFolder;
	iFileNames.ResetAndDestroy();
	iFileNames.Close();
	iPhoneImageArray.Close();
	delete iFileManager;
	iFs.Close();
	iNumsOfCopies.Close();
	}
	
void CRsDpofEngine::SetMMCPathL()
	{
	LOG("CRsDpofEngine::SetMMCPathL begin");

	TFileName fileInfo;	
	TInt driveId;
	TFileName path;
	User::LeaveIfError( DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRemovableMassStorage, driveId ) );
	User::LeaveIfError( PathInfo::GetRootPath( path, driveId ) );
	HBufC* mmcRoot = path.AllocLC();
   	
   	LOG1("CRsDpofEngine::SetMMCPathL mmcRoot: %S", mmcRoot);
	iHasMmc = ( mmcRoot && mmcRoot->Length() && RsUtils::PathExists( *mmcRoot, iFs ) );
	
	if( iHasMmc )
		{
		LOG("CRsDpofEngine::SetMMCPathL MMC found");		
		iMmcDrive = mmcRoot->Left( 2 ).AllocL();
		fileInfo.Copy( *iMmcDrive );
		fileInfo.Append( KDpofAutoPrintFilename );				
		iAutoPrintFile = fileInfo.AllocL();
		fileInfo.Copy( *iMmcDrive );
		fileInfo.Append( KDpofAutoPrintTempFolder );		
		iTempFolder = fileInfo.AllocL();
		}

	CleanupStack::PopAndDestroy( mmcRoot );	

	LOG("CRsDpofEngine::SetMMCPathL end");
	}
TInt CRsDpofEngine::CleanupTempImages()
	{
	LOG("CRsDpofEngine::CleanupTempImages begin");
	iStatus = KRequestPending;		
	TInt retVal = iFileManager->RmDir( *iTempFolder, iStatus );
	if( !retVal && !IsActive() ) SetActive();		
	LOG1("CRsDpofEngine::CleanupTempImages reurn: %d", retVal);  
	return retVal;	
	}

void CRsDpofEngine::SetImageFileNamesL(const RPointerArray<HBufC>& aImageList)
	{
	LOG("CRsDpofEngine::SetImageFileNamesL begin");
	iFileNames.ResetAndDestroy();
	for( TInt i = 0; i < aImageList.Count(); i++)
		{
		HBufC* fileName = aImageList[i]->AllocLC();
		LOG1("CRsDpofEngine::SetImageFileNamesL file: %S", fileName); 
		User::LeaveIfError( iFileNames.Append( fileName ) );
		CleanupStack::Pop( fileName );
		}		
	LOG("CRsDpofEngine::SetImageFileNamesL end");
	}

void CRsDpofEngine::SetImageLayout(TRsDpofImageLayout aImageLayout)
	{
	iImageLayout = aImageLayout;
	}


TInt CRsDpofEngine::PrintL()
	{
	LOG("CRsDpofEngine::PrintL begin");
	TInt retVal( KErrNone );
	iPhoneImgsSize = 0;

	iShouldCancel = EFalse;			
	iPhoneImageArray.Reset();

	// Check if there are phone memory images, and add up their total space
	LOG("CRsDpofEngine::PrintL checking phone memory images...");		
	for( TInt i = 0; i < iFileNames.Count() && !retVal; i++ )
		{		
		if( !RsUtils::FileOnDrive( (*iMmcDrive)[0], *(iFileNames[i]) ) )
			{
			LOG1("CRsDpofEngine::PrintL phone memory image: %S", iFileNames[i]);
			TRsPhoneImage phoneImage;
			phoneImage.iIndex = i;
			phoneImage.iCopied = EFalse;
			iPhoneImageArray.AppendL( phoneImage );				
			retVal = RsUtils::CumulativeFileSize( *(iFileNames[i]), iFs, iPhoneImgsSize );
			LOG1("CRsDpofEngine::PrintL error checking phone memory images: %d", retVal);									
			}								
		}
	LOG("CRsDpofEngine::PrintL checking phone memory images end");
	LOG1("CRsDpofEngine::PrintL checking phone memory images, size: %d", iPhoneImgsSize);
									
	if( !retVal )
		{
		retVal = CleanupTempImages();
		LOG1("CRsDpofEngine::PrintL error deleting temp folder: %d", retVal);
		if( retVal == KErrNone )
			{
			iStep = EDeleteDirectory;
			iObs.HandleDpofEngineEventL( EDeleteDirectory, retVal );	
			}	
		}
	
	LOG1("CRsDpofEngine::PrintL return: %d", retVal);
	return retVal;						
	}
	
	
TInt CRsDpofEngine::PrepareFilenamesL()
	{
	LOG("CRsDpofEngine::PrepareFilenamesL begin");
	TInt retVal( KErrNone );
		

	// Walk through the list of files
	// - replacing long names with short names
	// - "\" with "/"
	// - MMC drive "e:" with ".."
	
	LOG1("CRsDpofEngine::PrepareFilenamesL iFileNames->Count(): %d", iFileNames.Count());
	for( TInt i = 0; i < iFileNames.Count(); i++ )
		{
		LOG1("CRsDpofEngine::PrepareFilenamesL original: %S", iFileNames[i]);

		TFileName shortFilename;
		retVal = iFs.GetShortName( *(iFileNames[i]), shortFilename );
		if( retVal != KErrNone )
			{
			LOG1("CRsDpofEngine::PrepareFilenamesL 1 error getting short name: %d", retVal);
			return retVal;	
			}
		
		TParse parse;
		parse.Set( *(iFileNames[i]), NULL, NULL );	
		parse.Set( parse.DriveAndPath(), NULL, NULL );
		while( !parse.IsRoot() )
			{
			TBuf<16> shortDir;
			retVal = iFs.GetShortName( parse.DriveAndPath(), shortDir );
			if( retVal != KErrNone )
				{
				LOG1("CRsDpofEngine::PrepareFilenamesL 2 error getting short name: %d", retVal);
				return retVal;	
				}

			shortFilename.Insert( 0, KSlash );
			shortFilename.Insert( 0, shortDir );
			
			parse.PopDir();
			}
		shortFilename.Insert( 0, KSlash );
		shortFilename.Insert( 0, KDotDot );

		delete iFileNames[i];
		iFileNames[i] = NULL;
		iFileNames[i] = shortFilename.AllocL();
		
		LOG1("CRsDpofEngine::PrepareFilenamesL changed: %S", iFileNames[i]);	
		}

	LOG1("CRsDpofEngine::PrepareFilenamesL end with: %d", retVal);
	return retVal;
	}

TInt CRsDpofEngine::WriteSpecificSizeFileL()
	{
	LOG("CRsDpofEngine::WriteSpecificSizeFile begin");
	// Creates the autoprint file for a Specific Size image print
	RFile autoPFile;
	TBuf8<256> tempDes;
	TTime currTime;
	TDateTime currDT;
	currTime.HomeTime();
	currDT = currTime.DateTime();

	// Open the autoprint file
	TInt retVal = autoPFile.Replace( iFs, *iAutoPrintFile, EFileWrite );
	if( retVal != KErrNone )
		{
		LOG1("CRsDpofEngine::WriteSpecificSizeFile error opening autoprint file: %d", retVal);
		return retVal;	
		}
		
	CleanupClosePushL(autoPFile);	
	RFileWriteStream writeStream(autoPFile);
	CleanupClosePushL(writeStream);		

	// Write the common header information
	LOG("CRsDpofEngine::WriteSpecificSizeFile Write the common header information");
	writeStream.WriteL(KDpofHeader);
	tempDes.Format(KDateFormat, currDT.Year(), (currDT.Month()+1), (currDT.Day()+1), currDT.Hour(), currDT.Minute(), currDT.Second());
	writeStream.WriteL(tempDes);
	writeStream.WriteL(KCrLf);

	// For each image in the array
	LOG("CRsDpofEngine::WriteSpecificSizeFile For each image in the array");
	LOG1("CRsDpofEngine::WriteSpecificSizeFile iFileNames->Count(): %d", iFileNames.Count());
	
	TInt pid( 1 );

	
	for( TInt i = 0; i < iFileNames.Count(); i++ )
		{
		LOG1("CRsDpofEngine::WriteSpecificSizeFile i: %d", i);
		LOG1("CRsDpofEngine::WriteSpecificSizeFile iNumsOfCopies[i]: %d", iNumsOfCopies[i]);
		for( TInt j = 0; j < iNumsOfCopies[i]; j++ )
			{
			pid += j;			
			// write a job section, followed by the PID
			LOG1("CRsDpofEngine::WriteSpecificSizeFile write a job section, followed by the PID: %d", pid);
			writeStream.WriteL(KDpofJobAndPid);
			tempDes.NumFixedWidth( pid, EDecimal, 3 );
			writeStream.WriteL(tempDes);
			writeStream.WriteL(KCrLf);

			LOG1("CRsDpofEngine::WriteSpecificSizeFile iImageLayout: %d", iImageLayout);
			switch( iImageLayout )
				{
				case EOnePerPageSmall:
					writeStream.WriteL(KDpofPrtTypSizeSmall);
					break;
				case EOnePerPageMedium:
					writeStream.WriteL(KDpofPrtTypSizeMedium);
					break;
				case EOnePerPageLarge:
					writeStream.WriteL(KDpofPrtTypSizeLarge);
					break;
				case EOnePerPage4x6:
					writeStream.WriteL(KDpofPrtTypSize4x6);
					break;
				default:
					break;
				}

			LOG1("CRsDpofEngine::WriteSpecificSizeFile file: %S", iFileNames[i]);
			writeStream.WriteL(KDpofPrtQtyAndImgFmt);
			writeStream.WriteL(KDpofImgSrc);
			writeStream.WriteL(KDoubleQuote);
			tempDes.Copy(*(iFileNames[i]));
			writeStream.WriteL(tempDes);
			writeStream.WriteL(KDoubleQuote);
			writeStream.WriteL(KCrLf);
			}
		}


	writeStream.CommitL();	
	CleanupStack::PopAndDestroy(2); // writeStream, autoPFile
	
	LOG1("CRsDpofEngine::WriteSpecificSizeFile end with: %d", retVal);
	return retVal;
	}

TInt CRsDpofEngine::WriteMultipleFileL()
	{
	LOG("CRsDpofEngine::WriteMultipleFile begin");
	// Creates the autoprint file for a Multiple image print
	TInt retVal=KErrNone;
	RFile autoPFile;
	TBuf8<256> tempDes;
	TTime currTime;
	TDateTime currDT;
	currTime.HomeTime();
	currDT = currTime.DateTime();

	// Open the autoprint file
	retVal = autoPFile.Replace( iFs, *iAutoPrintFile, EFileWrite );
	if( retVal != KErrNone )
		{
		LOG1("CRsDpofEngine::WriteMultipleFile error opening autoprint file: %d", retVal);
		return retVal;	
		}
	
	CleanupClosePushL(autoPFile);	
	RFileWriteStream writeStream(autoPFile);
	CleanupClosePushL(writeStream);		
		
	// Write the common header information
	LOG("CRsDpofEngine::WriteMultipleFile Write the common header information");
	writeStream.WriteL(KDpofHeader);
	tempDes.Format(KDateFormat, currDT.Year(), (currDT.Month()+1), (currDT.Day()+1), currDT.Hour(), currDT.Minute(), currDT.Second());
	writeStream.WriteL(tempDes);
	writeStream.WriteL(KCrLf);

	// Start the job section, followed by the PID
	LOG("CRsDpofEngine::WriteMultipleFile Start the job section, followed by the PID");
	writeStream.WriteL(KDpofJobAndPid);
	tempDes.NumFixedWidth(1, EDecimal, 3);
	writeStream.WriteL(tempDes);
	writeStream.WriteL(KCrLf);

	LOG1("CRsDpofEngine::WriteMultipleFile iImageLayout: %d", iImageLayout);
	switch (iImageLayout)
		{
		case ETwoPerPage:
			writeStream.WriteL(KDpofPrtTypSize2pp);
			break;
		case EFourPerPage:
			writeStream.WriteL(KDpofPrtTypSize4pp);
			break;
		case ESixPerPage:
			writeStream.WriteL(KDpofPrtTypSize6pp);
			break;
		case ENinePerPage:
			writeStream.WriteL(KDpofPrtTypSize9pp);
			break;
		case ETwelvePerPage:
			writeStream.WriteL(KDpofPrtTypSize12pp);
			break;
		case ESixteenPerPage:
			writeStream.WriteL(KDpofPrtTypSize16pp);
			break;
		default:
			break;
		}
	writeStream.WriteL(KDpofPrtQtyAndImgFmt);

	// For each image in the array
	LOG("CRsDpofEngine::WriteMultipleFile For each image in the array");
	LOG1("CRsDpofEngine::WriteMultipleFile iFileNames->Count(): %d", iFileNames.Count());
	

	for( TInt i = 0; i < iFileNames.Count(); i++)
		{
		LOG1("CRsDpofEngine::WriteMultipleFile i: %d", i);
		LOG1("CRsDpofEngine::WriteMultipleFile iNumsOfCopies[i]: %d", iNumsOfCopies[i]);
		for( TInt j = 0; j < iNumsOfCopies[i]; j++ )
			{					
			LOG1("CRsDpofEngine::WriteMultipleFile file: %S", iFileNames[i]);
			// add it to the job section
			writeStream.WriteL(KDpofImgSrc);
			writeStream.WriteL(KDoubleQuote);
			tempDes.Copy(*(iFileNames[i]));
			writeStream.WriteL(tempDes);
			writeStream.WriteL(KDoubleQuote);
			writeStream.WriteL(KCrLf);
			}
		}

	
	writeStream.CommitL();	
	CleanupStack::PopAndDestroy(2); // writeStream, autoPFile

	LOG1("CRsDpofEngine::WriteMultipleFile end with: %d", retVal);
	return retVal;
	}

void CRsDpofEngine::DoCancel()
	{	
	LOG("CRsDpofEngine::DoCancel begin");
	iShouldCancel = ETrue;	
	LOG("CRsDpofEngine::DoCancel end");
	}
	
void CRsDpofEngine::Stop()
	{
	LOG("CRsDpofEngine::Stop begin");
	iShouldCancel = ETrue;
	iFileManager->CancelCopy();	
	iFileManager->CancelRmDir();
	LOG("CRsDpofEngine::Stop end");
	}	

void CRsDpofEngine::RunL()
	{
	LOG1("CRsDpofEngine::RunL called with iStatus.Int(): %d", iStatus.Int());
	TInt retVal( KErrNone );
			
	if( iStatus.Int() != KErrNone )
		{		
		LOG("CRsDpofEngine::RunL there was an error, so cleanup");
		iObs.HandleDpofEngineEventL(iStep, iStatus.Int());
		}
	else
		{
		LOG1("CRsDpofEngine::RunL iStep: %d", iStep);
		switch( iStep )
			{
			case ECopyImageFiles:
				{
				// change those filename array entries so that the filename is now correct
				TParsePtrC parsePtr( *(iFileNames[iCurrentPhoneImage->iIndex]) );
				iTempFile.Append( parsePtr.NameAndExt() );
				delete iFileNames[iCurrentPhoneImage->iIndex];
				iFileNames[iCurrentPhoneImage->iIndex] = NULL;
				iFileNames[iCurrentPhoneImage->iIndex] = iTempFile.AllocL();
				iCurrentPhoneImage->iCopied = ETrue;												

				retVal = CopyPhoneImagesToMMC();
				LOG1("CRsDpofEngine::RunL CopyPhoneImagesToMMC returned: %d", retVal);
				if( retVal == KErrNone )
					{
					iObs.HandleDpofEngineEventL(iStep, retVal);
					}
				else if( retVal == KErrCancel )
					{
					CreateDpofFileL();						
					}					
				else
					{
					iObs.HandleDpofEngineEventL( iStep, retVal );	
					}					
				}
				break;				
			case EDeleteDirectory:
				{
				LOG1("CRsDpofEngine::RunL iPhoneImageArray.Count(): %d", iPhoneImageArray.Count());
				if( iPhoneImageArray.Count() )
					{
					TUint32 size( 0 );
					RsUtils::DriveFreeSpace( (*iMmcDrive)[0], iFs, size );
										
					if( SysUtil::MMCSpaceBelowCriticalLevelL( &iFs, iPhoneImgsSize ) )
        				{
        				LOG("CRsDpofEngine::PrintL below MMC critical level");
						retVal = KErrTooBig;
        				}
        			else if( size < ( iPhoneImgsSize + KAutoPrintFileAddon ) )
						{
						LOG("CRsDpofEngine::PrintL NO room on MMC for phone memory images");
						retVal = KErrTooBig;	
						}														
					else
						{
						LOG("CRsDpofEngine::PrintL enough room on MMC for phone memory images");
						if( !RsUtils::PathExists( *iTempFolder, iFs ) )
							{
							retVal = iFs.MkDir( *iTempFolder );
							LOG1("CRsDpofEngine::PrintL error making temp folder: %d", retVal);
							if( !retVal )
								{
								retVal = CopyPhoneImagesToMMC();
								LOG1("CRsDpofEngine::PrintL error copying phone memory images: %d", retVal);
								}																																	
							}																								
						}
										
					iObs.HandleDpofEngineEventL( ECopyImageFiles, retVal );																											
					}
				else
					{
					CreateDpofFileL();
					iObs.HandleDpofEngineEventL( EDPOFGenerate, KErrNone );	
					}																
				}
				break;							
			default:
				break;
			}	
		}
	LOG("CRsDpofEngine::RunL end");
	}


TInt CRsDpofEngine::CopyPhoneImagesToMMC()
	{	
	LOG("CRsDpofEngine::CopyPhoneImagesToMMC begin");
	TInt retVal = KErrCancel;
	iStep = ECopyImageFiles;
	if( iShouldCancel )
		{
		LOG1("CRsDpofEngine::CopyPhoneImagesToMMC iShouldCancel== ETrue, end with: %d", retVal);
		return retVal;	
		}
				
	iCurrentPhoneImage = GetNextImageInfoForCopy();

	if( iCurrentPhoneImage )
		{
		// copy phone mem images to MMC card temp dir
		TTime theTime;
		theTime.HomeTime();
		iTempFile.Copy( *iTempFolder );
		iTempFile.AppendNumUC( theTime.Int64(), EHex );													
		iTempFile.Append( KBackslash );
		retVal = iFs.MkDir( iTempFile );
		LOG1("CRsDpofEngine::CopyPhoneImagesToMMC error making directory: %d", retVal);		
		if( retVal == KErrNone )
			{
			retVal = iFileManager->Copy( *(iFileNames[iCurrentPhoneImage->iIndex]), iTempFile, iStatus );
			LOG1("CRsDpofEngine::CopyPhoneImagesToMMC error copying: %d", retVal);
			if( retVal == KErrNone )
				{
				iStatus = KRequestPending;
				if( !IsActive() ) SetActive();
				}										
			}						
		}

	LOG1("CRsDpofEngine::CopyPhoneImagesToMMC end with: %d", retVal);
	return retVal;
	}

TRsPhoneImage* CRsDpofEngine::GetNextImageInfoForCopy()
	{		
	TRsPhoneImage* phoneImage = NULL;	
	for( TInt i = 0; i < iPhoneImageArray.Count() && !phoneImage; i++ )	
		{
		if( iPhoneImageArray[i].iCopied == EFalse )
			{
			phoneImage = &( iPhoneImageArray[i] );
			}
		}
	return phoneImage;
	}


TInt CRsDpofEngine::GetPrintPercentage()
	{
	LOG("CRsDpofEngine::GetPrintPercentage begin");
	// compute the percentage of the printing job has been completed
	TInt copied( 0 );
	TInt count = iPhoneImageArray.Count();	
	
	// assumes that writing to autoprint file takes 10% of the work
	for( TInt i = 0; i < count; i++ )
		{
		if( iPhoneImageArray[i].iCopied )
			copied++;
		}

	TInt percentage = (copied == count) ? KCopyPercentage : ((copied*100)/count);
	LOG1("CRsDpofEngine::GetPrintPercentage end with: %d", percentage);
	return percentage;		
	}

void CRsDpofEngine::CreateDpofFileL()
	{
	if(iDpofPeriodic) iDpofPeriodic->Cancel();
	delete iDpofPeriodic;
	iDpofPeriodic = NULL;	
	iDpofPeriodic = CPeriodic::NewL(CActive::EPriorityStandard);
	iDpofPeriodic->Start(KDpofDelay,KDpofDelay,TCallBack(CreateDpofFileCbL,this));
	}

void CRsDpofEngine::StartDiscovery(TRequestStatus& aStatus)
	{		
	LOG("CRsDpofEngine::StartDiscovery begin");
	aStatus = KRequestPending;
	TRequestStatus *status = &aStatus;	
	if( iHasMmc )
		{
		LOG1("CRsDpofEngine::StartDiscovery end with: %d", EPrinterDiscovered);
		User::RequestComplete(status,EPrinterDiscovered);	
		}		
	else
		{
		LOG1("CRsDpofEngine::StartDiscovery end with: %d", EDiscoveryDone);
		User::RequestComplete(status,EDiscoveryDone);	
		}		
	}


TBool CRsDpofEngine::WasCancelled() const
	{
	return iShouldCancel;
	}

TBool CRsDpofEngine::AutoPrintFileExists()
	{
    return ( RsUtils::FileExists( *iAutoPrintFile, iFs ) );
	}

TInt CRsDpofEngine::CreateDpofFileCbL(TAny *aObj)	
	{	
	CRsDpofEngine* obj = static_cast<CRsDpofEngine*>(aObj);
	if( obj->iDpofPeriodic ) obj->iDpofPeriodic->Cancel();		
	obj->DoCreateDpofFileL();
	return EFalse;
	}

void CRsDpofEngine::DoCreateDpofFileL()
	{
	LOG("CRsDpofEngine::DoCreateDpofFileL begin");	
	
	TParsePtrC parse( *iAutoPrintFile );

	// Delete existing autoprint file
	if( RsUtils::FileExists( *iAutoPrintFile, iFs ) )
		{
		LOG("CRsDpofEngine::DoCreateDpofFileL deleting existing autoprint file...");
		User::LeaveIfError( iFs.Delete( *iAutoPrintFile ) );	
		}		
	else
		{
		LOG("CRsDpofEngine::DoCreateDpofFileL making sure that the autoprint file folder exists...");
		// Make sure that the autoprint file folder exists
		if( !RsUtils::PathExists( parse.DriveAndPath(), iFs ) )
			User::LeaveIfError( iFs.MkDir( parse.DriveAndPath() ) );					
		}

	// Ready the array of filenames for DPOF generation
	LOG("CRsDpofEngine::DoCreateDpofFileL preparing file names...");
	PrepareFilenamesL();

	TInt retVal=KErrNone;
	// Generate autoprint file
	LOG1("CRsDpofEngine::DoCreateDpofFileL printing layout: %d", iImageLayout);
	switch( iImageLayout )
		{
		case EOnePerPageSmall:
		case EOnePerPageMedium:
		case EOnePerPageLarge:
		case EOnePerPage4x6:
			retVal = WriteSpecificSizeFileL();
			break;
		case ETwoPerPage:
		case EFourPerPage:
		case ESixPerPage:
		case ENinePerPage:
		case ETwelvePerPage:
		case ESixteenPerPage:
			retVal = WriteMultipleFileL();
			break;
		default:
			// Should never be here
			User::Leave(KErrGeneral);
			break;
		}

	LOG1("CRsDpofEngine::DoCreateDpofFileL end with: %d", retVal);
	if (retVal == KErrNone)
		{
		iObs.HandleDpofEngineEventL(EJobCompleted, KErrNone);
		}
	else
		{		
		iObs.HandleDpofEngineEventL(EJobError, retVal);
		}	
	}

TBool CRsDpofEngine::HasMmc() const
	{
	return iHasMmc;
	}


void CRsDpofEngine::SetNumsOfCopiesL( const RArray<TInt>& aNumsOfCopies )
	{
	LOG("CRsDpofEngine::SetNumsOfCopiesL begin");
	iNumsOfCopies.Reset();
	TInt count = aNumsOfCopies.Count();
	for( TInt i = 0; i < count; i++ )
		{
		iNumsOfCopies.AppendL( aNumsOfCopies[i] );
		}
	LOG("CRsDpofEngine::SetNumsOfCopiesL end");	
	}

//  End of File
