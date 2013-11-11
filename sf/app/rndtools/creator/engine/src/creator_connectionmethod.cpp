/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "engine.h"
#include "enginewrapper.h"
#include "creator_connectionmethod.h" 
#include "creator_traces.h"



using namespace CMManager;
//#define KUidLanBearerType  0x1020737A
#include <cmpluginpacketdatadef.h>  
#include <cmplugincsddef.h> 
#include <cmpluginhscsddef.h> 
#include <cmmanagerext.h>
#include <cmdestinationext.h> 
#include <cmpluginembdestinationdef.h>
#include <cmpluginvpndef.h>
#include <cmpluginlandef.h> 
#include <cmdefconnvalues.h>
#include <cmconnectionmethoddef.h>


#include <wlanmgmtinterface.h>
#include <wlanmgmtclient.h>
#include <wlanmgmtclient.inl> 
#include <wlanmgmtcommon.h> 
#include <wlanscaninfo.h> 
#include <wlanscaninfo.inl> 
#include <wlanscaninfointerface.h>

//#include <WEPSecuritySettingsUI.h>//For WEP setting
//#include <dbcreatorcommon.h>

static const TInt KConnectionSettingssFieldLength = 128;

_LIT(KCreatorConnectionSettingssPrefixName, "CR_");

//----------------------------------------------------------------------------

CConnectionSettingsParameters::CConnectionSettingsParameters()
    {
    LOGSTRING("Creator: CConnectionSettingsParameters::CConnectionSettingsParameters");
        
    iConnectionName = HBufC::New(KConnectionSettingssFieldLength);
    iStartPage = HBufC::New(KConnectionSettingssFieldLength);
    iLoginName = HBufC::New(KConnectionSettingssFieldLength);
    iLoginPass = HBufC::New(KConnectionSettingssFieldLength);
    iIPAddr = HBufC::New(KConnectionSettingssFieldLength);
    iIP4NameServer1 = HBufC::New(KConnectionSettingssFieldLength);
    iIP4NameServer2 = HBufC::New(KConnectionSettingssFieldLength);
    iDefaultTelNumber = HBufC::New(KConnectionSettingssFieldLength);
    iProxyServerAddress = HBufC::New(KConnectionSettingssFieldLength);
    iSubnetMask = HBufC::New(KConnectionSettingssFieldLength); 
    iGatewayAddress = HBufC::New(KConnectionSettingssFieldLength);
    iWLANName = HBufC::New(KConnectionSettingssFieldLength);
    iWlanIpAddr = HBufC::New(KConnectionSettingssFieldLength);
    iIP6NameServer1 = HBufC::New(KConnectionSettingssFieldLength);
    iIP6NameServer2 = HBufC::New(KConnectionSettingssFieldLength);
    }

CConnectionSettingsParameters::~CConnectionSettingsParameters()
    {
    LOGSTRING("Creator: CConnectionSettingssParameters::~CConnectionSettingssParameters");

    delete iProxyServerAddress;
    delete iDefaultTelNumber;
    delete iIP4NameServer2;
    delete iIP4NameServer1;
    delete iIPAddr;    
    delete iLoginPass;
    delete iLoginName;
    delete iStartPage;
    delete iConnectionName;
    delete iSubnetMask;  
    delete iGatewayAddress;
    delete iWLANName;
    delete iWlanIpAddr;
    delete iIP6NameServer1;
    delete iIP6NameServer2;    
    }

//----------------------------------------------------------------------------

CCreatorConnectionSettings* CCreatorConnectionSettings::NewL(CCreatorEngine* aEngine)
    {
    CCreatorConnectionSettings* self = CCreatorConnectionSettings::NewLC(aEngine);
    CleanupStack::Pop(self);
    return self;
    }

CCreatorConnectionSettings* CCreatorConnectionSettings::NewLC(CCreatorEngine* aEngine)
    {
    CCreatorConnectionSettings* self = new (ELeave) CCreatorConnectionSettings;
    CleanupStack::PushL(self);
    self->ConstructL(aEngine);
    return self;
    }

CCreatorConnectionSettings::CCreatorConnectionSettings()
    {
    }

void CCreatorConnectionSettings::ConstructL(CCreatorEngine* aEngine)
    {
    LOGSTRING("Creator: CCreatorConnectionSettings::ConstructL");

    iEngine = aEngine;
    
    iCmManagerExt.CreateTablesAndOpenL();
    }

