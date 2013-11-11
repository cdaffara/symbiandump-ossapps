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


#ifndef CIMGPVIEWEMPTYVIEW_H
#define CIMGPVIEWEMPTYVIEW_H

#include <aknview.h>
#include <e32std.h>

class CImagePrintBlankContainer;

const TUid KIMGPViewEmptyViewId = { 4 };

/**
 *
 *  Empty view
 *
 */
class CIMGPViewEmptyView : public CAknView
    {
	public:	    // Constructors and destructors

        /**
         *  Standard two-phase constructors
         *
         *  @param aAppUi Pointer to Imageprint AppUi class
         *  @param aPrintJob Print job interface to the engine
         *  @return Initialized instance of the class
         */
        static CIMGPViewEmptyView* NewL();                                       
                                       
        /**
         *  Destructor
         */
		virtual ~CIMGPViewEmptyView();

	protected:

        /**
         *  Constructor
         *
         */
        CIMGPViewEmptyView();
                         		
        /**
         *  2nd-phase constructor
         */
		void ConstructL();
		
	public:		// Methods derived from CAknView

		TUid Id() const;
		void HandleCommandL( TInt aCommand );
	private:	// Methods derived from CAknView

		void DoActivateL ( const TVwsViewId &aPrevViewId, 
                           TUid aCustomMessageId, 
                           const TDesC8 &aCustomMessage );
		void DoDeactivate();

    protected:
        void SizeChanged();
	    void HandleForegroundEventL( TBool aForeground );
	private:
		CImagePrintBlankContainer* iContainer;
    };
#endif // CIMGPVIEWEMPTYVIEW_H

//  End of File
