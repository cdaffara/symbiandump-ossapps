/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CMPSettingsModel stub
*
*/

// Version : %version: 3 %




#ifndef MMPSETTINGSMODEL_H
#define MMPSETTINGSMODEL_H

// INCLUDES
#include <e32std.h>
#include <qstring.h>

// DATA TYPES
// Config file version
enum TConfigVersion
    {
    EConfigDefault,
    EConfigUser
    };

// Bearer type
enum TDataBearer
    {
    EBearerGPRS,
    EBearerEGPRS,
    EBearerWCDMA,
    EBearerCDMA,
    EBearerCDMA2000,
    EBearerWLAN,
    EBearerHSDPA
    };

// CLASS DECLARATION

/**
*  CMPSettingsModel
*  Common API for MMF specific settings models.
*
*  @lib MPSettEngine.dll
*  @since 2.0
*/
class CMPSettingsModel
    {
    public:

       /**
        * Constructor. Uses ECom to construct an instance of this class.
        * @param aUid Implementation uid of the object that is to be constructed.
        */
        static CMPSettingsModel* NewL(TUid aUid);
        
        /**
         * Constructor
         */
        CMPSettingsModel();
        
        /**
        * Destructor.
        */
        virtual ~CMPSettingsModel();

    public: // New functions

        /**
        * Reads settings from MMF controller.
        * @since 2.0
        * @param aSettingsType Settings value type:
        *         EConfigDefault: Load default values
        *         EConfigUser:    Load user values
        */
        virtual void LoadSettingsL(TInt aConfigVersion);

        /**
        * Writes settings to MMF controller.
        * @since 2.0
        */
        virtual void StoreSettingsL();
        
        /**
        * Sets proxy mode..
        * @since 2.0
        * @param aMode Proxy mode.
        * @return Error value.
        */
        virtual TInt SetProxyMode(TInt aMode);
        
        /**
        * Returns proxy mode.
        * @since 2.0
        * @param aMode Proxy mode:
        *         0:    Disabled
        *         1:    Enabled
        * @return Error value.
        */
        virtual TInt GetProxyMode(TInt& aMode);

        /**
        * Sets Proxy host name.
        * @since 2.0
        * @param aHostName Proxy host name.
        * @return Error value.
        */
        virtual TInt SetProxyHostNameL(const TDesC& aHostName);
        
        /**
        * Returns proxy host name.
        * @since 2.0
        * @param aHostName Proxy host name.
        * @return Error value.
        */
        virtual TInt GetProxyHostName(TDes& aHostName);

        /**
        * Sets proxy port number.
        * @since 2.0
        * @param aPort Proxy port number.
        * @return Error value.
        */
        virtual TInt SetProxyPort(TInt aPort);
        
        /**
        * Returns proxy port number..
        * @since 2.0
        * @return Integer: Proxy port number.
        * @return Error value.
        */
        virtual TInt GetProxyPort(TInt& aPort);
        
        /**
        * Sets default access point.
        * @since 2.0
        * @param aApId Access point ID.
        * @return Error value.
        */
        virtual TInt SetDefaultAp(TUint32 aApId);

        /**
        * Returns default access point ID.
        * @since 2.0
        * @param aApId Access point ID.
        * @return Error value.
        */
        virtual TInt GetDefaultAp(TUint32& aApId);

        /**
        * Sets minimum UDP port number.
        * @since 2.0
        * @param aPort minimum port number.
        * @return Error value.
        */
        virtual TInt SetMinUDPPort(TInt aPort);

        /**
        * Returns minimum UDP port number.
        * @since 2.0
        * @param aPort minimum UDP port number in minutes.
        * @return Error value.
        */
        virtual TInt GetMinUDPPort(TInt& aPort);

        /**
        * Sets maximum UDP port number.
        * @since 2.0
        * @param aPort maximum port number.
        * @return Error value.
        */
        virtual TInt SetMaxUDPPort(TInt aPort);

        /**
        * Returns maximum UDP port number.
        * @since 2.0
        * @param aPort maximum UDP port number in minutes.
        * @return Error value.
        */
        virtual TInt GetMaxUDPPort(TInt& aPort);
        
public:
        static TUid mImplUid;
        static TInt mConfigMode;
        static TInt mProxyMode;
        static QString mHostName;
        static TInt mProxyPort;
        static TInt mMinUdpPort;
        static TInt mMaxUdpPort;
        static int mStoreSettingsCount;
        static int mDestructorCount;
        static int mApId;

    };

#endif      // MMPSETTINGSMODEL_H   
            
// End of File
