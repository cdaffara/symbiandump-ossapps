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


#ifndef CIMAGEPRINTSETTINGSVIEW_H
#define CIMAGEPRINTSETTINGSVIEW_H

#include <aknview.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <coeccntx.h>
#include <aknappui.h>

class MPrintSettings;
class MPrintJob;
class CImagePrintSettingsContainer;
class CIMGPPrintDlgManager;
class CImagePrintAppUi;
class MUIMode;

const TUid KImageSettingsViewId = { 2 };

/**
 *
 *  The settings view 
 *
 */
class CImagePrintSettingsView 
    : public CAknView
    {
	public:		// Constructors and destructors

        /**
         *  Two-phase constructor
         *
         *  @param aSettings    Print settings
         *  @param aPrintJob Print job interface to the engine
         *  @return Initialized instance of this class
         */
        static CImagePrintSettingsView* NewL( 
            MPrintSettings* aSettings,
            MPrintJob* aPrintJob,
            CImagePrintAppUi* aAppUi,
            MUIMode* aUIMode );                
        static CImagePrintSettingsView* NewLC( 
            MPrintSettings* aSettings,
            MPrintJob* aPrintJob,
            CImagePrintAppUi* aAppUi,
            MUIMode* aUIMode );

        /**
         *  Destructor
         */
        virtual ~CImagePrintSettingsView();

    protected:  // Constructors and destructors

        /**
         *  Second-phase constructor
         *
         *  @param aSettings    Print settings
         */
        void ConstructL( MPrintSettings* aSettings );

        /**
         *  Constructor
         *  @param aPrintJob Print job interface to the engine
         */
        CImagePrintSettingsView( MPrintJob* aPrintJob, 
                                 CImagePrintAppUi* aAppUi,
                                 MUIMode* aUIMode );

        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

    public:		// Methods derived from CAknView

        /**
         *  Returns this view's ID
         *  @return View's Id 
         */
		TUid Id() const;

        /**
         *  Handles commands coming to the viwe
         *  @param aCommand The command ID
         */
		void HandleCommandL(TInt aCommand);        

	private:	// Methods derived from CAknView

        /**
         *  Activates the view, creates container and other UI controls
         *
         *  @param aPrevViewId  Previous view's ID
         *  @param aCustomMessageId Custom message ID for the view (not used)
         *  @param aCustomMessage   Custom messsage (not used)
         */
		void DoActivateL( 
            const TVwsViewId &aPrevViewId, TUid aCustomMessageId, 
            const TDesC8 &aCustomMessage );

        /**
         *  Called upon view deactivation
         */
		void DoDeactivate();

        /**
        * Handle view foreground event
        */
        void HandleForegroundEventL( TBool aForeground);

    private:    // New methods

        /**
         *  Destroys control container and removes it from the stack
         */
        void DestroyContainer();

        /**
         *  Starts the printing sequence
         */
        void StartPrintingL();
        void HandlePossibleLinkDisconnectionL();	

	private:	// Data

        // Settings view control container
		CImagePrintSettingsContainer* iContainer;

        // Pointer to the print settings interface
        MPrintSettings* iSettings;

        // Pointer to print job interface
        MPrintJob* iPrintJob;

        // Print dlg manager
        CIMGPPrintDlgManager* iPrintDlgManager;

        //AppUI
        CImagePrintAppUi* iAppUi;
        
        // Interface to UI mode
        MUIMode* iUIMode;
    };

#endif // CIMAGEPRINTSETTINGSVIEW_H

//  End of File
