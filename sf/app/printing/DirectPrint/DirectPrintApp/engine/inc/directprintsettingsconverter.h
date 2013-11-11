/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:
* {Description of the file}
*
*/

// Protection against nested includes
#ifndef DIRECTPRINTSETTINGSCONVERTER_H
#define DIRECTPRINTSETTINGSCONVERTER_H

#include <e32std.h>

class CDirectPrintTIntCapability;
class CDirectPrintListCapability;
class CDirectPrintFloatCapability;
class CDirectPrintTextCapability;
class TDirectPrintCapability;

/**
 *
 *  Utility class for converting capabilities to correct format
 *
 */
class DirectPrintSettingsConverter
    {
    public:

        /**
         *  Converts TDirectPrintCapability to CDirectPrintTIntCapability
         *
         *  @param aCapability      Capability coming from the engine
         *  @param aIndexOnList     Capability's index on the list box
         *  @param aCapabUid        Capability's UID
         *  @return Initialized CDirectPrintTIntCapability instance
         */
        static CDirectPrintTIntCapability* AddTIntCapabilityLC(
            TDirectPrintCapability& aCapability, TInt aIndexOnList, TInt aCapabUid );

        /**
         *  Converts TDirectPrintCapability to CDirectPrintListCapability
         *
         *  @param aCapability      Capability coming from the engine
         *  @param aIndexOnList     Capability's index on the list box
         *  @param aCapabUid        Capability's UID
         *  @return Initialized CDirectPrintListCapability instance
         */
        static CDirectPrintListCapability* AddListCapabilityLC(
            TDirectPrintCapability& aCapability, TInt aIndexOnList, TInt aCapabUid );

        /**
         *  Converts TDirectPrintCapability to CDirectPrintFloatCapability
         *
         *  @param aCapability      Capability coming from the engine
         *  @param aIndexOnList     Capability's index on the list box
         *  @param aCapabUid        Capability's UID
         *  @return Initialized CDirectPrintFloatCapability instance
         */
        static CDirectPrintFloatCapability* AddFloatCapabilityLC(
            TDirectPrintCapability& aCapability, TInt aIndexOnList, TInt aCapabUid );

        /**
         *  Converts TDirectPrintCapability to CDirectPrintTextCapability
         *
         *  @param aCapability      Capability coming from the engine
         *  @param aIndexOnList     Capability's index on the list box
         *  @param aCapabUid        Capability's UID
         *  @return Initialized CDirectPrintTextCapability instance
         */
        static CDirectPrintTextCapability* AddTextCapabilityLC(
            TDirectPrintCapability& aCapability, TInt aIndexOnList, TInt aCapabUid );

    private:

        /**
         *  Creates text string from the enumeration ID
         *
         *  @param aId      Enumeration Id
         *  @return buffer containing the string
         */
        static HBufC* TextStringLC( TInt aId );
    };

#endif  //  DIRECTPRINTSETTINGSCONVERTER_H

//  End of File
