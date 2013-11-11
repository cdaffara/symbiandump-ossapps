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


#ifndef CIMAGEPRINTSETTINGSCONTAINER_H
#define CIMAGEPRINTSETTINGSCONTAINER_H

#include <coecntrl.h>
#include <aknlists.h>
#include <aknnotewrappers.h>
#include <AknWaitNoteWrapper.h>
#include <AknProgressDialog.h>

#include "midleprinteventobserver.h"

class MObjectProvider;
class MPrintSettings;
class CPropertiesBoxModel;
class CImagePrintSettingsItemList;
class CImagePrintAppUi;
class MUIMode;

/**
 *
 *  Image print settings container
 *
 */
class CImagePrintSettingsContainer
    : public CCoeControl
    {

    public:     // Constructors and destructor

        /**
         *  Two-phase constructor
         *  @param aSettings  Print settings interface
         *  @param aProvider  Object provider for setting the MOP
         *  @paramt aClientRect  The screen area
         *  @return Initialized instance of the class
         */
        static CImagePrintSettingsContainer* NewL(
                                MPrintSettings* aSettings,
                                MObjectProvider* aProvider,
                                TRect aClientRect,
                                CImagePrintAppUi* aAppUi,
                                MUIMode* aUIMode );

        /**
         *  Destructor
         */
        virtual ~CImagePrintSettingsContainer();

    protected:  // Constructors and destructor

        /**
         *  Constructor
         *  MPrintSettings* aSettings   Print settings interface
         */
        CImagePrintSettingsContainer( MPrintSettings* aSettings,
                                      CImagePrintAppUi* aAppUi,
                                      MUIMode* aUIMode );

        /**
         *  2nd phase constructor
         *  MObjectProvider* aProvider  Object provider for setting the MOP
         *  TRect aClientRect           The screen area
         */
        void ConstructL( MObjectProvider* aProvider, const TRect& aClientRect );

    protected:    // Methods derived from CCoeControl

        /**
         *  Returns the amount of owned UI controls
         *
         *  @return Number of components
         */
        TInt CountComponentControls() const;

        /**
         *  Gets the specified component of a compound control.
         *
         *  @param aIndex The index of the control to get.
         *  @return The component control with an index of aIndex
         */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
         *  Draws the control to the device screen
         *
         *  @param aRect Modified area (not used)
         */
        void Draw( const TRect& aRect ) const;

		void SizeChanged();
        void HandleResourceChange( TInt aType );

    public:     // New methods

        /**
         *  Starts editing the active item
         */
        void EditItemL();

		/**
		 * Starts the device discovery
		 *
		 * @param aProtocol protocol for which the discovery is done
		 * @param aHidden whether discovery is visible (dialog) or hidden
		 */
		void DoDiscoveryL( TInt aProtocol, TBool aHidden = EFalse );

		/**
		 * Changes the text in status pane
		 *
		 * @param aName new text for status pane
		 */        
	
		void ChangeStatusPaneTextL( const TInt aCopiesOfImage );
		
		/**
		 * Updates the size of settings list
		 *
		 * @param aRect new size for list
		 */
        void UpdateListSize( TRect aRect );

        /**
         * Tells ListBox to update itself as its' size has
         * possible changed
         */
        void UpdateListBox( );

    protected:

        // Settings
        MPrintSettings* iPrintSettings;

        // List box model
        CPropertiesBoxModel* iModel;

        // Item list
        CAknSettingStyleListBox* iListBox;

        // AppUi
        CImagePrintAppUi* iAppUi;

        // USB connect note
        CAknProgressDialog* iUSBConnectionWaitNote;

        MUIMode* iUIMode;
	};

#endif // CIMAGEPRINTSETTINGSCONTAINER_H

//  End of File
