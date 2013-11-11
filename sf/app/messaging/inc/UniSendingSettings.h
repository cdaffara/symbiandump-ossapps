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
 * Description:
 *
 */

#ifndef __UNISENDINGSETTINGS_H
#define __UNISENDINGSETTINGS_H

// INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* TUniSendingSettings
*
* @since 3.1
*/
class TUniSendingSettings
    {
    public:

        enum TUniPriority
            {
            EUniPriorityHigh,
            EUniPriorityNormal,
            EUniPriorityLow
            };

        enum TUniCharSupport
            {
            EUniCharSupportFull,
            EUniCharSupportReduced
            };

        enum TUniValidityPeriod
            {
            EUniValidityPeriod1h,
            EUniValidityPeriod6h,
            EUniValidityPeriod24h,
            EUniValidityPeriod3Days,
            EUniValidityPeriodWeek,
            EUniValidityPeriodMax
            };

        enum TUniMessageType
            {
            EUniMessageTypeAutomatic,
            EUniMessageTypeText,
            EUniMessageTypeMultimedia,
            EUniMessageTypeFax,
            EUniMessageTypePaging
            };

    public:

        /**
        * Default constructor
        */
        inline TUniSendingSettings();

        /**
        * Copy constructor
        */
        inline TUniSendingSettings( const TUniSendingSettings& aCopy );

    public:

        TUniPriority        iPriority;
        TBool               iDeliveryReport;
        TUniCharSupport     iCharSupport;
        TUniValidityPeriod  iValidityPeriod;
        TBool               iReplyViaSameCentre;
        TUniMessageType     iMessageType;
        TBool               iDialogExited;
    };


// INLINES
// ---------------------------------------------------------
// TUniSendingSettings::TUniSendingSettings
// Default Constructor
// ---------------------------------------------------------
//
inline TUniSendingSettings::TUniSendingSettings() :
    iPriority( EUniPriorityNormal ),
    iDeliveryReport( EFalse ),
    iCharSupport( EUniCharSupportReduced ),
    iValidityPeriod( EUniValidityPeriod24h ),
    iReplyViaSameCentre( EFalse ),
    iMessageType( EUniMessageTypeAutomatic ),
    iDialogExited( EFalse )
    {
    }

// ---------------------------------------------------------
// TUniSendingSettings::TUniSendingSettings
// Copy Constructor
// ---------------------------------------------------------
//
inline TUniSendingSettings::TUniSendingSettings( const TUniSendingSettings& aCopy ) :
    iPriority( aCopy.iPriority ),
    iDeliveryReport( aCopy.iDeliveryReport ),
    iCharSupport( aCopy.iCharSupport ),
    iValidityPeriod( aCopy.iValidityPeriod ),
    iReplyViaSameCentre( aCopy.iReplyViaSameCentre ),
    iMessageType( aCopy.iMessageType ),
    iDialogExited( aCopy.iDialogExited )
    {
    }

#endif      // __UNISENDINGSETTINGS_H

// End of File
