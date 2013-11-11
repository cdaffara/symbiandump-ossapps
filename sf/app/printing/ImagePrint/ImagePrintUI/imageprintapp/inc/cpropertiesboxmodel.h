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


#ifndef CPROPERTIESBOXMODEL_H
#define CPROPERTIESBOXMODEL_H

#include <e32base.h>
#include <bamdesca.h>
#include <eiklbo.h>

#include "mprintsettings.h"

class CImagePrintSettingsContainer;
class CImagePrintEngine;

const TInt KMaxListName = 128;

/**
 *
 *  List box model that handles the contents of the list box and launches
 *  the pop-up dialogs that are used .
 *
 */
class CPropertiesBoxModel :
    public CBase,
    public MEikListBoxObserver,
    public MDesCArray
    {
    public:         //  Constructors and destructors

        /**
         *  Two-phase constructor
         *
         *  @param aSettings   Print settings
         *  @return Initialized instance of CPropertiesBoxModel
         */
        static CPropertiesBoxModel* NewL( CImagePrintEngine& aEngine,
											MPrintSettings* aSettings,
											CImagePrintSettingsContainer* aContainer );

        static CPropertiesBoxModel* NewLC( CImagePrintEngine& aEngine,
											MPrintSettings* aSettings,
											CImagePrintSettingsContainer* aContainer );

        /**
         *  Destructor
         */
        virtual ~CPropertiesBoxModel();

    private:        //  Constructors and destructors

        /**
         *  Constructor
         *
         *  @param aSettings    Print settings
         */
        CPropertiesBoxModel( CImagePrintEngine& aEngine,
								MPrintSettings* aSettings,
								CImagePrintSettingsContainer* aContainer );

        /**
         *  2nd phase constructor
         */
        void ConstructL();

    public:         //  Methods derived from MEikListBoxObserver

        void HandleListBoxEventL( CEikListBox* aListBox,
                                  TListBoxEvent aEventType );
      
        /**
         *  Returns number of copies.
         */
        TInt NumberOfCopies();
      

    private:        //  Methods derived from MDesCArray

        TInt MdcaCount() const;
        TPtrC MdcaPoint( TInt aIndex ) const;

    private:        //  New methods

        /**
         *  Returns the index to the capability array from list box array
         *
         *  @return Capability array index
         */
        TUint CapabilityArrayIndex( TInt aListBoxIndex ) const;

        /**
         *  Stores current setting to the engine
         */
        void SaveCurrentSelectionL( CBaseCapability &aCapab, CEikListBox* aListBox );

        /**
         * Launches list query for default printer
         *
         * @return wanted protocol
         */
        TInt LaunchDefaultPrinterListL();
        
        /**
         *  Stores number of copies value to the engine
         *  NOTE: Number of copies value is job based value since IP 6.2
         */
        void LaunchNumberOfCopiesDialogL();

        /**
         *  Init print settings 
         */
		void InitSettingsL();

        /**
         *  Sets DefaultPrinterItem for ListBox 
         */
        void SetDefaultPrinterItem() const;

        /**
         *  Sets SetNumberOfCopiesItem for ListBox
         */
        void SetNumberOfCopiesItem() const;

        /**
         *  Sets other capability(template, papersize etc.) for ListBox 
         */
        void SetOtherCapability( const TInt &aIndex ) const;
        
        /**
         *  Sets number of copies value for settings
         */
        void SetNumberOfCopiesValueL();

    private:  // Data

		CImagePrintEngine& iEngine;
        MPrintSettings* iSettingsIF;  // use
        CPrintSettings* iPrintSettings;
        TInt iCount;
        TInt iNumberOfCopiesValue;
        CImagePrintSettingsContainer* iContainer;
        mutable TBuf<KMaxListName> iBuf;
		HBufC* iDefaultPrinter;
		HBufC* iNumberOfCopiesText;
		HBufC* iMMC;
        HBufC* iPictBridge;
    };

#endif  //  CPROPERTIESBOXMODEL_H

//  End of File
