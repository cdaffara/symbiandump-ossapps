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
* Description: This file contains CTestSetBaseMenuContainer 
* class declaration.
*
*/

#ifndef CTestSetBaseMenuContainer_H
#define CTestSetBaseMenuContainer_H

// INCLUDES
#include <coecntrl.h>
#include <eiklbo.h>

   
// FORWARD DECLARATIONS
//class CEikLabel;        // for example labels
class CEikTextListBox;

// CLASS DECLARATION

/**
*  CTestSetBaseMenuContainer  container control class.
*  
*/
class CTestSetBaseMenuContainer : public CCoeControl
    {
    public: // Constructors and destructor
        
        /**
        * Symbian OS default constructor.
        * @param aRect Frame rectangle for container.
        * @return Observer for list box.
        */
        void ConstructL(const TRect& aRect, MEikListBoxObserver* aListBoxObserver);

        /**
        * Destructor.
        */
        ~CTestSetBaseMenuContainer();

    public: // New functions

        /**
        * Get currently selected items index.
        * @return Current item index.
        */		
		TInt GetActiveLine();    
		
        /**
         * Stores current position of focus of iListBox to the CAppUiAppUi object
         */
		void SaveActiveLine();
    
    public: // Functions from base classes
     
        /**
        * Method HandleSelectedListItemL handles valix index.
        * @param        TInt aIndex
        * @return       nothing
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
        * Gets a count of the component controls of this list box control.
        * This information is used for DrawNow().
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl,ComponentControl.
        * Gets a pointer to the specified component control.
        * @param aIndex Index of the component control to look up.
        * @return The control at aIndex.
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

#endif // CTestSetBaseMenuContainer_H

// End of File