CCreatorConnectionSettings::~CCreatorConnectionSettings()
    {
    LOGSTRING("Creator: CCreatorConnectionSettings::~CCreatorConnectionSettings");

    if ( iEngine && iEntryIds.Count() )
        {
        TRAP_IGNORE( iEngine->WriteEntryIdsToStoreL( iEntryIds, KUidDictionaryUidIAP ) );
        }
    iEntryIds.Reset();
    iEntryIds.Close();
    
    if (iParameters)
        {
        delete iParameters;
        }
    
    iCmManagerExt.Close();
    }
    

//----------------------------------------------------------------------------

TBool CCreatorConnectionSettings::AskDataFromUserL(TInt aCommand)
    {
    LOGSTRING("Creator: CCreatorConnectionSettings::AskDataFromUserL");
    
    CCreatorModuleBase::AskDataFromUserL( aCommand );
    
    if( aCommand == ECmdDeleteIAPs )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog(_L("Delete all Connection Methods?"), this, ECreatorModuleDelete );
        }
    else if( aCommand == ECmdDeleteCreatorIAPs )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog(_L("Delete all Connection Methods created with Creator?"), this, ECreatorModuleDelete );
        }
    else
        {
        return iEngine->GetEngineWrapper()->EntriesQueryDialog(&iEntriesToBeCreated, _L("How many entries to create?"), EFalse, this, ECreatorModuleStart );
        }
    }


//----------------------------------------------------------------------------

