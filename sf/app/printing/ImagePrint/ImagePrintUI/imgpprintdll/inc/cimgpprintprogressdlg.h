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


#ifndef CIMGPPRINTPROGRESSDLG_H
#define CIMGPPRINTPROGRESSDLG_H

#include <AknWaitDialog.h>
#include <eikdialg.h>

/**
 *
 *  Modified print progress dialog to enable "branding bitmap" to appear to
 *  the bottom of the progress dialog
 *
 */
class CIMGPPrintProgressDlg
    : public CAknProgressDialog
    {
    
    public:     // Constructor and destructor

        /**
         *  Constructor
         *  
         *  @param aSelfPtr  Pointer to itself. The pointer needs to be
         *                   valid when the dialog is dismissed and must not
	     *                   be on the stack.
         *  @param aVisibilityDelayOff  Cut off the delay at the beginning
         */
        CIMGPPrintProgressDlg( CEikDialog** aSelfPtr, TBool aVisibilityDelayOff );

        /**
         *  Destructor
         */
        virtual ~CIMGPPrintProgressDlg();
    
    public:     // Methods derived from CAknProgressDialog

        TKeyResponse OfferKeyEventL( const TKeyEvent &aKeyEvent, TEventCode aType );

        void PostLayoutDynInitL();

        CEikCaptionedControl* GetFirstLineOnFirstPageOrNull();

		void HandleResourceChange( TInt aType );  
		
	private:
	
	    /**
         *  Handles commands coming to the viwe
         *  @param aCommand The command ID
         */
		                                   
    };

#endif  //  CIMGPPRINTPROGRESSDLG_H

//  End of File
