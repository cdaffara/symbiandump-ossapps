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


#ifndef CIMAGEPRINTDOC_H
#define CIMAGEPRINTDOC_H

#include <AknDoc.h>
#include <badesca.h>

class CEikAppUi;
class CImagePrintEngine;
class CIFFactory;
class MPrintSettings;
class MPrintPreview;
class MPrintJob;
class MDiscovery;
class MIdlePrintEventObserver;

/**
 *
 * Document class
 *
 */
class CImagePrintDoc : public CAknDocument
    {
	public:	// Constructors and destructors

		/**
		 * Two phase construction
         * 
         * @Param aApp Application class of this application.
         *
         * @return Initialized instance of CImagePrintDoc
		 */
		static CImagePrintDoc* NewL(CEikApplication& aApp);

		/**
		* Destructor.
		*/
		virtual ~CImagePrintDoc();

	private: // Constructors and destructors

		/**
		* Default constructor.
		*/
		CImagePrintDoc(CEikApplication& aApp);

    private: // Methods derived from CAknDocumnet

       /**
        * Returns AppUi class
        *
        * @return Initialized instance of CEikAppUi
        */
        CEikAppUi* CreateAppUiL();

	public: // New methods

		/**
		* Returns Engine
        *
        * @Return CImagePrintEngine Engine for this application
		*/
		CImagePrintEngine* Engine();

		/**
		*  Returns discovery interface
		*
		*  @return Device discovery interface
		*/
		MDiscovery* Discovery();
		
        /**
        *  Returns print job interface
        *
        *  @return Print job interface
        */
		MPrintJob* PrintJob();

		/**
		*  Returns print preview interface
		*
		*  @return Print preview interface
		*/		
		MPrintPreview* PrintPreview();
		
		/**
		*  Returns settings interface
		*
		*  @return Settings interface
		*/
		MPrintSettings* PrintSettings();

        /**
        *   Initializes engine
        */
        void InitializeEngineL();

        /**
        *   Set selected images to engine.
        *   @param aArray array of printed image, ownership is transferred
        */
        void SetImageArrayL( CDesCArrayFlat* aArray );

        /**
        *   Restarts Image Print Engine
        */
        void RestartEngine();

	private: // Data

	    // Engine for this application
        CImagePrintEngine* iEngine;
    };

#endif // CIMAGEPRINTDOC_H

//  End of File
