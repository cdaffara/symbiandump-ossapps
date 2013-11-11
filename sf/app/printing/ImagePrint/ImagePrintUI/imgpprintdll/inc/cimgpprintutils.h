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


#ifndef CIMGPPRINTUTILS_H
#define CIMGPPRINTUTILS_H

#include <eikapp.h>
#include <e32std.h>
#include <aknappui.h>
#include <data_caging_path_literals.hrh>

#include "midleprinteventobserver.h"
#include "rimageprintclient.h"
#include "maiwobserver.h"
#include "mprotocolobserver.h"
#include "mprintjobstatus.h"

class CAknNavigationDecorator;
class CImagePrintDoc;
class CIMGPPrintDlgManager;
class MPrintSettings;
class CImagePrintEngine;
class MIdlePrintEventObserver;
class CUIProtocolController;

/**
 *
 *  AppUi class
 *
 */
class CIMGPPrintUtils
    : public CBase,
      public MProtocolObserver,
      public MPrintJobStatus
     {

	public:		// Constructors and destructors


         /**
         *  Second phase constructor
         */
        IMPORT_C static CIMGPPrintUtils* NewL( MAiwObserver* aAiwObserver );
        static CIMGPPrintUtils* NewLC( MAiwObserver* aAiwObserver );
        void ConstructL( MAiwObserver* aAiwObserver );
		virtual ~CIMGPPrintUtils();

        void DialogDismissedL(TInt /*aButtonId*/);

public:		// from MPrintJobStatus
        void PrintJobProgress( TInt aStatus, 
        					   TInt aPercentCompletion, 
            				   TInt aJobStateCode );
        void PrintJobError( TInt aError, TInt aErrorStringCode );
        void PrinterStatus( TInt aError, TInt aErrorStringCode );
        
    public:    // New methods


        /**
         * Starts printing process
         */
        IMPORT_C void StartPrintingL( CDesCArrayFlat* aImages );

        /**
         * Starts the BrowserEngine for selecting images
         */
        void StartBrowser();

        /**
         * Method is used to forward the selected images to engine.
         *
         * @param aFileArray, array that contains the selected images.
         */
        void SetImageArrayL( CDesCArrayFlat* aFileArray );

         /**
         * Method is used to set the printer UID value in the UI side.
         *
         * @param aPrinterUID printer uid.
         */
        void SetPrinterUID( TInt aPrinterUID );

        /**
         * Method is used to get the printer UID value in the UI side.
         *
         * @return printer uid.
         */
        TInt GetPrinterUID();

		CImagePrintEngine& Engine() const;

        /**
         * Called in aiw printing when printing is completed
         */
         void JobFinished();

		/**
		 * Called when initial printing didn't succeed because default
		 * printer was not available. Does printing after full discovery
		 */
		void RestartPrintingL();

	public: // static helpers
		IMPORT_C static TBool CheckMIMEType( const TDesC8& aMimeType,
			const TFileName& aFileName );

		IMPORT_C static void GetDataFilePathL( TFileName& aFileName );
		
   public: // Methods derived from MProtocolObserver
        void UsbCableDisconnected();
       
    private: // New methods

        /**
        * Print
        */
        void PrintL();
        
        static TInt IdleCallbackL( TAny* aObj );

    private:    // Data

        CIMGPPrintDlgManager* iPrintDlgManager; // use

        // UID of the selected printer
        TInt iPrinterUID;        
        // Application engine
        CImagePrintEngine* iEngine;

        TInt iResourceOffset;
        TInt iResourceOffset2;

        // Used in aiw printing
        MAiwObserver* iAiwObserver;
        // Interface for settings
        MPrintSettings* iSettings;
        // Protocol controller
        CUIProtocolController* iProtocolController;
        
        CIdle* iIdle;
	};

#endif // CIMGPPRINTUTILS_H

//  End of File
