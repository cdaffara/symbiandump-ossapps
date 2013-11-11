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
* Description:  Contains the CPrintJob class definition and the PJS enum.
*
*/


#ifndef CPRINTJOB_H
#define CPRINTJOB_H

#include <e32base.h>

#include "cxhtmlfilecomposer.h"
#include "crsbtdevice.h"

#define KMaxIdLength 10


class TPrintSetting
{
	public:
		TInt iCapability;
		TInt iValue;
};

NONSHARABLE_CLASS( CPrintJob ): public CBase
{
	public:
		/// Possible states of a print job.
		enum TJobState
		{
			EStateNone,
			EStateCreated,
			EStateSubmitted,
			EStateSending,
			EStatePrinting,
			EStateCancel
		};
	
	public:
		/*
		* @function 	NewL
		* @description	Constructs the CPrintJob object.
		* 				NOTE: The ownership of aFileFactory does not change here!
		* @param		aPrinter		Selected printer for the job
		* @param		aImages			Array of images to be printed
		* @param		aFileFactory	Pointer to XHTML-print file factory
		* @param		aUserName		User name got from central repository
		*/
		static CPrintJob* NewL(CRsBtDevice* aDevice, RArray<TFileName>& aImages, const TDesC8& aUserName);

		/*
		* @function 	Destructor
		* @description	Destroys the object and frees the memory allocated by this object
		*/
		~CPrintJob();

		/*
		* @function 	SetJobId
		* @description	Initializes the job id
		* @param		TDesC& aId
		*/
		void SetJobId(const TInt aId);

		/*
		* @function 	JobId
		* @description	Returns the job id as TInt
		* @return 		Job id
		*/
		TInt JobId();

		/*
		* @function 	GetJobName
		* @description	Returns the file path
		* @param 		TDes8&
		*/
		void GetJobName(TDes8& aJobName);

		/*
		* @function 	ImageCount
		* @description	Returns the number of images in job.
		* @return 		TInt
		*/
		TInt ImageCount();

		/*
		* @function 	Images
		* @description	Returns job's images as CImageInfo
		* @param		RArray<CImageInfo> Output parameter
		*/
		void Images(RArray<CImageInfo>& aImages);

		/*
		* @function 	PrinterId
		* @description	Returns the printer id
		* @param		TInt
		*/
		TInt PrinterId();

		/*
		* @function 	Device
		* @description	Returns the printer id
		* @param		TInt
		*/
		CRsBtDevice* Device();

		/*
		* @function 	GetUserName
		* @description	Returns the user name
		* @param 		TPtrC8&
		*/
		void GetUserName(TPtrC8& aDocFormat);

		/*
		* @function 	SetNumsOfCopies
		* @description	Sets the number of copies for certain image.
		* @param 		TInt aCapabilityId
		* @param		TInt aValue
		* @return		TInt System wide error code
		*/
		TInt SetNumsOfCopies( const RArray<TInt>& aNumsOfCopies);

		/*
		* @function 	XhtmlPrintFile
		* @description	Returns the file path of generated XHTML-Print file
		* @param 		TDes8&
		*/
		void GetPrintFileL(TDes8& aPrintFile);

		/*
		* @function 	Sheets
		* @description	Returns the total number of sheets the job requires
		* @return 		TInt
		*/
		TInt Sheets();

		/*
		* @function 	JobState
		* @description	Returns the job state
		* @return 		TJobState
		*/
		TJobState JobState();

		/*
		* @function 	UpdateJobState
		* @description	Updates the job state
		*/
		void UpdateJobState(const TJobState aState);

		/*
		* @function 	GetPrintSetting
		* @description	Gets print settings
		*/
		TInt GetPrintSetting(TInt aCapabilityID, TInt& aValue);
		TInt SetPrintSetting(TInt aCapabilityID, TInt aValue);


	protected:
		/*
		* @function 	ConstructL
		* @description	Construction's second phase
		* @param		aPriner		Selected printer for the job
		* @param		aImages		Array of images to be printed
		*/
		void ConstructL(RArray<TFileName>& aImages, const TDesC8& aUserName);

	private:

		/*
		* @function 	CPrintJob
		* @description	C++ constructor
		* @param		aPrinterId	Id of the printer to be used for this job.
		*
		*/
		CPrintJob(CRsBtDevice* aDevice);

	private:

	//@var iJobId				Id for printing job received from printer control point
	TInt iJobId;

	//@var iDeviceId			Defines the printer to be used.
	TInt iDeviceId;

	CRsBtDevice* iDevice;

	//@var iUserName			Name of the user
	TFileName8 iUserName;

	//@var iImages				Job's images
	RArray<CImageInfo> iImages;

	//@var iSheets				Number of sheets to print
	TInt iSheets;

	TJobState iJobState;

};

#endif // CPRINTJOB_H

//  End of File
