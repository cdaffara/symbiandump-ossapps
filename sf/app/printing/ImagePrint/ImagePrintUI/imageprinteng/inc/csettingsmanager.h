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


#ifndef CSETTINGSMANAGER_H
#define CSETTINGSMANAGER_H

#include <e32base.h>
#include <f32file.h>

#include "mprintsettings.h"

class CImagePrint;
class CRealFactory;
class CRepository;

/**
 *
 *  Settings manager handles and modifies the capabilities
 *
 */
class CSettingsManager
    : public CBase,
      public MPrintSettings
    {
    public:     // Construction and destruction

        /**
         *  Two-phase constructor
         *
         *  @param aFactory     Pointer to factory
         *  @return Initialized instance of the class
         */
        static CSettingsManager* NewL(
            CRealFactory* aFactory );
        static CSettingsManager* NewLC(
            CRealFactory* aFactory );

        /**
         *  Destructor
         */
        ~CSettingsManager();

    private:    // Construction and destruction

        /**
         *  Constructor
         *
         *  @param aFactory     Pointer to factory
         */
        CSettingsManager( CRealFactory* aFactory );

        /**
         *  2nd phase constructor
         */
        void ConstructL();

    public:     // Methods derived from MPrintSettings

        CTemplateSettings* TemplateSettingsL();
        void SetActiveTemplateL( TUint aUid );
        TUint NumOfPagesL();
        TInt NumberOfCopies( RArray<TInt>& aArray );
        TInt NumberOfCopies( TInt& aNumberOfCopies, TInt aIndex );
        TInt SetNumberOfCopiesL( const RArray<TInt>& aArray );
        TInt UpdateNumberOfCopiesToServer();
        TBool IsMMC();
        void SetActivePageL( TUint aActivePage );
        TUint CurActivePageL();
        CPrintSettings* PrinterSettingsL();
        void SetPrinterSettingsL( CPrintSettings& aSettings );
        TInt SetPrinterSettingL( CBaseCapability& aCapab );
        TInt TemplateUid();
        TBool UpdatePageNumberL( TInt aCurrentImage );
        TInt PrinterUid();
        void SetUsbState( TBool aConnected );
        TBool GetUsbState();
        TInt GetDefaultPrinterProtocol();
        TInt SetDefaultPrinterProtocol( TInt aProtocol );
        TInt GetCurrentPrinterProtocol();
        void SetCurrentPrinterProtocol( TInt aProtocol );
        TUint GetCurrentPrinterVendor();
        void SetCurrentPrinterVendor( TUint aVendor );
        TInt GetDefaultPrintID();
        TInt SetDefaultPrinterID( TInt aId );
        HBufC* GetDefaultPrinterNameLC();
        TInt SetDefaultPrinterNameL( const TDesC& aPrinterName );
        HBufC* GetCurrentPrinterName();
        TInt SetCurrentPrinterName( const TDesC& aPrinterName );
        HBufC* GetCurrentPaperSizeTextL();

    public:     // New methods

        /**
         *  Initializes settings after successfully creating a print job
         *
         *  @param aPrinterUid      Printer UID for the capability fetching
         *  @param aMMCUid          MMC printer UID if available
         */
        void InitSettingsL( TInt aPrinterUid, TInt aMMCUid );

        /**
         *  Fetches the correct capability, leaves if cannot find the capapbility
         *
         *  @param aUid     Uid of the fetched capability
         *  @return The capability
         */
        CBaseCapability* FetchCapabilityL( TInt aUid );

    private:    // New methods

        /**
         *  Initializes capabilities
         *
         *  @param aPrinterUid      Printer UID for the capability fetching
         */
        void InitCapabilitiesL( TInt aPrinterUid );

        /**
         *  Initializes template settings
         */
        void InitTemplateSettingsL();

    private:    // Data

        // Pointer to factory and Image Print Server
        CRealFactory* iFactory;

        // Printer and template settings
        CTemplateSettings* iTemplateSettings;
        CPrintSettings* iPrintSettings;

        // Active printer Uid from from Print Server
        TInt iPrinterUid;

        // Current active page
        TUint iActivePage;

        // MMC Uid
        TInt iMMCUid;

        // Images on page is dependent on the current template
        TInt iImagesOnPage;

        // USB connection state
        TBool iUsbConnected;

        // A Central Repository session
        CRepository* iCRSession;

        // printer brand
        TUint iVendor;

        // Current printer protocol
        TInt iCurrentPrinterProtocol;

        // Current printet name
        HBufC* iCurrentPrinterName;

        // Number of copies array
        RArray<TInt> iNumberOfCopiesArray;
        RArray<TInt> iNumberOfCopiesCumulativeArray;

    };

#endif  //  CSETTINGSMANAGER_H

//  End of File
