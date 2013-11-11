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


#ifndef CPRINTEVENTCATCHER_H
#define CPRINTEVENTCATCHER_H

#include <e32std.h>
#include <e32base.h>

#include "imageprint.h"
#include "mprinteventregisterif.h"

class CIFFactory;
class CImagePrintEngine;
class MPrintPreviewObserver;
class MPrintJobStatus;

// CLASS DEFINITION
/**
 *
 *  Image Print Server passes the notifications via single interface,
 *  which is implemented by CPrintEventCatcher. It receives the events
 *  and forwards them to the correct instances inside ImagePrintEngine.
 *
 */
class CPrintEventCatcher :
    public CBase,
    public MPrintEventObserver,
    public MPrintEventRegisterIF
    {
    public:     // Construction and destruction

        /**
         *  Two phase constructor
         *
         *  @param aFactory The factory
         *  @param aEngine  The image print engine
         *  @return Initialized instance of the engine
         */
        static CPrintEventCatcher* NewL(
            CIFFactory* aFactory, CImagePrintEngine* aEngine );
        static CPrintEventCatcher* NewLC(
            CIFFactory* aFactory, CImagePrintEngine* aEngine );

        /**
         *  Destructor
         */
        virtual ~CPrintEventCatcher();

    private:    // Construction and destruction

        /**
         *  Constructor
         *
         *  @param aFactory The factory
         *  @param aEngine  The image print engine
         */
        CPrintEventCatcher( CIFFactory* aFactory, CImagePrintEngine* aEngine );

        /**
         *  2nd phase constructor
         */
        void ConstructL();

    public:     // Methods derived from MPrintEventObserver

        void PrintJobProgressEvent( TInt aStatus, TInt aPercentCompletion,
            TInt aJobStateCode );
	    void PrintJobErrorEvent( TInt aError, TInt aErrorStringCode );
	    void PrinterStatusEvent( TInt aError, TInt aErrorStringCode );
	    void PreviewImageEvent( TInt aFsBitmapHandle );
	    void ShowMessageL( TInt aMsgLine1Code, TInt aMsgLine2Code );
	    TBool AskYesNoQuestionL( TInt aMsgLine1Code, TInt aMsgLine2Code );
	    const TDesC& AskForInputL( TInt aMsgLine1Code, TInt aMsgLine2Code );

	public: 	// from MPrintEventRegisterIF
	
        void RegisterObserver( MPrintJobStatus* aObserver );
        void UnRegisterObserver( MPrintJobStatus* aObserver );

    public:     // New methods

        /**
         *  Sets the new preview observer, which will receive the
         *  print preview rendering events
         *
         *  @param aObserver    Print preview observer
         */
        void SetPreviewObserver( MPrintPreviewObserver* aObserver );

    private:    // Data

        CIFFactory* iFactory;
        CImagePrintEngine* iEngine;

        // Preview observer
        MPrintPreviewObserver* iPreviewObserver;

        // Print job observer
        RPointerArray<MPrintJobStatus> iObservers;
    };


#endif  // CPRINTEVENTCATCHER_H

//  End of File
