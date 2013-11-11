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


#ifndef CRSDPOFENGINE_H
#define CRSDPOFENGINE_H

#include <e32base.h>
#include <f32file.h>

class CFileManager;

/// Possible DPOF status.
enum TRsDpofPrintStatus
	{
	ECopyImageFiles,
	EDeleteDirectory,
	EDPOFGenerate,
	EJobCompleted,
	EJobCancelled,
	EJobError
	};

class MDpofEngineObserver
	{
	public:
		virtual void HandleDpofEngineEventL(const TInt aStatus, const TInt aRetCode)=0;
	};

class TRsPhoneImage
	{
	public:
		TInt	iIndex;
		TBool	iCopied;
	};


/**
  * \brief Brief Description
  *
  * Detailed Description
  */
class CRsDpofEngine : public CActive
	{
	public:
	
		enum TRsDpofImageLayout
		{
		EOnePerPageSmall = 0,
		EOnePerPageMedium,
		EOnePerPageLarge,
		EOnePerPage4x6,
		ETwoPerPage,
		EFourPerPage,
		ESixPerPage,
		ENinePerPage,
		ETwelvePerPage,
		ESixteenPerPage
		};


		static CRsDpofEngine* NewL(MDpofEngineObserver& aObs);
		~CRsDpofEngine();
		
		void SetImageFileNamesL(const RPointerArray<HBufC>& aImageList);
		void SetImageLayout(TRsDpofImageLayout aImageLayout);
		TInt PrintL();
		TInt GetPrintPercentage();
		void Stop();
		void StartDiscovery(TRequestStatus& aStatus);
		TBool WasCancelled() const;
		TBool HasMmc() const;
		TBool AutoPrintFileExists();
        void SetMMCPathL();		
		
		void SetNumsOfCopiesL( const RArray<TInt>& aNumsOfCopies );

	protected: 	/// From CActive
	
	    void DoCancel();
	    void RunL();

	private:
	
		void ConstructL();
		CRsDpofEngine(MDpofEngineObserver& aObs);
		TInt WriteSpecificSizeFileL();
		TInt WriteMultipleFileL();
		TInt PrepareFilenamesL();
		void CreateDpofFileL();		
		TInt CleanupTempImages();
		TInt CopyPhoneImagesToMMC();
		TRsPhoneImage* GetNextImageInfoForCopy();
		static TInt CreateDpofFileCbL(TAny *aObj);
		void DoCreateDpofFileL();

	private:

		HBufC*					iMmcDrive;
		TBool 					iHasMmc;
		RPointerArray<HBufC> 	iFileNames;
		TInt 					iImageLayout;
		HBufC* 					iAutoPrintFile;
		HBufC* 					iTempFolder;
		MDpofEngineObserver& 	iObs;
		TRsDpofPrintStatus	 	iStep;
		RFs						iFs;
		RArray<TRsPhoneImage>	iPhoneImageArray;
		TRsPhoneImage*		 	iCurrentPhoneImage;
		CFileManager*			iFileManager;
		TFileName			 	iTempFile;
		TBool				 	iShouldCancel;
		CPeriodic*				iDpofPeriodic;
		TInt					iPhoneImgsSize;
		
		RArray<TInt>			iNumsOfCopies;
	};

#endif // CRSDPOFENGINE_H

//  End of File
