/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MUIMODEOBSERVER_H
#define MUIMODEOBSERVER_H

#include <e32std.h>

/**
 *
 *  Observer class that informs AppUi class to change view, 
 *  exit application etc.
 *
 */
class MUIModeObserver
    {
    public:
    
       // available views
       enum TView
            {
            ImageSelectionView,
            PreviewView,
            SettingsView
            };
        
        /**
         * Changes the view to the given one
         *
         * @param aView the view to be changed to
         */
        virtual void ChangeViewL( TInt aView ) = 0;

		/**
		 * Exits the application
		 */
        virtual void ExitApplication() = 0;
        
        /**
         * Sets the text for navipane
         *
         * @param aCopiesOfImage image count for navepane
         * @param aPaperSize paper size text for navipane
         */      
        virtual void SetNavipaneTextL( const TInt aCopiesOfImage, HBufC* aPaperSize = NULL ) = 0;
        
    };

#endif  //  MUIMODEOBSERVER_H

//  End of File