TInt CCreatorConnectionSettings::CreateConnectionSettingsEntryL(CCreatorModuleBaseParameters *aParameters)
    {
    LOGSTRING("Creator: CCreatorConnectionSettings::CreateConnectionSettingsEntryL");

    CConnectionSettingsParameters* parameters = (CConnectionSettingsParameters*) aParameters;
    
    // clear any existing parameter definations 
    delete iParameters;
    iParameters = NULL;
            
    // random data needed if no predefined data available
    if (!parameters)
        {
        iParameters = new (ELeave) CConnectionSettingsParameters();
        parameters = iParameters;
        SetRandomParametersL(*parameters);
        }
    
    if( parameters->iBearerType == KRandomBearerType )
    	{
    	SetRandomBearerTypeL(*parameters);
    	}
    
    TInt err = KErrNone;

    // create unique name for each network destination
    RCmDestinationExt dest;
    TInt destinationError = KErrNone;
    TInt count = 0;
	TBuf<KConnectionSettingssFieldLength> name;
	name.Copy(parameters->iConnectionName->Des());
    do
    	{
    	TRAP(destinationError, dest = iCmManagerExt.CreateDestinationL(name));
    	if (destinationError == KErrAlreadyExists)
    		{
    		name.Copy(parameters->iConnectionName->Des());
    		count++;
    		name.AppendFormat(_L(" (%d)"), count);
    		}
    	else if (destinationError != KErrNone)
    		{
    		User::Leave(destinationError);
    		}
    	}
    while (destinationError == KErrAlreadyExists);
    parameters->iConnectionName->Des().Copy(name);        

    CleanupClosePushL(dest);
    dest.UpdateL();
    
    RCmConnectionMethodExt conMethod = dest.CreateConnectionMethodL( parameters->iBearerType);//KUidPacketDataBearerType
    CleanupClosePushL( conMethod );
    
    if(parameters->iBearerType == KUidWlanBearerType)
        {
        //IP net mask of interface
        conMethod.SetStringAttributeL( EWlanIpNetMask, parameters->iSubnetMask->Des()  );

        //IP address of EPOC.
        conMethod.SetStringAttributeL( EWlanIpAddr, parameters->iWlanIpAddr->Des()  );
        //
        //following gateway ID:s is not workin, 
        //logically ID would be EWlanIpGateway because EWlanIpNetMask
        //is going to net mask, which is just before gateway in ui
        //--->
        conMethod.SetStringAttributeL( EWlanIpGateway, parameters->iGatewayAddress->Des()  );
        //conMethod.SetStringAttributeL( EApIspIPGateway, parameters->iGatewayAddress->Des()  );
        conMethod.SetStringAttributeL( ECmWapIPGatewayAddress, parameters->iGatewayAddress->Des()  );
        conMethod.SetStringAttributeL( ECmIPGateway, parameters->iGatewayAddress->Des()  );
        //<---
        
        //IP Address of primary and secondary name server (IP v4).
        conMethod.SetStringAttributeL( EWlanIpNameServer1, parameters->iIP4NameServer1->Des()  );
        conMethod.SetStringAttributeL( EWlanIpNameServer2, parameters->iIP4NameServer2->Des()  );
        //The SSID of this WLAN connection.
        conMethod.SetStringAttributeL( EWlanSSID, parameters->iWLANName->Des()  );

        //IP6 address of primary and secondary name servers
        conMethod.SetStringAttributeL( EWlanIp6NameServer1, parameters->iIP6NameServer1->Des()  );
        conMethod.SetStringAttributeL( EWlanIp6NameServer2, parameters->iIP6NameServer2->Des()  );        

        //
        //Setting secure mode settings
        //
        
        //Network status (default: EWlanSecModeOpen) // EWlanAuthenticationMode use instead?
        conMethod.SetIntAttributeL( EWlanSecurityMode, parameters->iWLanSecMode );
        //WLAN Network mode  
        conMethod.SetIntAttributeL( EWlanConnectionMode, parameters->iWlanNetMode );
        
        
        if(parameters->iWLanSecMode == EWlanSecModeOpen)
            {}//Nothing to set when mode is open
        else if(parameters->iWLanSecMode == EWlanSecModeWep)
            {
            //See WEPSecuritySettingsUI.h
            
            
            ///CWEPSecuritySettings
           // conMethod.SetIntAttributeL( EWlanSecurityMode, parameters->iWLanSecMode );
            }
        else if(parameters->iWLanSecMode == EWlanSecMode802_1x)
            {
            }
        else if(parameters->iWLanSecMode == EWlanSecModeWpa)
            {
            }
        else if(parameters->iWLanSecMode == EWlanSecModeWpa2)
            {
            }            
        
        //WLAN security setting 
        //conMethod.SetStringAttributeL( EWlanAuthenticationMode, parameters->iIP6NameServer2->Des()  );
        //Utilise this method
        //conMethod.SetStringAttributeL( EWlanSecurityMode, parameters->iWLanSecMode  );
        }
    //Not known special attributes for packet data 
    else if(parameters->iBearerType == KUidPacketDataBearerType)
        {

        //Access point name.
        conMethod.SetStringAttributeL( EPacketDataAPName, parameters->iConnectionName->Des() );//crash
        
        //Authentication user name
        conMethod.SetStringAttributeL( EPacketDataIFAuthName, parameters->iLoginName->Des()  );
        ///TProtocolType a;
        //EPdpTypeIPv4 or EPdpTypeIPv6
        conMethod.SetIntAttributeL( EPacketDataPDPType, parameters->iProtocolType  );
                
        //IP6 address of primary and secondary name servers
        conMethod.SetStringAttributeL( EPacketDataIPIP6NameServer1, parameters->iIP6NameServer1->Des()  );
        conMethod.SetStringAttributeL( EPacketDataIPIP6NameServer2, parameters->iIP6NameServer2->Des()  );                
        //IP address of primary name server
        conMethod.SetStringAttributeL( EPacketDataIPNameServer1, parameters->iIP4NameServer1->Des()  );
        //IP address of secondary name server
        conMethod.SetStringAttributeL( EPacketDataIPNameServer2, parameters->iIP4NameServer2->Des()  );
        
        //Also existing but UI not showing them: EPacketDataHeaderCompression, EPacketDataDisablePlainTextAuth
        //EPacketDataIFAuthPass, EDialIFAuthPass, EPacketDataIFPromptForAuth, EPacketDataIPAddr
        }
    else if(parameters->iBearerType == KUidCSDBearerType || parameters->iBearerType == KUidHSCSDBearerType)
        {
        //Authentication user name
        conMethod.SetStringAttributeL( EDialIFAuthName, parameters->iLoginName->Des()  );
        //Dial-up number
        conMethod.SetStringAttributeL( EDialDefaultTelNum, parameters->iDefaultTelNumber->Des() );
        //Call type
        conMethod.SetIntAttributeL( EDialCallType , parameters->iBearerCallTypeIsdn );
        //Maximum speed
        conMethod.SetIntAttributeL( EDialMaxConnSpeed, parameters->iBearerSpeed );
        //Prompt user for username and password? (Only for dial out)
        conMethod.SetBoolAttributeL( EDialPromptForLogin, parameters->iPromptPassword );
        //IP6 address of primary and secondary name servers
        conMethod.SetStringAttributeL( EDialIP6NameServer1, parameters->iIP6NameServer1->Des()  );
        conMethod.SetStringAttributeL( EDialIP6NameServer2, parameters->iIP6NameServer2->Des()  );         
        //Disable plaintext authentication
        conMethod.SetBoolAttributeL( EDialDisablePlainTextAuth, parameters->iDisableTextAuth );       
        //IP address of primary name server
        conMethod.SetStringAttributeL( EDialIPNameServer1, parameters->iIP4NameServer1->Des()  );
        //IP address of secondary name server
        conMethod.SetStringAttributeL( EDialIPNameServer2, parameters->iIP4NameServer2->Des()  );

        //Also existing  EDialEnableIPHeaderComp; EDialIFPromptForAuth;EDialIPAddress parameters
        
        }
    else if(parameters->iBearerType == KUidLanBearerType)
        {

        conMethod.SetStringAttributeL( ELanIpNetMask, parameters->iSubnetMask->Des() );
        conMethod.SetStringAttributeL( ELanIpGateway, parameters->iGatewayAddress->Des());
        conMethod.SetStringAttributeL( ELanIpAddr, parameters->iIPAddr->Des() );
        conMethod.SetStringAttributeL( ELanIpNameServer1, parameters->iIP4NameServer1->Des() );
        conMethod.SetStringAttributeL( ELanIpNameServer2, parameters->iIP4NameServer2->Des() );
        conMethod.SetStringAttributeL( ELanIp6NameServer1,parameters->iIP6NameServer1->Des() );
        conMethod.SetStringAttributeL( ELanIp6NameServer2,parameters->iIP6NameServer2->Des() );
        
        }
    else //KUidEmbeddedDestination and KPluginVPNBearerTypeUid
        {
        //NOTE: NOT Tested, because no supporting Emulator for them
        
        //Authentication user name
        conMethod.SetStringAttributeL( ECmIFAuthName, parameters->iLoginName->Des()  );
        //IP6 address of primary and secondary name servers
        conMethod.SetStringAttributeL( ECmIP6NameServer1, parameters->iIP6NameServer1->Des()  );
        conMethod.SetStringAttributeL( ECmIP6NameServer2, parameters->iIP6NameServer2->Des()  );        
        //Disable plaintext authentication
        conMethod.SetBoolAttributeL( ECmDisablePlainTextAuth, parameters->iDisableTextAuth );
        //Prompt user for authentication
        conMethod.SetBoolAttributeL( ECmIFPromptForAuth, parameters->iPromptPassword );
        //Authentication password
        conMethod.SetStringAttributeL( ECmIFAuthPass, parameters->iLoginPass->Des()  );
        }
    
    //
    //Adding common attributes, all not neccessary used with all bearer types, but should not cause any problems eather
    //

    //CM Name (Connection Method, formelly known as Access Point)         
    conMethod.SetStringAttributeL( ECmName, parameters->iConnectionName->Des() );        
    // Start page of CM
    conMethod.SetStringAttributeL( ECmStartPage, parameters->iStartPage->Des() );
    // Enable Proxy   
    conMethod.SetBoolAttributeL( ECmProxyUsageEnabled, parameters->iUseProxy );
    // Server Name
    conMethod.SetStringAttributeL( ECmProxyServerName, parameters->iProxyServerAddress->Des() );
    // Port Number
    conMethod.SetIntAttributeL( ECmProxyPortNumber, parameters->iProxyPortNumber );
    //Proxy login name
    conMethod.SetStringAttributeL( ECmWapIPProxyLoginName, parameters->iLoginName->Des() );
    //Proxy login password
    conMethod.SetStringAttributeL( ECmWapIPProxyLoginPass, parameters->iLoginPass->Des() );
    //IP address of gateway
    conMethod.SetStringAttributeL( ECmIPGateway, parameters->iGatewayAddress->Des() );
    //connection-oriented or connectionless API should be used
    //ECmWapIPWSPOption option TWapWSPOption: ECmWapWspOptionConnectionless or ECmWapWspOptionConnectionOriented    
    conMethod.SetIntAttributeL( ECmWapIPWSPOption, parameters->iWapWspOption );   
    //NOT WORKING????? Attempt secure WTLS connection to the gateway
    conMethod.SetBoolAttributeL( ECmWapIPSecurity, parameters->iSecureAuthentication );
      
    //
    //Adding advanced settings (Network type, Phone IP addr, DNS addr, proxy srv name, proxy port nbr
    //
    
    //IP address of interface
    conMethod.SetStringAttributeL( ECmIPAddress, parameters->iIPAddr->Des() );    
    //IP address of primary name server
    conMethod.SetStringAttributeL( ECmIPNameServer1, parameters->iIP4NameServer1->Des()  );
    //IP address of secondary name server
    conMethod.SetStringAttributeL( ECmIPNameServer2, parameters->iIP4NameServer2->Des()  );
      
    
    conMethod.UpdateL();

    //If no defaults set, set the created CM as a default
    SetDefaultCML( conMethod );
    
    // id has been generated, store it for being able to delete
    // only entries created with Creator
    iEntryIds.AppendL( dest.Id() );
    
    conMethod.Close();
    CleanupStack::Pop(  );//conMethod
    
    dest.UpdateL();
    dest.Close();
    CleanupStack::Pop(); // dest
        
    return err;
    }

