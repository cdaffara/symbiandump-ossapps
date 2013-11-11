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
* Description:  HTI Internet Access Point handler.
*
*/


#ifndef CHTIIAPHANDLER_H
#define CHTIIAPHANDLER_H


// INCLUDES
#include <cmpsettingsconsts.h>
#include <HtiServicePluginInterface.h>

// CONSTANTS

// MACROS

// DATA TYPES
enum TApFieldDataType
    {
    EDataTypeText,
    EDataTypeBool,
    EDataTypeUint,
    EDataTypeText8,
    EDataTypeUnknown
    };

struct TApField
    {
    TInt      iId;
    HBufC*    iData;
    TApFieldDataType iDataType;
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  HTI Internet Access Point handler.
*/
class CHtiIAPHandler : public CBase
    {
    public:
        enum TLegacyApFieldCodes
        {
        EApWapStartPage            =   3,
        EApIspDefaultTelNumber     =  18,
        EApIspUseLoginScript       =  20,
        EApIspLoginScript          =  21,
        EApIspLoginName            =  23,
        EApIspLoginPass            =  24,
        EApIspIfPromptForAuth      =  29,
        EApIspIfCallbackEnabled    =  33,
        EApIspIfCallbackType       =  34,
        EApIspIfCallbackInfo       =  35,
        EApIspIPAddr               =  38,
        EApIspIPNameServer1        =  42,
        EApIspIPNameServer2        =  43,
        EApIspEnableIpHeaderComp   =  44,
        EApIspDisablePlainTextAuth =  46,
        EApIspBearerSpeed          =  49,
        EApIspBearerCallTypeIsdn   =  50,
        EApIspInitString           =  52,
        EApGprsAccessPointName     =  58,
        EApGprsPdpType             =  59,
        EApProxyServerAddress      =  91,
        EApProxyPortNumber         =  93,
        EApIP6NameServer1          = 104,
        EApIP6NameServer2          = 105,
        EApWlanNetworkName         = 163,
        EApWlanNetworkMode         = 164,
        EApWlanSecurityMode        = 165,

        // new fields that do not exist in legacy TApMember enum
        EApSeamlessnessLevel       = 200
        };

        /**
        * Two-phased constructor.
        */
        static CHtiIAPHandler* NewL();

        /**
        * Called when there is a message to be processed by this service.
        * @param aMessage message body destinated to the servive
        * @param aPriority message priority
        */
        void ProcessMessageL( const TDesC8& aMessage,
                              THtiMessagePriority aPriority );

        /**
        * Destructor.
        */
        virtual ~CHtiIAPHandler();

        /**
        * Sets the dispatcher to send outgoing messages to.
        * @param aDispatcher pointer to dispatcher instance
        */
        void SetDispatcher( MHtiDispatcher* aDispatcher );

    private:

        /**
        * C++ default constructor.
        */
        CHtiIAPHandler();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // helpers

        void HandleCreateIapL( const TDesC8& aData );
        void HandleDeleteIapL( const TDesC8& aData );
        void HandleCreateDestinationL( const TDesC8& aData );
        void HandleDeleteDestinationL( const TDesC8& aData );
        void ModifyDestinationL( const TInt aCommand, const TDesC8& aData );
        void SetDefaultConnectionL( const TDesC8& aData );

        void ParseCreateMessageL( const TDesC8& aData );
        TInt GetAccessPointUIDL();
        TInt GetDestinationIDL( const TDesC& aName );
        void CloseActiveConnectionsL();

        TUint GetBearerUID( const TInt aBearerType );
        void MapLegacyFieldIDs();
        void ResolveFieldDataTypes();

        void SendOkMsgL( const TDesC8& aData );
        void SendErrorMessageL( TInt aError, const TDesC8& aDescription );

    private:  // Data

        // Pointer to the dispatcher (referenced)
        MHtiDispatcher* iDispatcher; // referenced

        // Name of the connection to create/delete
        TBuf<KMaxConnNameLength>   iConnName;
        TBuf<KMaxConnNameLength>   iDestName;

        // The type of Access Point to create
        TUint         iBearerType;

        // The fields to add to the Access Point
        RArray<TApField> iFields;

    };

#endif // CHTIIAPHANDLER_H

// End of file

