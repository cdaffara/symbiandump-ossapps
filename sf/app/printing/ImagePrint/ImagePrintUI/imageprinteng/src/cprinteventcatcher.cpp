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
* Description:  
*
*/


#include "cprinteventcatcher.h"
#include "crealfactory.h"
#include "mprintpreviewobserver.h"
#include "mprintjobstatus.h"
#include "cprintjobmanager.h"
#include "clog.h"

// CONSTRUCTION
CPrintEventCatcher* CPrintEventCatcher::NewL(
    CIFFactory* aFactory,
    CImagePrintEngine* aEngine )
    {
    CPrintEventCatcher* self = CPrintEventCatcher::NewLC( aFactory, aEngine );
    CleanupStack::Pop();    // self
    
    return self; 
    }

CPrintEventCatcher* CPrintEventCatcher::NewLC(
    CIFFactory* aFactory,
    CImagePrintEngine* aEngine )
    {
    CPrintEventCatcher* self = 
        new ( ELeave ) CPrintEventCatcher( aFactory, aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// Constructor
CPrintEventCatcher::CPrintEventCatcher(
    CIFFactory* aFactory,
    CImagePrintEngine* aEngine ) :
      iFactory( aFactory ),
      iEngine( aEngine )
    {
    }
     
// Destructor
CPrintEventCatcher::~CPrintEventCatcher()
    {
    iObservers.Reset();
    }

// 2nd phase constructor
void CPrintEventCatcher::ConstructL()
    {
    }

// Print progress event
void CPrintEventCatcher::PrintJobProgressEvent( 
    TInt aStatus, 
    TInt aPercentCompletion, 
    TInt aJobStateCode )
    {
    TInt obsCount = iObservers.Count();
    for( TInt i=0; i<obsCount; i++ )
		{
        iObservers[i]->PrintJobProgress( aStatus, aPercentCompletion, 
                                         aJobStateCode );
        }
    }

// Print job error event
void CPrintEventCatcher::PrintJobErrorEvent( 
    TInt aError,
    TInt aErrorStringCode )
    {
    LOG2("CPrintEventCatcher::PrintJobErrorEvent: aError: %d, aErrorStringCode: %d ", 
    	 aError, aErrorStringCode );
    TInt obsCount = iObservers.Count();
    for( TInt i=0; i<obsCount; i++ )
		{
        iObservers[i]->PrintJobError( aError, aErrorStringCode );
        }
    }

// Print job status event ("minor" error)
void CPrintEventCatcher::PrinterStatusEvent( 
    TInt aError,
    TInt aErrorStringCode )
    {
    LOG2("CPrintEventCatcher::PrinterStatusEvent: aError: %d, aErrorStringCode: %d ", 
    	 aError, aErrorStringCode );
    TInt obsCount = iObservers.Count();
    for( TInt i=0; i<obsCount; i++ )
		{
        iObservers[i]->PrinterStatus( aError, aErrorStringCode );
        }
    }

// Receives preview events from the image print server
void CPrintEventCatcher::PreviewImageEvent(
    TInt /*aFsBitmapHandle*/ )
    {
    // Should not be called from engine anymore
    }

// Set preview observer
void CPrintEventCatcher::SetPreviewObserver( 
    MPrintPreviewObserver* aObserver )
    {
    iPreviewObserver = aObserver;
    }

void CPrintEventCatcher::RegisterObserver( MPrintJobStatus* aObserver )
    {
    iObservers.Append( aObserver );
    }

void CPrintEventCatcher::UnRegisterObserver( MPrintJobStatus* aObserver )
    {
    TInt observerPosition = iObservers.Find( aObserver );
    if( observerPosition != KErrNotFound )
        {
        iObservers.Remove( observerPosition );
        }    
    }


// GOING TO GET REMOVED
void CPrintEventCatcher::ShowMessageL( 
    TInt /*aMsgLine1Code*/,     
    TInt /*aMsgLine2Code*/ )
    {
    }

// GOING TO GET REMOVED
TBool CPrintEventCatcher::AskYesNoQuestionL( 
    TInt /*aMsgLine1Code*/,     
    TInt /*aMsgLine2Code*/ )
    {
    return EFalse;
    }

// GOING TO GET REMOVED
const TDesC& CPrintEventCatcher::AskForInputL( 
    TInt /*aMsgLine1Code*/,     
    TInt /*aMsgLine2Code*/ )
    {
    return KNullDesC;
    }

//  End of File