//----------------------------------------------------------------------------
void CConnectionSettingsParameters::SetRandomCMNameL(CCreatorEngine& aEngine)
    {
    LOGSTRING("Creator: CConnectionSettingsParameters::SetRandomCMNameL");

    TInt num = aEngine.RandomNumber(1, 3);
    TBuf<10> apType;

    if (num==1)
        {
        apType = _L("mms");
        }            
    else if (num==2)
        {
        apType = _L("wap");
        }
    else
        {
        apType = _L("internet");
        }
    TBuf<160> company = aEngine.RandomString(CCreatorEngine::ECompany);
    iConnectionName->Des() = KCreatorConnectionSettingssPrefixName;
    iConnectionName->Des().Append( company );
    iConnectionName->Des().Append(_L(" "));
    iConnectionName->Des().Append(apType);
    }

void CConnectionSettingsParameters::SetRandomWLANNameL(CCreatorEngine& aEngine)
    {
    LOGSTRING("Creator: CConnectionSettingsParameters::SetRandomWLANNameL");

    TBuf<160> company = aEngine.RandomString(CCreatorEngine::ECompany);
    iWLANName->Des() = KCreatorConnectionSettingssPrefixName;
    iWLANName->Des().Append( company );
    iWLANName->Des().Append( _L(" WLAN") );
    }

