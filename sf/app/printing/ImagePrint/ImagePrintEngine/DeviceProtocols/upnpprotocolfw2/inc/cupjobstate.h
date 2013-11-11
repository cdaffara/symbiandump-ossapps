/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares CUPJobState class
*
*/


#ifndef CUPJOBSTATE_H
#define CUPJOBSTATE_H

#include <e32base.h>

#include "upconsts.h"

/*
* @class 		TUPJobEndState
* @description	This class contains the state variables related to the end / abort state
*				of the job that is active at printer.
*/
NONSHARABLE_CLASS(TUPJobEndState)
{
	public:
		inline TUPJobEndState(){Init();}

		inline void Init()
		{
			iId.Set(KNullDesC8());
			iJobName.Set(KNullDesC8());
			iOriginator.Set(KNullDesC8());
			iSheets = KErrNotFound;
			iState = EStateUnknown;
			iReason = ENoMessage;
			iAbortState = EFalse;
		}

		TPtrC8 iId;
		TPtrC8 iJobName;
		TPtrC8 iOriginator;
		TInt iSheets;
		TUPnPState iState;
		TInt iReason;
		TBool iAbortState;
};

/*
* @class 		CUPJobState
* @description	This class collects and maintains the processing state of the job that is active at printer.
*/
NONSHARABLE_CLASS(CUPJobState) : public CBase
{
	public:

		/*
		* @function		NewL
		* @description	A method to create a new object.
		*/
		static CUPJobState* NewL();

		/*
		* @function		NewLC
		* @description	A method to create a new object.
		*/
		static CUPJobState* NewLC();

		/*
		* @function		~CUPJobState
		* @description	Destructor 
		*/
		~CUPJobState();

		/*
		* @function		Init
		* @description	Initializes private members 
		*/
		void Init();

		/*
		* @function		State
		* @description	Returns the stored state of the job 
		* @return		TUPnPState
		*/
		TUPnPState State();

		/*
		* @function		SetActiveId
		* @description	Sets the id for the active job at UPnPProtocol 
		* @return		TUPnPState
		*/
		void SetActiveId(const TDesC8& aId);
		
		/*
		* @function		ActiveId
		* @description	Returns id of the job active at UPnPProtocol 
		* @return		TUPnPState
		*/
		TPtrC8 ActiveId();

		/*
		* @function		SheetsCompleted
		* @description	Returns the number of printed sheets of the job 
		* @return		TInt
		*/
		TInt SheetsCompleted();

		/*
		* @function		Reason
		* @description	Returns the reason code for the job state 
		* @return		TInt
		*/
		TInt Reason();

		/*
		* @function		SetSheetsToPrint
		* @description	Sets the number of the sheets about to print 
		* @param		TInt
		*/
		void SetSheetsToPrint(TInt aSheets);
		
		/*
		* @function		SheetsToPrint
		* @description	Returns the number of the sheets about to print 
		* @return		TInt
		*/
		TInt SheetsToPrint();
		
		/*
		* @function		SetSheetsCompleted
		* @description	Returns and updates the job state according to the printed sheets
		* @param		TDesC8 Reference to the string value of printed sheets
		*/
		void SetSheetsCompleted(const TDesC8& aSheetsCompleted);

		/*
		* @function		Progress
		* @description	Returns progress percentage of the job 
		* @param		TInt Number of the sheets sent to printer
		* @return		TInt
		*/
		TInt Progress(TInt aSheets);
		
		/*
		* @function		Progress
		* @description	Returns progress percentage of the job 
		* @return		TInt
		*/
		TInt Progress();
		
		/*
		* @function		UpdateJobState
		* @description	Returns the state of the job after updated with given arguments 
		* @param		TDesC8& Printer state
		* @param		TDesC8& Reason for the printer state
		* @param		TDesC8& List of job ids pending at the printer queue
		* @param		TDesC8& Id of the currently handled job in printer
		* @param		TDesC8& Number of the sheets completed at printer
		* @return		TUPnPState
		*/
		TUPnPState UpdateJobState(const TDesC8& aPrinterState, const TDesC8& aReason, const TDesC8& aJobList, const TDesC8& aJobId, const TDesC8& aJobMediaSheetsCompleted);


		/*
		* @function		GetErrorValue
		* @description	Maps the ImagePrint and device errors.
		* @param		TDesC8 Reference to device returned error code
		* @return		TInt Correspondent ImagePrint error code
		*/
		TInt GetErrorValue(const TDesC8& aError);

		/*
		* @function		PrinterStateUpdated
		* @description	Returns the state of the job after printer state updated 
		* @param		TDesC8& New printer state
		* @return		TUPnPState
		*/
		TUPnPState PrinterStateUpdated( const TDesC8& aEvent );
		
		/*
		* @function		PrinterStateReasonsUpdated
		* @description	Returns the state of the job after printer state reason updated 
		* @param		TDesC8& Reason for the printer state		
		* @return		TUPnPState
		*/
		TUPnPState PrinterStateReasonsUpdated( const TDesC8& aEvent );
		
		/*
		* @function		JobIdListUpdated
		* @description	Returns the state of the job after job ID list updated 
		* @param		TDesC8& List of job ids pending at the printer queue		
		* @return		TUPnPState
		*/
		TUPnPState JobIdListUpdated( const TDesC8& aEvent );

		/*
		* @function		JobEndStateUpdatedL
		* @description	Returns the state of the job after job end state updated 
		* @param		TDesC8& The end state of last printed job. Format: 
		*						<id>,<name>,<originator>,<completed sheets>,<state>
		* @return		TUPnPState
		*/
		TUPnPState JobEndStateUpdatedL( const TDesC8& aEvent );

		/*
		* @function		JobMediaSheetsCompletedUpdated
		* @description	Returns the state of the job after media sheets completed list updated 
		* @param		TDesC8& Number of the printed sheets
		* @return		TUPnPState
		*/
		TUPnPState JobMediaSheetsCompletedUpdated( const TDesC8& aEvent );

		/*
		* @function		JobMediaSheetsCompletedUpdated
		* @description	Returns the state of the job after media sheets completed list updated 
		* @param		TDesC8& ID of the current job
		* @param		TDesC8& Number of the printed sheets
		* @return		TUPnPState
		*/
		TUPnPState JobMediaSheetsCompletedUpdated(const TDesC8& aId, const TDesC8& aEvent );

		/*
		* @function		JobAbortStateUpdatedL
		* @description	Returns the state of the job after job abort state updated 
		* @param		TDesC8& The abort state of last aborted job. Format: 
		*						<id>,<name>,<originator>,<completed sheets>,aborted,<reason>
		* @return		TUPnPState
		*/
		TUPnPState JobAbortStateUpdatedL( const TDesC8& aEvent );

		/*
		* @function		ContentCompleteListUpdated
		* @description	Returns the state of the job after content completed list updated 
		* @param		TDesC8& Number of the sheets completedly received in printer
		* @return		TUPnPState
		*/
		TUPnPState ContentCompleteListUpdated( const TDesC8& aEvent );


	protected:

		/*
		* @function		ConstructL
		* @description	Symbian second phase constructor 
		*/
		void ConstructL();

		/*
		* @function		State
		* @description	updates job's state according to the given state.
		* @param		TUPnPState State enumeration value
		* @return		TUPnPState
		*/
		TUPnPState State(TUPnPState aState);

		/*
		* @function		SetState
		* @description	Updates the stored state of the job 
		* @param		TUPnPState state to update with
		*/
		void SetState(TUPnPState aState);
		
		/*
		* @function		SetJobList
		* @description	Returns and updates the job state according to the list of the jobs pending in printer.
		* @param		TDesC8 Reference to list of jobs
		* @return		TUPnPState
		*/
		TUPnPState SetJobList(const TDesC8& aJobIdList);
		
		/*
		* @function		PendingJobId
		* @description	Returns the position of active job in job list.
		* @param		TDesC8 Reference to list of jobs
		* @return		TInt
		*/
		TInt PendingJobId(const TDesC8& aJobIdList);

		/*
		* @function		SetId
		* @description	Updates the printer returned job id if necessary.
		* @param		TDesC8 Reference to printer returned id
		*/
		void SetId(const TDesC8& aId);

		/*
		* @function		InitErrorValues
		* @description	Initializes the error values stored in map for device vs. ImagePrint errors 
		*/
		void InitErrorValues();

		/*
		* @function		SetReason
		* @description	Sets the reason for job state.
		* @param		TInt Error value for the reason
		*/
		void SetReason(TInt aReason);
		
		/*
		* @function		ParseState
		* @description	Maps the states received from the device to state enumerations.
		* @param		TDesC8 Reference to state string
		* @return		TInt Corresponding state enumeration
		*/
		TUPnPState ParseState(const TDesC8& aState);

		/*
		* @function		ParseJobEndStateL
		* @description	Separates the elements of the job end and abort state for handling and returns
		*				KErrNotFound if the end state is not for the active job
		* @param		TDesC8& The end or abort state of last printed job. Formats: 
		*						<id>,<name>,<originator>,<completed sheets>,<state>
		*						<id>,<name>,<originator>,<completed sheets>,abort,<reason>
		* @return		TInt KErrNotFound if the end state is not for the active job
		*/
		TInt ParseJobEndStateL(const TDesC8& aJobEndState);

	private:

		/*
		* @function		CUPJobState
		* @description	Default constructor.
		*/
		CUPJobState();

	private:
		// @var iActiveId		Id of the active job interested
		TBuf8<KMaxIdLength> iActiveId;

		// @var iId				Id of the job
		TBuf8<KMaxIdLength> iId;

		// @var iSheetsCompleted Number of the completed sheets for the job
		TInt iSheetsCompleted;

		// @var iReason			Reason string of the state
		TInt iReason;

		// @var iState			Overall state of the job
		TUPnPState iState;

		// @var iErrorValues	Error values mapping array
		RArray<TMapError> iErrorValues;

		// @var iEndState		job end state info container
		TUPJobEndState iEndState;
		
		// @var iSheetsToPrint	Number of the pages to print
		TInt iSheetsToPrint;

};
#endif // CUPJOBSTATE_H

//  End of File
