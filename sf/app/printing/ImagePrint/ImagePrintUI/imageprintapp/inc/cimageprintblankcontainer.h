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


#ifndef CIMAGEPRINTBLANKCONTAINER_H
#define CIMAGEPRINTBLANKCONTAINER_H

#include <coecntrl.h>

class CAknViewAppUi;
class CAknsListBoxBackgroundControlContext;

//  CLASS DEFINITION
/**
 * CImagePrintBlankContainer is used to show white background before
 * the CImagePrintView is launched
 */
class CImagePrintBlankContainer
    : public CCoeControl
	{

    public: // Constructors and destructor

        /**
         *  Two-phase constructor
         *
         *  @param aRect  ClientRect of the owner control
         *
         *  @return Initialized instance of CImagePrintBlankContainer
         */        
        static CImagePrintBlankContainer* NewL( TRect& aRect );
		static CImagePrintBlankContainer* NewLC( TRect& aRect );
		
	    /**
         * Destructor
         */
        virtual ~CImagePrintBlankContainer();
	
	public:  //Scalable UI
	    void HandleResourceChange( TInt aType );
	
	protected: // 2:nd phase constructor and default constructor

		/**
         *  2:nd phase constructor
         *
         *  @param aRect  ClientRect of the owner control
         */
        void ConstructL( const TRect& aRect );

		CImagePrintBlankContainer();
    
    private: // Methods derived from CCoeControl
        
        void Draw( const TRect& aRect ) const;		
        	
    private:    // Data
        
        // Skin control context for drawing the background bitmap
        CAknsListBoxBackgroundControlContext* iSkinControlContext;
	};

#endif   // CIMAGEPRINTBLANKCONTAINER_H

//  End of File