void CConnectionSettingsParameters::SetRandomWLANNetModeL(CCreatorEngine& aEngine)
    {
    LOGSTRING("Creator: CConnectionSettingsParameters::SetRandomWLANNetModeL");

    TInt wlanNetModeRand = aEngine.RandomNumber(1, 2);
    if(wlanNetModeRand == 1)
        {
        iWlanNetMode = EAdhoc;//Ad-Hoc mode
        }
    else
        {
        iWlanNetMode = CMManager::EInfra;//Infrastructure mode
        } 
    }

void CConnectionSettingsParameters::SetRandomWLANSecurityModeL(CCreatorEngine& aEngine)
    {
    LOGSTRING("Creator: CConnectionSettingsParameters::SetRandomWLANSecurityModeL");

    TInt wlanSecModeRand = aEngine.RandomNumber(1, 5);
    if(wlanSecModeRand == 1)
        {
        iWLanSecMode = EWlanSecModeOpen;///< Open security mode
        }
    else if(wlanSecModeRand == 2)
        {
        iWLanSecMode = EWlanSecModeWep;///< WEP security mode
        //See WEPSecuritySettingsUI.h
                
        ///CWEPSecuritySettings
                
        //WEP key in us
        
        //Authentication type
                
        //WEP Key settings (key len, key format, key data)        
        }
    else if(wlanSecModeRand == 3)
        {
        iWLanSecMode = EWlanSecMode802_1x; ///< 802.1x security mode
        }
    else if(wlanSecModeRand == 4)
        {
        iWLanSecMode = EWlanSecModeWpa;///< WPA security mode
        }
    else if(wlanSecModeRand == 5)
        {
        iWLanSecMode = EWlanSecModeWpa2;///< WPA@ security mode
        }
    }

void CConnectionSettingsParameters::SetRandomLoginNameL(CCreatorEngine& aEngine)
    {
    iLoginName->Des() = aEngine.RandomString(CCreatorEngine::EFirstName);
    iLoginName->Des().LowerCase();
    }

void CConnectionSettingsParameters::SetRandomLoginPassL(CCreatorEngine& aEngine)
    {
    iLoginPass->Des() = aEngine.RandomString(CCreatorEngine::ESurname);
    iLoginPass->Des().LowerCase();
    }

void CConnectionSettingsParameters::SetRandomSecureAuthenticationL(CCreatorEngine& /*aEngine*/)
    {
    iSecureAuthentication = EFalse;
    }

void CConnectionSettingsParameters::SetRandomPromptPasswordL(CCreatorEngine& /*aEngine*/)
    {
    iPromptPassword = ETrue;
    }

void CConnectionSettingsParameters::SetRandomUseProxyL(CCreatorEngine& /*aEngine*/)
    {
    iUseProxy = ETrue;
    }

void CConnectionSettingsParameters::SetRandomProxyAddressL(CCreatorEngine& /*aEngine*/)
    {
    iProxyServerAddress->Des() = _L("127.0.0.1");
    }

void CConnectionSettingsParameters::SetRandomProxyPortL(CCreatorEngine& /*aEngine*/)
    {
    iProxyPortNumber = 80;
    }

