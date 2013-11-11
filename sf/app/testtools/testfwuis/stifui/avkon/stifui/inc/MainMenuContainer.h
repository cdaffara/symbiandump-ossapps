/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file contains CMainMenuContainer class definition.
*
*/

#ifndef CMAINMENUCONTAINER_H
#define CMAINMENUCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <eiklbo.h>

   
// FORWARD DECLARATIONS
class CEikTextListBox;

// CLASS DECLARATION

/**
*  CMainMenuContainer  container control class.
*  
*/
class CMainMenuContainer : public CCoeControl, public MEikListBoxObserver /* MCoeControlObserver */
    {
    public: // Constructors and destructor
        
        /**
        * Symbian OS default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);

        /**
        * Destructor.
        */
        ~CMainMenuContainer();

    public: // New functions
    
        /**
        * Get currently selected items index.
        * @return Current item index.
        */		
		TInt GetActiveLine();   
		
		/**
		 * Makes the iListBox store its focus position to CAppUiAppUi object
		 */
		void SaveActiveLine();

    public: // Functions from base classes
   
        /**
        * Method HandleSelectedListItemL handles valix index.
        * @param aIndex Current item index.
        */
        void HandleSelectedListItemL( TInt aIndex );    

    private: // Functions from base classes

        /**
        * From CoeControl,SizeChanged.
        * Called by framework when the view size is changed.
        */
        void SizeChanged();

        /**
        * From CoeControl,CountComponentControls.
        * @return Number of controls indside this container.
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl,ComponentControl.
        * Gets the specified component of a compound control.
        * @param aIndex The index of the control to get.
        * @return The component control with an index of aIndex.
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
        * From CCoeControl,Draw.
        * Draw a control, called by window server.
        * @param aRect The region of the control to be redrawn. Co-ordinates 
        *              are relative to the control's origin (top left corner).
        */
        void Draw(const TRect& aRect) const;

        /**
        * Handles list box events.
        * @param aListBox The originating list box. 
        * @param aEventType A code for the event. Further information may be
        *                   obtained by accessing the list box itself.
        */
		void HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType);

        /**
        * From CCoeControl,OfferKeyEventL
        * Handles key events.
        * @param aKeyEvent The key event.
        * @param aType The type of key event.
        * @return Indicates whether or not the key event was used 
        *         by this control.
        */
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
		
        
    private: //data
        
		CEikTextListBox*    iListBox;
    };

#endif

// End of File
