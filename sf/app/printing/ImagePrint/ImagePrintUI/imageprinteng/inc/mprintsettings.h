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


#ifndef MPRINTSETTINGS_H
#define MPRINTSETTINGS_H

#include <e32def.h>
#include <e32std.h>

class CTemplateSettings;
class CPrintSettings;
class CBaseCapability;

/**
 *
 * Interface to use the device discovery functionality of Image Print Engine
 *
 */
class MPrintSettings
    {
    public:     // Abstract methods

        /**
         *  Fetches the available template icons and UIDs associated to
         *  the selected. Leaves on error.
         *  Synchonours
         *
         *  @return The current settings, ownership is transferred
         */
        virtual CTemplateSettings* TemplateSettingsL() = 0;

        /**
         *  Sets the active UID. If this succeeds, the preview bitmap
         *  needs to be updated on UI. Also the amount of pages need
         *  to be queried again.
         *  Synchronous
         *
         *  @param aUid Uid of the activated template
         */
        virtual void SetActiveTemplateL( TUint aUid ) = 0;

        /**
         *  Returns the number of pages with the current template/image
         *  selection.
         *  Synchronous
         *
         *  @return Number of pages
         */
        virtual TUint NumOfPagesL() = 0;

        /**
         *  Returns the number of copies value
         *  Synchronous
         *
         *  @return Number of Copies
         */
        virtual TInt NumberOfCopies( RArray<TInt>& aArray ) = 0;

        /**
         *  Returns the number of copies value
         *  Synchronous
         *
         *  @return Number of Copies
         */
        virtual TInt NumberOfCopies( TInt& aNumberOfCopies, TInt aIndex ) = 0;

        /**
         *  Sets the number of copies value for each image
         *  Synchronous
         */
        virtual TInt SetNumberOfCopiesL( const RArray<TInt>& aArray ) = 0;

        virtual TInt UpdateNumberOfCopiesToServer() = 0;

        /**
         *  Checks if the user has selected MMC printing
         *  Synchronous
         *
         *  @return Boolean value to match
         */
        virtual TBool IsMMC() = 0;

        /**
         *  Sets the active preview page on display. If this succeeds, the
         *  preview bitmap needs to be updated on UI,
         *  Synchronous
         *
         *  @param aActivePage New active page
         */
        virtual void SetActivePageL( TUint aActivePage ) = 0;

        /**
         *  Returns current active page
         *  Synchronous
         *
         *  @return Current active page
         */
        virtual TUint CurActivePageL() = 0;

        /**
         *  Fetch printer capabilities and current settings
         *  Synchronous
         *
         *  @return Printer's capabilities, ownership changes
         */
        virtual CPrintSettings* PrinterSettingsL() = 0;

        /**
         *  Sets new settings for the print job
         *  Synchronous
         *
         *  @param  aSettings   New set of settings
         */
        virtual void SetPrinterSettingsL( CPrintSettings& aSettings ) = 0;

        /**
         *  Sets a single setting for the print job
         *  Synchronous
         *
         *  @param aCapab   Capability to be changed
         *  @return If return value == 0 then new settings should be
         *          fetched using PrinterSettingsL() and the UI must
         *          be updated
         */
        virtual TInt SetPrinterSettingL( CBaseCapability& aCapab ) = 0;

       /**
        * Returns the UID of the currently selected template
        *
        * @return Tint UID
        */
        virtual TInt TemplateUid() = 0;

        // Called when template OR current image is changed
        virtual TBool UpdatePageNumberL( TInt aCurrentImage ) = 0;
        virtual TInt PrinterUid() = 0;

        /**
         * Sets the current usb connection state
         *
         * @param aConnected   Connection state
         */
        virtual void SetUsbState( TBool aConnected ) = 0;

        /**
         * Gets the current usb connection state
         *
         * @return  Connection state
         */
        virtual TBool GetUsbState() = 0;

        /**
          *  Default printer protocol getter
          *  @return protocol number or KErrNotFound
          */
        virtual TInt GetDefaultPrinterProtocol() = 0;

         /**
          *  Default printer protocol setter
          *  @param aProtocol protocol number
          *  @return error code, KErrNone if OK
          */
        virtual TInt SetDefaultPrinterProtocol( TInt aProtocol ) = 0;

         /**
          *  Current printer protocol getter
          *  @return protocol number or KErrNotFound
          */
        virtual TInt GetCurrentPrinterProtocol() = 0;

         /**
          *  Current printer protocol setter
          *  @param aProtocol protocol number
          *  @return error code, KErrNone if OK
          */
        virtual void SetCurrentPrinterProtocol( TInt aProtocol ) = 0;

         /**
          *  Current printer brand getter
          *  @return current printer brand id
          */
        virtual TUint GetCurrentPrinterVendor() = 0;

         /**
          *  Current printer brand setter
          *  @param aVendor printer brand
          */
        virtual void SetCurrentPrinterVendor( TUint aVendor ) = 0;

         /**
          *  Default printer id getter
          *  @return printer id or KErrNotFound
          */
        virtual TInt GetDefaultPrintID() = 0;

        /**
          *  Default printer protocol setter
          *  @param aId printer id
          *  @return error code, KErrNone if OK
          */
        virtual TInt SetDefaultPrinterID( TInt aId ) = 0;

        /**
          *  Default printer name getter
          *  @return printer name
          */
        virtual HBufC* GetDefaultPrinterNameLC() = 0;

        /**
          *  Default printer name setter
          *  @param aPrinterName printer name
          *  @return error code, KErrNone if OK
          */
        virtual TInt SetDefaultPrinterNameL( const TDesC& aPrinterName ) = 0;

         /**
          *  Current printer name getter
          *  @return printer name
          */
        virtual HBufC* GetCurrentPrinterName() = 0;

        /**
          *  Current printer name setter
          *  @param aPrinterName printer name
          *  @return error code, KErrNone if OK
          */
        virtual TInt SetCurrentPrinterName( const TDesC& aPrinterName ) = 0;

        /**
          * Current paper size text getter
          * @Return paper size text
          */
        virtual HBufC* GetCurrentPaperSizeTextL() = 0;

    };

#endif //  MPRINTSETTINGS_H

//  End of File