void CConnectionSettingsParameters::SetRandomStartPageL(CCreatorEngine& aEngine)
    {
    LOGSTRING("Creator: CConnectionSettingsParameters::SetRandomStartPageL");

    TInt num = aEngine.RandomNumber(1, 3);
    TBuf<10> apType;

    if (num==1)
        {
        apType = _L("mms");
        }            
    else if (num==2)
        {
        apType = _L("wap");
        }
    else
        {
        apType = _L("internet");
        }
    TBuf<160> company = aEngine.RandomString(CCreatorEngine::ECompany);    
    iStartPage->Des() = _L("http://");
    iStartPage->Des().Append(apType);
    iStartPage->Des().Append(_L("."));
    iStartPage->Des().Append(company);
    iStartPage->Des().Append(_L(".com"));
    iStartPage->Des().LowerCase();
    }

void CConnectionSettingsParameters::SetRandomIPAddressL(CCreatorEngine& /*aEngine*/)
    {
    iIPAddr->Des() = _L("10.10.10.10");
    }

void CConnectionSettingsParameters::SetRandomIP4NameServer1L(CCreatorEngine& /*aEngine*/)
    {
    iIP4NameServer1->Des() = _L("10.1.1.1");
    }
void CConnectionSettingsParameters::SetRandomIP4NameServer2L(CCreatorEngine& /*aEngine*/)
    {
    iIP4NameServer2->Des() = _L("10.2.2.2");
    }

void CConnectionSettingsParameters::SetRandomProtocolTypeL(CCreatorEngine& aEngine)
    {
    LOGSTRING("Creator: CConnectionSettingsParameters::SetRandomProtocolTypeL");

    TInt protocolTypeRand = aEngine.RandomNumber(1, 2);
    if(protocolTypeRand == 1)
        {
        iProtocolType = RPacketContext::EPdpTypeIPv4;
        }
    else
        {
        iProtocolType = RPacketContext::EPdpTypeIPv6;
        }
    }

void CConnectionSettingsParameters::SetRandomTelephoneNumberL(CCreatorEngine& aEngine)
    {
    iDefaultTelNumber->Des() = aEngine.RandomString(CCreatorEngine::EPhoneNumber);
    }

void CConnectionSettingsParameters::SetRandomBearerTypeIsdnL(CCreatorEngine& /*aEngine*/)
    {
    iBearerCallTypeIsdn = ECmCallTypeISDNv110;
    }

void CConnectionSettingsParameters::SetRandomBearerSpeedL(CCreatorEngine& /*aEngine*/)
    {
    iBearerSpeed = ECmSpeed14400;
    }

void CConnectionSettingsParameters::SetRandomWapWspOptionL(CCreatorEngine& /*aEngine*/)
    {
    iWapWspOption = ECmWapWspOptionConnectionless;
    }

void CConnectionSettingsParameters::SetRandomSubnetMaskL(CCreatorEngine& /*aEngine*/)
    {
    iSubnetMask->Des() = _L("255.255.255.0");
    }

void CConnectionSettingsParameters::SetRandomGatewayAddressL(CCreatorEngine& /*aEngine*/)
    {
    iGatewayAddress->Des() = _L("10.0.0.1");
    }

void CConnectionSettingsParameters::SetRandomWlanIpAddrL(CCreatorEngine& /*aEngine*/)
    {
    iWlanIpAddr->Des() = _L("10.0.0.99");
    }

void CConnectionSettingsParameters::SetRandomIPv6NameServer1L(CCreatorEngine& /*aEngine*/)
    {
    iIP6NameServer1->Des() = _L("2007:abcd:1000:2000:3000:4000:5000:6001");
    }

void CConnectionSettingsParameters::SetRandomIPv6NameServer2L(CCreatorEngine& /*aEngine*/)
    {
    iIP6NameServer2->Des() = _L("2007:abcd:1000:2000:3000:4000:5000:6002");
    }

void CConnectionSettingsParameters::SetRandomDisableTextAuthL(CCreatorEngine& /*aEngine*/)
    {
    iDisableTextAuth = EFalse;
    }

