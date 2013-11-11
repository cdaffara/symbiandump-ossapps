/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:     Class stores data for one accesspoint
*
*/





#ifndef CWPAPNAPDEF_H
#define CWPAPNAPDEF_H

// INCLUDES
#include <e32def.h>
#include <e32std.h>
#include <MWPVisitor.h>
#include "CWPAPItemBase.h"
#include "WPAPDefs.h"
#include <cmconnectionmethodext.h>
#include <cmplugindialcommondefs.h>
#include <cmmanagerext.h>
#include <etelpckt.h>

// FORWARD DECLARATIONS
class CWPAPPhysicalProxy;

class CWPParameter;
class CCommsDatabase;
class EAPSettings;
class EapCertificateEntry;
class RCmManagerExt;
class TEapExpandedType;


// CLASS DECLARATION

/**
 * CEapTypeElement is arrray element for several EapTypes  
 */ 
class CEapTypeElement: public CBase
        {
        public:
        ~CEapTypeElement();
        TBuf8<KExpandedEapIdLength> iEapTypeString;
        TBuf8<KExpandedEapIdLength> iEncapsulatingExpandedEapId;
        EAPSettings* iEAPSettings;
        EapCertificateEntry* iCertificate;
        };

struct SECssID
		{
        CWPParameter* iSSSID;
        CWPParameter* iSUSSID;
		}; 
/**
 * CWPAPNapdef handles NAPDEF characteristics.
 */ 
