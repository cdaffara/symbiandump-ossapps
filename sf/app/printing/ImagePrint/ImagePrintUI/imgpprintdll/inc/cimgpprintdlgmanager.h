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


#ifndef CIMGPPRINTDLGMANAGER_H
#define CIMGPPRINTDLGMANAGER_H

#include <e32std.h>
#include <e32base.h>
#include <AknProgressDialog.h>

#include "mprintjobobserver.h"
#include "mprintjob.h"

class CAknProgressDialog;
class CAknWaitDialog;
class CIMGPPrintUtils;
class MPrintingObserver;

/**
 *
 *   Print progress dialog manager - handles the user interface of the printing
 *
 */
class CIMGPPrintDlgManager
    : public CBase,
      public MPrintJobObserver,
      public MProgressDialogCallback

    {
    public:     // Constructors and destructors

        /**
         *  Two-phase constructor
         *
         *  @param aPrintJob Pointer to print job interface   
         *  @param aNumOfPages Number of printed pages
         *  @param aMMCPrinting Flag to índicate if "printing" to MMC
         *  @param aVendor printer brand
         *  @param aPrintUtils pointer to CIMGPPrintUtils class
         *  @param aPrintingObserver pointer to MPrintingObserver interface
         *
         *  @return Initialized instance of print dialog manager
         */
        IMPORT_C static CIMGPPrintDlgManager* NewL( 
        	TInt aNoc,
            MPrintJob* aPrintJob, 
            TUint aNumOfPages, 
            TBool aMMCPrinting, 
            TUint aVendor,
            CIMGPPrintUtils* aPrintUtils = NULL, 
            MPrintingObserver* aPrintingObserver = NULL );

        /**
         *  Two-phase constructor
         *
         *  @param aPrintJob Pointer to print job interface   
         *  @param aNumOfPages Number of printed pages
         *  @param aMMCPrinting Flag to índicate if "printing" to MMC 
         *  @param aVendor printer brand       
         *  @param aPrintUtils pointer to CIMGPPrintUtils class
         *  @param aPrintingObserver pointer to MPrintingObserver interface
         *
         *  @return Initialized instance of print dialog manager
         */
        static CIMGPPrintDlgManager* NewLC( 
        	TInt aNoc,
            MPrintJob* aPrintJob, 
            TUint aNumOfPages, 
            TBool aMMCPrinting, 
            TUint aVendor,
            CIMGPPrintUtils* aPrintUtils = NULL, 
            MPrintingObserver* aPrintingObserver = NULL  );
        
        /**
         *  Destructor
         */
        virtual ~CIMGPPrintDlgManager();

    protected: // Constructors and destructors

        /**
         *  Default constructor
         *
         *  @param aPrintJob Pointer to print job interface
         *  @param aNumOfPages Number of printed pages
         *  @param aMMCPrinting Flag to índicate if "printing" to MMC   
         *  @param aVendor printer brand     
         *  @param aPrintUtils pointer to CIMGPPrintUtils class
         *  @param aPrintingObserver pointer to MPrintingObserver interface
         *
         */
        CIMGPPrintDlgManager( TInt aNoc,
        				  MPrintJob* aPrintJob, 
                          TUint aNumOfPages, 
                          TBool aMMCPrinting, 
                          TUint	aVendor,
                          CIMGPPrintUtils* aPrintUtils = NULL, 
            			  MPrintingObserver* aPrintingObserver = NULL );
            			  
    private: // Constructor
        void ConstructL();

            
    protected: // Methods derived from MPrintJobObserver

        void PrintProgress( TUint aComplete );
        void PrintProgressL( TUint aComplete );
        void JobFinished();
        void JobError( TInt aErrCode, TInt aErrorStringCode );
        void JobStatusEvent ( TInt aErrCode, TInt aErrorStringCode );

    protected: // Methods derived from MProgressDialogCallback

        void DialogDismissedL( TInt aButtonId );

    public: // New methods

        /**
         *  Executes the printing, displays print progress dialog
         */
        IMPORT_C void StartPrintingL();

    protected: // New methods

        /**
         *  Shows the MMC printing completed dialog
         */
        void ShowMMCPrintingCompletedDlgL();

		/**
		* Pop up cancellation wait note
		*/
		void ShowCancelWaitNoteL();
        
        /**
         * prepares the progress dialog for showing
         */
        void PrepareProgressDialogL();
        
    private:

    protected:  //  Data

        // Pointer to the print job instance
        MPrintJob* iPrintJob;

        // Progress dialog
        CAknProgressDialog* iDialog;

		// Cancelling dialog
		CAknWaitDialog* iCancellingDialog;

		// Cancelling status
		TBool iCancelling;
		
		// Printing cancelled
		TBool iPrintingCancelled;

        // Number of pages
        TUint iNumOfPages;

        // Is printing from MMC or not (different text displayed)
        TBool iMMCPrinting;

        // printer brand
        TUint iVendor;

        // Flag to tell if print was cancelled
        TBool iCancelled;

        // Used in aiw printing
        CIMGPPrintUtils* iPrintUtils;

		// Used in preview printing
		MPrintingObserver* iPrintingObserver; // not owned
		
		// Used to observe is this first time job done note is displayed / 1 job. 
		TBool iFirstNote;
		
		// Number of copies to MMC print job done note.
		TInt iNoc;
	
    };

#endif  // CIMGPPRINTDLGMANAGER_H

//  End of File
