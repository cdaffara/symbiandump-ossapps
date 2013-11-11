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


#ifndef CIMAGEPRINTAPPUI_H
#define CIMAGEPRINTAPPUI_H

#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknappui.h>
#include <aknViewAppUi.h>
#include <ErrorUI.h>
#include <aknnotewrappers.h>
#include <AknWaitNoteWrapper.h>
#include <AknProgressDialog.h>
#include <e32property.h>
#include <data_caging_path_literals.hrh>

#include "midleprinteventobserver.h"
#include "rimageprintclient.h"
#include "muimodeobserver.h"

class CAknNavigationDecorator;
class CDiscoveryDlgManager;
class CImagePrintDoc;
class CIMGPPrintDlgManager;
class MPrintSettings;
class MPrintPreview;
class CAknStaticNoteDialog;
class MUIMode;

const TInt KPrinterNameID( 100 );

//  CLASS DEFINITION
/**
 *
 *  AppUi class
 *
 */
class CImagePrintAppUi
    : public CAknViewAppUi,
      public MUIModeObserver
     {

	public:		// Constructors and destructors

        /**
         *  Second phase constructor
         */
		void ConstructL();

        /**
         *  Destructor
         */
		virtual ~CImagePrintAppUi();

	private:	// Methods derived from CAknViewAppUi base class

		void HandleCommandL( TInt aCommand );

    public: // Methods derived from MUIModeObserver
        void ChangeViewL( TInt aView );
        void ExitApplication();
        
        // Set text to navipane.
        void SetNavipaneTextL( const TInt aCopiesOfImage, HBufC* aPaperSize = NULL );
        
    public:    // New methods

        /**
         * Method is used to forward the selected images to engine.
         *
         * @param aFileArray, array that contains the selected images.
         */
        void SetImageArrayL( CDesCArrayFlat* aFileArray );

        /**
         * Method returns the Dosument class od this application
         *
         * @return CimagePrintDoc document class
         */
        CImagePrintDoc* GetDocument();

        /**
         * Method returns the image array
         *
         * @return CDesCArrayFlat array that contains the selected images
         */
        CDesCArrayFlat* GetImageArray();

        /**
         * Launches application help
         *
         * @param aHelptext help text context
         */
        void LaunchHelpApplicationL( const TDesC& aHelpText );

        /**
         * Gets the AIW state (iAiwState)
         *
         */
        TInt GetAiwState();

        /**
         * Returns UID of the application
         *
         * @return UID
         */
         TInt GetApplicationUID();

        /**
         * Returns UID for the settings view
         *
         * @return UID
         */
         TUid GetSettingsViewUid();

         /**
         * Returns AIW state.
         *
         * ETrue = Application is launched via AIW
         * EFalse = Application is launched from active idle.
         *
         * @return AIW state.
         */
        TBool AiwState();

        /**
         * Check orientation mode
         *
         * @return ETrue of orientation is portrait, otherwise EFalse
         */
        TBool OrientationPortrait();

        /**
         * Check whether the operating resolution is QVGA
         *
         * @return ETrue if operating resolution is QVGA, otherwise EFalse
         */
        TBool QVGAMode();

        /**
         * Gets the album call index
         *
         * @return album call index
         */
        TInt GetAlbumCall();

        /**
         * Sets the album call index
         *
         * @param aAlbumCall album call index
         */
        void SetAlbumCall(TInt aAlbumCall);

        /**
         * Sets the validity status of images
         *
         * @param aStatus ETrue if images are valid, otherwise EFalse
         */
        void SetValidImagesStatus( TBool aStatus );

        /**
         * callback for printer search in aiw mode
         *
         * @param aObj callback object
         *
         * @return status of the call
         */
        static TInt SearchDefaultPrinterCbL( TAny* aObj );

    private: // New methods

        CDesCArrayFlat* GetImagesFromAiwProviderL();

    private:    // Data

        // Used for navigation pane text
        CAknNavigationDecorator* iNaviDecorator;

        // Offset for engine's resource file
        TInt iResourceOffset;

        //Document class
        CImagePrintDoc* iDoc;

        CIMGPPrintDlgManager* iPrintDlgManager; // use

        // Printed image files
        CDesCArrayFlat* iFiles;

        // UID of the selected printer
        TInt iPrinterUID;

        // Information about the state of usb discovery
        TBool iUSBDiscoveryOngoing;

        // Is usb printer connected to phone
        TBool iUSBPrinter;

        // USB connect/disconnect note
        CAknProgressDialog* iUSBConnectionWaitNote;

        // Is usb device connected
        TBool iUsbConnected;

        // Is USB printer selected
        TBool iUSBPrinterSelected;

        // For AIW calls;
        TInt iAiwState;

        RProperty iAiwCall;

        TBool iApplicationStarting;

        TBool iBTDiscovery;

		RImagePrintClient iImagePrintClient;

        TInt iAlbumCall;

        TBool iNotifyDone;

        // ETrue when exiting the application in AIW mode
        TBool iExiting;

        TBool iFullDiscovery;

        TBool iDisconnectOnce;

        TBool iUsbWanted;

        TBool iValidImages;

        MUIMode* iUIMode;

        // idle object for performing search in aiw mode, owned
        CIdle* iIdleSearch;

    };

#endif  // CIMAGEPRINTAPPUI_H

//  End of File
