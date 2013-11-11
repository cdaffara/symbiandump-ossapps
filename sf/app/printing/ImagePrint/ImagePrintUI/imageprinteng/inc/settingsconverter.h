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


#ifndef SETTINGSCONVERTER_H
#define SETTINGSCONVERTER_H

#include <e32std.h>

class CTIntCapability;
class CListCapability;
class CFloatCapability;
class TPrintCapability;
class CRealFactory;

/**
 *
 *  Utility class for converting capabilities to correct format
 *
 */
class SettingsConverter
    {
    public:

        /**
         *  Converts TCapability to CTIntCapability
         *
         *  @param aCapability      Capability coming from the engine
         *  @param aIndexOnList     Capability's index on the list box
         *  @param aCapabUid        Capability's UID
         *  @return Initialized CTIntCapability instance
         */
        static CTIntCapability* AddTIntCapabilityLC(
            TPrintCapability& aCapability, TInt aIndexOnList, TInt aCapabUid );

        /**
         *  Converts TCapability to CListCapability
         *
         *  @param aCapability      Capability coming from the engine
         *  @param aIndexOnList     Capability's index on the list box
         *  @param aCapabUid        Capability's UID
         *  @return Initialized CListCapability instance
         */
        static CListCapability* AddListCapabilityLC(
            TPrintCapability& aCapability, TInt aIndexOnList, TInt aCapabUid,
                    CRealFactory* aFactory );

        /**
         *  Converts TCapability to CFloatCapability
         *
         *  @param aCapability      Capability coming from the engine
         *  @param aIndexOnList     Capability's index on the list box
         *  @param aCapabUid        Capability's UID
         *  @return Initialized CFloatCapability instance
         */
        static CFloatCapability* AddFloatCapabilityLC(
            TPrintCapability& aCapability, TInt aIndexOnList, TInt aCapabUid );

    private:

        /**
         *  Creates text string from the enumeration ID
         *
         *  @param aId      Enumeration Id
         *  @return buffer containing the string
         */
        static HBufC* TextStringLC( TInt aId );
    };

#endif  //  SETTINGSCONVERTER_H

//  End of File
