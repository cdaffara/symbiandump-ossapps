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


#ifndef CPRINTJOBMANAGER_H
#define CPRINTJOBMANAGER_H

#include <e32std.h>
#include <e32base.h>

#include "mprintjob.h"
#include "mprintjobstatus.h"

class MPrintJobObserver;
class CImagePrint;
class CImagePrintEngine;
class CRealFactory;

/**
 *
 *  Print job manager - handels the creation of print jobs and the
 *  printing process
 *
 */
class CPrintJobManager
    : public CBase,
      public MPrintJob,
      public MPrintJobStatus
    {
    public:     // Construction and destruction

        /**
         *  Two phase constructor
         *
         *  @param aFactory     Factory class
         *  @param aDLLEngine   DLL engine
         *
         *  @return Initialized instance
         */
        static CPrintJobManager* NewL( CRealFactory* aFactory,
                            CImagePrintEngine* aDLLEngine );
        static CPrintJobManager* NewLC( CRealFactory* aFactory,
                            CImagePrintEngine* aDLLEngine );

        /**
         *  Destructor
         */
        virtual ~CPrintJobManager();

    private:    // Construction and destruction

        /**
         *  Constuctor
         *
         *  @param aFactory     Factory class
         *  @param aDLLEngine   DLL engine
         */
        CPrintJobManager( CRealFactory* aFactory,
            CImagePrintEngine* aDLLEngine );
        
        /**
         *  2nd phase constructor
         */
        void ConstructL();

    public:     // Methods derived from MPrintJob

        void PrintL( MPrintJobObserver* aObserver );
        void CancelL();
		void GetPrintJobL( RPointerArray<TDesC>& aImages );

    public:     // Methods derived from MPrintJobStatus

        void PrintJobProgress( TInt aStatus, TInt aPercentCompletion,
                               TInt aJobStateCode );
        void PrintJobError( TInt aError, TInt aErrorStringCode );
        void PrinterStatus( TInt aError, TInt aErrorStringCode );

    public:     // New methods

        /**
         *  Creates the print job but does not submit it
         *
         *  @param aPrinterId   The ID of the printer which will be used
         */
        TInt CreatePrintJobL( TInt aPrinterId );

        /**
         *  Returns if the printing has been activated
         *  
         *  @return state of printing
         */
        TBool IsPrinting() const;

    private:    // Enumeration

        // Print progress status        
        enum TPrintProgressStatus
            {
            EActive,
            ECompleted
            };
    
    private:    // Data

        // Instance of the factory class
        CRealFactory* iFactory;

        // Main class of this DLL
        CImagePrintEngine* iDLLEngine;

        // Print job observer
        MPrintJobObserver* iObserver;
        
        // Synchronous wait system for cancellation
        CActiveSchedulerWait iWait;

        // Status flags
        TBool iPrintingOnGoing;
        TBool iCancelling;

        // Chosen printer
        TInt iPrinterUid;
        
        // Images from current job
        RPointerArray<HBufC> iImages;
    };

#endif  //  CPRINTJOBMANAGER_H

//  End of File