void CCreatorConnectionSettings::SetRandomBearerTypeL(CConnectionSettingsParameters& parameters)
	{
    LOGSTRING("Creator: CCreatorConnectionSettings::SetRandomBearerTypeL");

	RArray<TUint32> supportedBearers = RArray<TUint32>( 10 );
	iCmManagerExt.SupportedBearersL(supportedBearers);
	CleanupClosePushL( supportedBearers );
	    
	//In random mode, skipping KUidLanBearerType
	//Because of Emulator migh show it as supported, when it's really not
	//and that causes weird situation in UI when there is an item, but it can't be opened
	TInt lanID = supportedBearers.Find(KUidLanBearerType);
	if( lanID != KErrNotFound )
		{
		supportedBearers.Remove(lanID);
		}
	
	// in emulator remove WLAN
	#ifdef __WINSCW__	
	TInt wlanID = supportedBearers.Find(KUidWlanBearerType);
	if( wlanID != KErrNotFound )
		{
		supportedBearers.Remove(wlanID);
		}
	#endif	
				
	/*
	    for(TInt i = 0; i<supportedCount; i++)
	        {
	        TUint bearer = supportedBearers[i];
	        if(bearer == KUidLanBearerType)
	            {
	            supportedBearers.Remove(i);
	            }
	        }
	    */
	    
	//Select random bearer type. 
	//Also possible to use SupportedBearersL() -method to find supported methods
	//but using that will cause problems when setting attributes.
	TInt supportedCount = supportedBearers.Count();    
	TInt bearerTypeID = iEngine->RandomNumber(0, supportedCount-1);
	    
	TUint bearerType = supportedBearers[bearerTypeID];
	    
	//Bearer types KUidEmbeddedDestination and KPluginVPNBearerTypeUid is not supported
	if(bearerType == KUidWlanBearerType)//Wireless LAN
		{            
		parameters.iBearerType = KUidWlanBearerType;
		}
	else if(bearerType == KUidPacketDataBearerType)//Packet data (GPRS)
		{
		parameters.iBearerType = KUidPacketDataBearerType;
		}
	else if(bearerType == KUidCSDBearerType)//Data Call
		{
		parameters.iBearerType = KUidCSDBearerType;
		}
	else if(bearerType == KUidHSCSDBearerType) //High speed GSM
		{
		parameters.iBearerType = KUidHSCSDBearerType;
		}
	else if(bearerType == KUidEmbeddedDestination)
		{
		parameters.iBearerType = KUidEmbeddedDestination;
		}
	else// if(bearerType == KPluginVPNBearerTypeUid)
		{
		parameters.iBearerType = KPluginVPNBearerTypeUid;
		}
	/* In random mode skipping LAN type
	    else// if(bearerType == KUidLanBearerType)
	        {
	        iBearerType = KUidLanBearerType;
	        }*/
	    
	CleanupStack::PopAndDestroy(); // supportedBearers
	}

void CCreatorConnectionSettings::SetRandomParametersL(CConnectionSettingsParameters& parameters)
    {
    LOGSTRING("Creator: CCreatorConnectionSettings::SetRandomParametersL");
        
    // Connection method name:
    parameters.SetRandomCMNameL(*iEngine);
    
    SetRandomBearerTypeL(parameters);
 
    if(parameters.iBearerType == KUidWlanBearerType)//Wireless LAN
        {           
        // WLAN name:
        parameters.SetRandomWLANNameL(*iEngine);
        
        //WLAN network mode        
        parameters.SetRandomWLANNetModeL(*iEngine);     
        
        //WLAN security mode:
        parameters.SetRandomWLANSecurityModeL(*iEngine);
        }
    
    // Login name:
    parameters.SetRandomLoginNameL(*iEngine);

    // Login password:
    parameters.SetRandomLoginPassL(*iEngine);

    // Secure authentication:
    parameters.SetRandomSecureAuthenticationL(*iEngine);
    
    // Prompt password:
    parameters.SetRandomPromptPasswordL(*iEngine);    

    // Use proxy:
    parameters.SetRandomUseProxyL(*iEngine);    
    
    // Proxy address
    parameters.SetRandomProxyAddressL(*iEngine);    
    
    // Proxy port number:
    parameters.SetRandomProxyPortL(*iEngine);    

    // Start page:
    parameters.SetRandomStartPageL(*iEngine);
       
    // IP address:
    parameters.SetRandomIPAddressL(*iEngine);    
    
    // IPv4 name servers:
    parameters.SetRandomIP4NameServer1L(*iEngine);
    parameters.SetRandomIP4NameServer2L(*iEngine);
      
    // Protocol type:
    parameters.SetRandomProtocolTypeL(*iEngine);
    
    // Default telephone number:
    parameters.SetRandomTelephoneNumberL(*iEngine);
         
    // Bearer isdn
    parameters.SetRandomBearerTypeIsdnL(*iEngine);

    // Bearer speed:
    parameters.SetRandomBearerSpeedL(*iEngine);    

    // WAP wsp option:
    parameters.SetRandomWapWspOptionL(*iEngine);

    // Subnet mask:
    parameters.SetRandomSubnetMaskL(*iEngine);
    
    // Gateway address:
    parameters.SetRandomGatewayAddressL(*iEngine);
    
    // WLAN ip address:
    parameters.SetRandomWlanIpAddrL(*iEngine);

    // IPv6 name servers:
    parameters.SetRandomIPv6NameServer1L(*iEngine);
    parameters.SetRandomIPv6NameServer2L(*iEngine);
    
    // Disable text authorization:
    parameters.SetRandomDisableTextAuthL(*iEngine);    
    }

//----------------------------------------------------------------------------

