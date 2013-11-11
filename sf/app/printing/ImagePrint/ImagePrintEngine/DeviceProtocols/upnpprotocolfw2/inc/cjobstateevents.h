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
* Description:  Declares CJobStateEvents class
*
*/


#ifndef CJOBSTATEEVENTS_H
#define CJOBSTATEEVENTS_H

#include "upconsts.h"

#define KUPnPMaxReasonLen 50
typedef TBuf8<KDefaultRealWidth+KMaxName+KDefaultRealWidth+KMaxFileName+KUPnPMaxReasonLen> TUPnPVariable;

/*
* @class 		CJobStateEvents
* @description	This class contains the state variables related to the job that is active at printer.
*/
NONSHARABLE_CLASS(CJobStateEvents) : public CBase
{
	public:
		enum TJobStateEvent
		{
			ENone = 0,
			EPrinterState,
			EPrinterStateReasons,
			EJobIdList,
			EJobEndState,
			EJobMediaSheetsCompleted,
			EContentCompleteList,
			EJobAbortState
		};
	
	public:

		static CJobStateEvents* NewL();

		static CJobStateEvents* NewLC();

		static CJobStateEvents* NewL(CJobStateEvents &a);

		static CJobStateEvents* NewLC(CJobStateEvents &a);

		static CJobStateEvents* NewL(
				const TDesC8& aPrinterState, const TDesC8& aPrinterStateReasons, const TDesC8& aJobIdList, 
				const TDesC8& aJobEndState,const TDesC8& aJobMediaSheetsCompleted,
				const TDesC8& aContentCompleteList, const TDesC8& aJobAbortState);

		static CJobStateEvents* NewLC(
				const TDesC8& aPrinterState, const TDesC8& aPrinterStateReasons, const TDesC8& aJobIdList, 
				const TDesC8& aJobEndState,const TDesC8& aJobMediaSheetsCompleted,
				const TDesC8& aContentCompleteList, const TDesC8& aJobAbortState);

		void ConstructL(
				const TDesC8& aPrinterState, const TDesC8& aPrinterStateReasons, const TDesC8& aJobIdList, 
				const TDesC8& aJobEndState,const TDesC8& aJobMediaSheetsCompleted,
				const TDesC8& aContentCompleteList, const TDesC8& aJobAbortState);

		void ConstructL(CJobStateEvents &a);
		
		~CJobStateEvents();

		CJobStateEvents(const CJobStateEvents &a);
		
		void Delete();
		
		void InitL();
		
		TBool ChangedEventL(const TDesC8 &aEvent, TJobStateEvent aEnum);

		void ContentCompleteL ( const TDesC8 &aEvent, TBool& aChanged );
		
		void JobAbortStateL ( const TDesC8 &aEvent, TBool& aChanged );
		
		TBool operator==(const CJobStateEvents &a);

		TBool operator!=(const CJobStateEvents &a);

	protected:

		CJobStateEvents();
		

	public:
		HBufC8* iPrinterState;
		HBufC8* iPrinterStateReasons;
		HBufC8* iJobIdList;
		HBufC8* iJobEndState;
		HBufC8* iJobMediaSheetsCompleted;
		HBufC8* iContentCompleteList;
		HBufC8* iJobAbortState;
};

#endif // CJOBSTATEEVENTS_H

//  End of File