class CWPAPNapdef : public CWPAPItemBase, private MWPVisitor
	{
	public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aFollowingLink ETrue if the NAPDEF was reached
        *                       via a link.
        * @param aDefaultName   The default name of an access point
        * @param aLogicalCharacteristic The PXLOGICAL, if following a link from 
        *                       there, or NAPDEF, if a direct link to NAPDEF.
        * @param aLink          The NAPDEF characteristic
        */
		static CWPAPNapdef* NewLC( TBool aFollowingLink,
                                   const TDesC& aDefaultName, 
                                   CWPCharacteristic& aLogicalCharacteristic,
                                   CWPCharacteristic& aLink );

        /**
        * Destructor.
        */
        virtual ~CWPAPNapdef();

	public: // From CWPAPItemBase

		/**
		* @precondition characteristic must be set otherwise Panics.
		*/
		TBool ValidateL();
		
        /**
        * AddDataL
        */     		
		void AddDataL( RCmConnectionMethodExt& aCmItem );
		
        /**
        * AddItemsL
        */           
        TInt AddItemsL( RPointerArray<CWPAPAccesspointItem>& aItems,
                        CWPAPItemBase* aLogicalProxy,
                        CWPAPItemBase* aPhysicalProxy );
        /**
        * Name
        */
        const TDesC& Name();
    
        /**
        * SaveWlanDataL
        */    
        void SaveWlanDataL( TUint32 aIapId, CCommsDatabase& aCommsDb );

        /**
        * UpdateLingerL from the ItemBase
        */    

        void UpdateLingerL(const TUint32 aIapId);
        
        /**
        * Set The Internet Parameter Indicator 
        */
        void SetInternetIndicator(TInt value);

        /**
        * Get The Internet Parameter Indicator 
        */
        TInt GetInternetIndicator();


	private: // From MWPVisitor

        /**
        * VisitL
        */   
		void VisitL( CWPCharacteristic& aElement );		

        /**
        * VisitL
        */   
		void VisitL( CWPParameter& aElement );
      
        /**
        * VisitLinkL
        */   
        void VisitLinkL( CWPCharacteristic& /*aLink*/ ) {};

	private:  // New functions

		/**
        * C++ default constructor.
        * @param aFollowingLink ETrue if the NAPDEF was reached
        *                       via a link.
        * @param aDefaultName   The default name of an access point
        * @param aLogicalCharacteristic The PXLOGICAL, if following a link from 
        *                       there, or NAPDEF, if a direct link to NAPDEF.
        */
		CWPAPNapdef( TBool aFollowingLink,
                     const TDesC& aDefaultName, 
                     CWPCharacteristic& aLogicalCharacteristic );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();

        /**
        * Writes a DNS address in either IPv4 or IPv6 format
        * to an access point item.
        * @param aAPItem The access point item
        * @param aValue The DNS address to write
        */
        void WriteDNSAddressL( RCmConnectionMethodExt& aCmItem,
                               const TDesC& aValue );
                               
        /**
        * AddWlanDataL
        */            
        void AddWlanDataL( RCmConnectionMethodExt& aCmItem );
        
        /**
        * HandleWLanParameters
        */            
        void HandleWLanParametersL( CWPParameter& aParameter );
        
        void HandleEAPParametersCCL( CWPParameter& aParameter );
        
        /**
        * ConvertAsciiToHex
        */            
        void ConvertAsciiToHex( const TDesC8& aSource, HBufC8*& aDest );
        
        /**
        * StrCopy
        */                    
        void StrCopy( TDes8& aTarget, const TDesC& aSource );
        
        /**
        * Add linger value
        */                    
        void AddLingerL( const TInt , const TInt );
        
        /**
        * Update linger value	
        */                    
        void UdateligerValueL(CWPParameter* aLinger);
        /**
        * Isalpha	
        */                    
        TBool Notalpha(const TDesC& aValue);
        /**
        * CheckIPv4values	
        */  
        TBool CheckIPv4values(const TDesC& aValue );
        
        /**
        * CheckAddressPart	
        */  
        TBool CheckAddressPart( TLex& aLex );
        
        /**
        * ConvertEAPStringToIds
        */  
        void ConvertEAPStringToIds(const TDesC& aEAPString, TDes8& aExpandedId, TEapExpandedType& aID);
  public: // new function
  
        /**
        * NapId value
        */  
        const TDesC& NapId();
        /**
        * Bearer value
        */
        TUint Bearer();
        
        /**
        * SetCMManager
        */
        void SetCMManager( RCmManagerExt* aCm );

	private: // Data (Total ownership is assumed unless otherwise stated)

        enum TAPWEPKeyFormat
            {
            EAscii,                 // Ascii format
            EHexadecimal            // Hex format
            };
        /// PXLOGICAL characteristic
        CWPCharacteristic& iCharacteristic;

        /// The human-readable name of the connection. Refs.
		CWPParameter* iConnectionName; 

        /// The access point address. Refs.
		CWPParameter* iAccesspointName;

        /// The access point address. Refs.
		CWPParameter* iAccesspointNameType;

        /// The user name. Refs.
		CWPParameter* iUserName;

        /// The password. Refs.
		CWPParameter* iPassword;

        /// The IP address of the phone. Refs.
		CWPParameter* iPhoneIPAddress;

        /// The network type of the access point. Refs.
        CWPParameter* iIfNetworks;

        /// Array of name servers. Refs.
        RPointerArray<CWPParameter> iNameServers;

        /// Number of IPv4 name servers found so far.
        TInt iNumIPv4NameServers;

        /// Number of IPv6 name servers found so far.
        TInt iNumIPv6NameServers;

        /// The bearer of the access point.
        //TApBearerType iBearer;
        TUint iBearer;
        
        // Received bearer was unsupported
        TBool iBearerUnsupported;

        /// The PDP type of a GPRS access point.
        TUint32 iPdpType;

        /// ETrue of secure authentication must be used.
		TBool iSecureAuthentication;

        /// ETrue if the user must be prompted for password.
		TBool iPromptPassword;

        /// ETrue if we arrived to NAPDEF via a link
        TBool iFollowingLink;
        
//JMan for WLan        

		// Is WLAN supported (queried from Feature Manager)
        TBool iWlanSupported;
        
        // The Network Access Point ID.
        CWPParameter* iNapID;
        
        // WLAN characteristic parameters
        
        // The Primary Service Set Identifier name.
        CWPParameter* iPriSSID;
        
        // The Used Primary Service Set Identifier name.
        CWPParameter* iPriUSSID;
        
        // The Hidden Primary Service Set Identifier name.
        CWPParameter* iPriHSSID;
        
        //for secssid 
        RPointerArray<SECssID> iSecSSID;
        
        
        // The operation mode of the WLAN network.
        TUint32 iNetMode;
        
        // The Security mode of the WLAN network.
        TUint32 iSecMode;
        
        // The ascii formed pre-shared key.
        CWPParameter* iWpaPresKeyAsc;
                
        // The wep authentication mode
        CWPParameter* iWepAuthMode;
        
        // Indicates the default wepkey
        TUint iDefaultWepKeyIndex;
        
        // WEPKEY characteristic parameters
        
        // The lenght of the wepkey
        CWPParameter* iWepKeyLength;

        // The index of the wepkey
        TUint iWepInd;
        
        // The wepkey data
        CWPParameter* iWepKeyData;
        
        // Type of authentication (EAuthOpen, EAuthShared)
        TWEPAuthentication iAuthentication;

        // Data of the key
        TBuf8<KMaxLengthOfKeyData> iKeyData[KMaxNumberofKeys];
        
        // Format of the key
        TAPWEPKeyFormat iKeyFormat[KMaxNumberofKeys];
        
        RPointerArray<CEapTypeElement> iEapTypeArray;      
        
		// Pointer to Linger value parameter. Refs.
		CWPParameter*   iLinger;

		//linger value
		TUint iLingerValue;
		TBool iLingerFlag;
		
		// Pointer to the used CMManager, needed to save WLAN data
		RCmManagerExt* iCm;
		
		//Internet parameter Indicator
    TInt iInternetIndicator;

	private: // For testing usage.
		friend class T_CWPAPNapdef;
		friend class T_CWPAPAdapter;
		friend class T_WPAPTestUtils;
	};

#endif	// CWPAPNAPDEF_H
            
// End of File