void CCreatorConnectionSettings::SetDefaultCML(RCmConnectionMethodExt conMethod)
    {
    LOGSTRING("Creator: CCreatorConnectionSettings::SetDefaultCM");

    TCmDefConnValue defConnValue;
        
    //Returns the default connection method/SNAP.
    TRAPD( err, iCmManagerExt.ReadDefConnL( defConnValue ) );
    if( err ==  KErrNotSupported )
        {
        return; //If Default Connection NOT supported, just returning 
        }
    else if ( err != KErrNone )
        {
        User::Leave( err ); //If something else happend
        }
    else if ( defConnValue.iId == 0 ) // default not set
        {       
        TCmDefConnValue newDefaultCM;        
        HBufC* cmName = conMethod.GetStringAttributeL( ECmName );        
        CleanupStack::PushL( cmName );
        newDefaultCM.iId = AccessPointNameToIdL( cmName->Des() );
        newDefaultCM.iType = ECmDefConnDestination;
        //Stores the default connection method/SNAP.
        iCmManagerExt.WriteDefConnL( newDefaultCM );
        CleanupStack::PopAndDestroy( cmName );        
       }
    }

//----------------------------------------------------------------------------

TUint32 CCreatorConnectionSettings::AccessPointNameToIdL(const TDesC& aAPName,
                                                         TBool aAnyIfNotFound )
    {
    LOGSTRING("Creator: CCreatorConnectionSettings::AccessPointNameToIdL");

    //RCmDestination destination
    
    RArray<TUint32> destIdArray = RArray<TUint32>( 10 );
    
    iCmManagerExt.AllDestinationsL( destIdArray );
    
    CleanupClosePushL( destIdArray );
    TUint32 id = 0 ;
    TBool found = EFalse;
    
    for( TInt i = 0; i < destIdArray.Count(); i++ )
        {
        RCmDestinationExt dest = iCmManagerExt.DestinationL( destIdArray[i] );           
        CleanupClosePushL( dest );        

        HBufC* destName = dest.NameLC();
        
        if (destName->CompareF(aAPName) == 0){        
         id = dest.Id();
         found = ETrue;
        }
        if ( !found && id == 0 && aAnyIfNotFound )
            {
            id = dest.Id();
            }
        CleanupStack::PopAndDestroy( 2 ); // destName , dest
        if(found)
            {
            break;
            }
        }

    CleanupStack::PopAndDestroy(); // destIdArray   
    return id;
    }

//----------------------------------------------------------------------------

void CCreatorConnectionSettings::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/) // from MMsvSessionObserver
    {
    LOGSTRING("Creator: CCreatorConnectionSettings::HandleSessionEventL");
    }

//----------------------------------------------------------------------------
void CCreatorConnectionSettings::DeleteAllL()
    {
    LOGSTRING("Creator: CCreatorConnectionSettings::DeleteAllL");
    DeleteAllDestinationsL( EFalse );
    }

//----------------------------------------------------------------------------
void CCreatorConnectionSettings::DeleteAllCreatedByCreatorL()
    {
    LOGSTRING("Creator: CCreatorConnectionSettings::DeleteAllCreatedByCreatorL");
    iEntryIds.Reset();
    
    // fetch ids of entries created by Creator
    iEngine->ReadEntryIdsFromStoreL( iEntryIds, KUidDictionaryUidIAP );
    
    // delete entries
    DeleteAllDestinationsL( ETrue );
    }

//----------------------------------------------------------------------------

void CCreatorConnectionSettings::DeleteAllDestinationsL( TBool aOnlyCreatedWithCreator )
    {
    LOGSTRING("Creator: CCreatorConnectionSettings::DeleteAllDestinationsL");
    RArray<TUint32> destIdArray(5);
    CleanupClosePushL( destIdArray );
    iCmManagerExt.AllDestinationsL( destIdArray );    
    
    for( TInt i = 0; i < destIdArray.Count(); i++ )
        {
        TRAP_IGNORE
            (
            RCmDestinationExt dest = iCmManagerExt.DestinationL( destIdArray[i] );           
            CleanupClosePushL( dest );
            
            if ( !aOnlyCreatedWithCreator || iEntryIds.Find( destIdArray[i]) != KErrNotFound )
                {
                dest.DeleteLD();
                }
            CleanupStack::PopAndDestroy(); // dest
            );
        }
   
    CleanupStack::PopAndDestroy(); // destIdArray 
    
    // reset must be done here, because iEntryIds is stored in destructor
    iEntryIds.Reset();
    
    // all entries deleted, remove the IMPS related registry
    iEngine->RemoveStoreL( KUidDictionaryUidIAP );
    }
//----------------------------------------------------------------------------
