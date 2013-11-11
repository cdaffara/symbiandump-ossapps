/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Class stores data for one accesspoint
*
*/



 
// INCLUDE FILES

#include "CWPAPNapdef.h"
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <wpapadapterresource.rsg>
#include <featmgr.h>
#include <WPAdapterUtil.h>
#include "CWPAPAccesspointItem.h"
#include "WPAPAdapter.pan"
#include <WlanCdbCols.h>
#include <EapType.h>
#include <EapSettings.h>
#include "ProvisioningDebug.h"
#include <cmconnectionmethodext.h>
#include <cmpluginpacketdatadef.h>
#include <cmpluginwlandef.h>
#include <cmmanagerext.h>
#include <cmconnectionmethodext.h>
#include <commdb.h>
#include <WlanCdbCols.h>
#include <EapExpandedType.h>
#include <centralrepository.h>
#include <pdpcontextmanagerinternalcrkeys.h>
#include <EapGeneralSettings.h>
#include <EapTypeDefinitions.h>

const TUint KIapColumn        = 0x00000100;
const TUint KLingerColumn     = 0x00000200;
const TUint KColumnMask       = 0xFFFFFF00;
const TUint KRowMask          = 0x000000FF;

// Delay for comms db begintransaction retry (microseconds)
const TInt KBeginTransRetryDelay = 1000000; 
// Maximum number of retries
const  TInt KBeginTransRetryCount = 7;      

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPAPNapdef::NewLC
// -----------------------------------------------------------------------------
//
CWPAPNapdef* CWPAPNapdef::NewLC( TBool aFollowingLink,
                                 const TDesC& aDefaultName, 
                                 CWPCharacteristic& aLogicalCharacteristic,
                                 CWPCharacteristic& aLink )
	{
	CWPAPNapdef* self = new(ELeave) CWPAPNapdef( aFollowingLink,
	                                             aDefaultName,
                                                 aLogicalCharacteristic ); 
	CleanupStack::PushL( self );
	self->ConstructL();
    aLink.AcceptL( *self );
	return self;
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CWPAPNapdef::~CWPAPNapdef()
	{
	FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::~CWPAPNapdef" ) );
    
    iEapTypeArray.ResetAndDestroy();
    iEapTypeArray.Close();
    iSecSSID.ResetAndDestroy();
	iSecSSID.Close();
    iNameServers.Close();
  
	}

// -----------------------------------------------------------------------------
// CWPAPNapdef::ValidateL
// -----------------------------------------------------------------------------
//
TBool CWPAPNapdef::ValidateL()
	{
	FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::ValidateL" ) );
	
    TBool result( ETrue );

	if ( iBearer == KUidWlanBearerType )
	    {	    	
	    FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::ValidateL wlan" ) );
	    if ( iWlanSupported )
	    	{	    	
	    	FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::ValidateL wlan supported" ) );
	    	return result;
	    	}
	    else
	    	{
	   		FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::ValidateL wlan NOT supported" ) );
	    	return EFalse;
	    	}
	    }
	    
	// Received unsupported bearer
	if( iBearerUnsupported )
	    {
	    return EFalse;
	    }
	    
	// Didn't receive a bearer value.
	if ( iBearer == 0 )
		{
    // Access point address type can also tell the bearer
		if( iAccesspointNameType )
            {
            TPtrC addrType( iAccesspointNameType->Value() );
            if( addrType == KAddrTypeAPN )
                {
                iBearer =KUidPacketDataBearerType;
                }
            }
        
        // If still couldn't determine the bearer, default to GPRS
        if( iBearer == 0 )
			{
			iBearer = KUidPacketDataBearerType;
			}
		}
		        
    // "NAP-ADDRESS" (Access point name) must be defined
    if( iAccesspointName )
        {
        if( iAccesspointName->Value() == KNullDesC )
            {
            return EFalse;
            }
        }
    else
        {
        return EFalse;
        }

	/* 
	Combinations of authentication type, username and password and corressponding
	value of prompt for password field. AuthType in this table means that provisioning document
	contained authtype parameter and it was supported by the implementation.
	Provisioning document names are used in table.
	x means that value of the specified parameter is received and it was valid.
	AuthType received & supported 
	| AuthName 
	| | AuthSecret 
	| |	| "Prompt for Password"
		  no
	x     no
      x   no
	x x   yes
        x no
    x   x yes
      x x no
    x x	x no
	*/

	// Set the "prompt password"-value. See the table above. Value is set to true
	// only if we have supported authentication type and either username or password
	// is empty.
	if ( IsReceived( EWPParameterAuthentication_id ) &&
	     ( ( !iPassword && iUserName ) ||( iPassword && !iUserName ) ) )
		{
		iPromptPassword = ETrue;	
		}

    // IFNETWORKS is a list of possible protocols that a GPRS access points can be 
    // used for. 
    if( iBearer == KUidPacketDataBearerType && IsReceived( EWPParameterIfNetworks_id ) )
        {
        TLex lex( iIfNetworks->Value() );
        lex.Mark();
        do
            {
            TChar ch( lex.Get() );

            if( ( !iPdpType && !ch.IsAlphaDigit() ) || ( !iPdpType && lex.Eos() ) )
                {
				if(!lex.Eos())
					{
					lex.UnGet();
					}
                TPtrC value( lex.MarkedToken() );
				if( !lex.Eos() )
					{
					lex.Inc();
					}
                
                if( value == KIPv6 
                    && FeatureManager::FeatureSupported( KFeatureIdIPv6 ) )
                    {
                    //iPdpType = EIPv6;
                    // CMManager
                    iPdpType = RPacketContext::EPdpTypeIPv6;
                    result = ETrue;
                    }
                else if( value == KIPv6 
                    && !FeatureManager::FeatureSupported( KFeatureIdIPv6 ) )
                    {
                    result = EFalse;
                    }
                else if( value == KIPv4 )
                    {
                    //iPdpType = EIPv4;
                    // CMManager
                    iPdpType = RPacketContext::EPdpTypeIPv4;
                    result = ETrue;
                    }
                }
            } while( !lex.Eos() );
        }

	return result;
	}

// -----------------------------------------------------------------------------
// CWPAPNapdef::AddItemsL
// -----------------------------------------------------------------------------
//
TInt CWPAPNapdef::AddItemsL( RPointerArray<CWPAPAccesspointItem>& aItems,
                            CWPAPItemBase* aLogicalProxy,
                            CWPAPItemBase* aPhysicalProxy ) 
	{
	FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::AddItemsL" ) );
	
    TInt count( 0 );

    if( ( iFollowingLink || IsReceived( EWPParameterInternet_id ) 
                         || IsReceived( EWPParameterWlan_id ) )
                         && ValidateL() )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::AddItemsL append" ) );
        CWPAPAccesspointItem* item = CWPAPAccesspointItem::NewLC(
                                                             iDefaultName,
                                                             aLogicalProxy,
                                                             aPhysicalProxy,
                                                             this,
                                                             iCharacteristic );
        User::LeaveIfError( aItems.Append( item ) );
        //Internet Parameter is received
        if (IsReceived(EWPParameterInternet_id) && !iFollowingLink)
            {
            SetInternetIndicator(aItems.Count() - 1);
            }

        
        count++;
        CleanupStack::Pop( item );
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::AddItemsL append done" ) );
        }
    return count;
    }


// -----------------------------------------------------------------------------
// CWPAPNapdef::SetInternetIndicator
// -----------------------------------------------------------------------------
//
void CWPAPNapdef::SetInternetIndicator(TInt aValue)
    {
    iInternetIndicator = aValue;
    }

// -----------------------------------------------------------------------------
// CWPAPNapdef::GetInternetIndicator
// -----------------------------------------------------------------------------
//
TInt CWPAPNapdef::GetInternetIndicator()
    {
    return iInternetIndicator;
    }


// -----------------------------------------------------------------------------
// CWPAPNapdef::AddDataL
// -----------------------------------------------------------------------------
//
void CWPAPNapdef::AddDataL( /*CApAccessPointItem& aAPItem*/ RCmConnectionMethodExt& aCmItem ) 
	{
	FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::AddDataL" ) );
	
	TInt err( KErrNone );

    if ( iBearer == KUidWlanBearerType )    
	    {			    	
	    	if ( iWlanSupported )
        	{
        	// CMManager
        	AddWlanDataL( aCmItem);
        	}
        else
        	{
        	// WLAN is disabled (or does not exist) in the device.
        	FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::AddDataL WLAN disabled." ) );
        	User::Leave( KErrNotSupported );
        	}
	    }
    else
        {	    
    	if ( iConnectionName )
    		{
    		// CMManager
    		aCmItem.SetStringAttributeL( CMManager::ECmName, (iConnectionName->Value().Left( KNameMaxLength ))); 
    		}
    	else // Use default name
    		{
    		// CMManager
    		aCmItem.SetStringAttributeL( CMManager::ECmName, iDefaultName );
    		}
    	
    	if ( iAccesspointName )
    		{
    		if ( iBearer == KUidPacketDataBearerType )
    			{
    			// CMManager
    			TRAP ( err, aCmItem.SetStringAttributeL( CMManager::EPacketDataAPName, iAccesspointName->Value() ));
                FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::AddDataL value: %S, err: %d"),
                                                                     &iAccesspointName->Value(), err));    			                              
    			}
    		else
    			{
    			// CMManager
    			TRAP ( err, aCmItem.SetStringAttributeL( CMManager::EDialDefaultTelNum, iAccesspointName->Value() ));
                FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::EApIspDefaultTelNumber value: %S, err: %d"),
                			      &iAccesspointName->Value(), err ) );
    			}
    		}
    		
    	TRAP ( err, aCmItem.UpdateL() );

    	 if ( iUserName )
    		{
    		// CMManager
    		TRAP ( err, aCmItem.SetStringAttributeL( CMManager::EDialLoginName, iUserName->Value() ));
    		FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::AddDataL EDialLoginName value: %S, err: %d"),
    		                    		      &iUserName->Value(), err));
    		TRAP ( err, aCmItem.SetStringAttributeL( CMManager::EDialIFAuthName, iUserName->Value() ));
    		FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::AddDataL EDialIFAuthName value: %S, err: %d"),
    		                    		      &iUserName->Value(), err));
    		TRAP ( err, aCmItem.SetStringAttributeL( CMManager::ECmIFAuthName, iUserName->Value() ));
    		FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::AddDataL ECmIFAuthName value: %S, err: %d"),
    		                    		      &iUserName->Value(), err));
    		}
    		
		// CMManager
    	TRAP ( err, aCmItem.SetBoolAttributeL( CMManager::ECmIFPromptForAuth, iPromptPassword));
    	FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::AddDataL ECmIFPromptForAuth value: %d, err: %d"),
    	                            	      iPromptPassword, err ));

    	if ( iPassword )
    		{
    		// CMManager 
    		TRAP ( err, aCmItem.SetStringAttributeL( CMManager::EDialLoginPassword, iPassword->Value() ));
    		FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::AddDataL EDialLoginPassword value: %d, err: %d"),
    		                		      &iPassword->Value(), err ) );
    		TRAP ( err, aCmItem.SetStringAttributeL( CMManager::ECmIFAuthPass, iPassword->Value() ));
    		FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::AddDataL ECmIFAuthPass value: %d, err: %d"),
    		                		      &iPassword->Value(), err ) );
    		}	

		// CMManager
    	TRAP ( err, aCmItem.SetBoolAttributeL( CMManager::ECmDisablePlainTextAuth, iSecureAuthentication ));
        FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::AddDataL EApEApIspDisablePlainTextAuth value: %d, err: %d"),    	                         
                	                        iSecureAuthentication, err ) );
                	                        
			
        // IPv6 APs always have dynamic phone address
    	//CMManager
    	if ( iPhoneIPAddress && iPdpType != RPacketContext::EPdpTypeIPv6 )
    		{
    		// CMManager IS CORRECT?
    		//err = aAPItem.WriteTextL( EApIspIPAddr, iPhoneIPAddress->Value() );
    		TRAP ( err, aCmItem.SetStringAttributeL( CMManager::ECmIPAddress, iPhoneIPAddress->Value() ));
    		FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::AddDataL EApIspIPAddr value: %S, err: %d"),
                        		     &iPhoneIPAddress->Value(), err ) );
    		}

		TRAP ( err, aCmItem.UpdateL() );
        for( TInt i( 0 ); i < iNameServers.Count(); i++ )
            {
            WriteDNSAddressL( aCmItem, iNameServers[i]->Value() );
            }
		TRAP ( err, aCmItem.UpdateL() );
        if( FeatureManager::FeatureSupported( KFeatureIdIPv6 )
            && iPdpType )
            {
         	// CMManager
            TRAP ( err, aCmItem.SetIntAttributeL( CMManager::EPacketDataPDPType, iPdpType ));
            }
    
    if ( err ) err = 0; // prevent compiler warning
        }
	FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::AddDataL" ) );
	}

// -----------------------------------------------------------------------------
// CWPAPNapdef::AddWlanDataL
// -----------------------------------------------------------------------------
//
void CWPAPNapdef::AddWlanDataL( RCmConnectionMethodExt& aCmItem )
    {
    FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::AddWlanDataL" ) );
    
    if ( !iWlanSupported )
    	{
    		FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::AddWlanDataL WLAN NOT supported." ) );
    		User::Leave( KErrNotSupported);
    	}
        
    TInt err = KErrNone;
    
	if ( iConnectionName )
		{
		FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::AddWlanDataL iConnectionName" ) );
		
		//Connection name
		// CMManager
		aCmItem.SetStringAttributeL( CMManager::ECmName, iConnectionName->Value().Left( KNameMaxLength ) );
		
		// CMManager 
		// Network name
		aCmItem.SetStringAttributeL( CMManager::EWlanSSID, iConnectionName->Value());
		
        FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::AddWlanDataL iConnectionName err (%d) (%S)"), err, &iConnectionName->Value()));
				
		}
	else // Use default name
		{
		FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::AddWlanDataL iConnectionName default" ) );
		aCmItem.SetStringAttributeL( CMManager::EWlanSSID, iDefaultName);
		}
		   	
   	//EApWlanNetworkMode Gives network mode, TUint32
   	FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::AddWlanDataL EApWlanNetworkMode" ) );
	// CMManager
   	TRAP ( err, aCmItem.SetIntAttributeL( CMManager::EWlanConnectionMode, iNetMode ));
   	FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::AddWlanDataL EApWlanNetworkMode err (%d)"), err));
        
    //EApWlanSecurityMode The security mode, TUint32
    if ( iSecMode == 0 )
        {
        // This is needed if Provisioning message does not contain SECMODE parameter
        // AP does not work if secmode is not set
        iSecMode = CMManager::EWlanSecModeOpen;
        }
        
    FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::AddWlanDataL EApWlanSecurityMode" ) );
   	// CMManager, securitymode causes error -1, this will be set manually in SaveWlanData
   	//TRAP ( err, aCmItem.SetIntAttributeL( CMManager::EWlanSecurityMode, iSecMode ));
    FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::AddWlanDataL EApWlanSecurityMode err (%d)"), err));
    	
    if (err)
        {
        err = KErrNone;
        }
    }

// -----------------------------------------------------------------------------
// CWPAPNapdef::WriteDNSAddressL
// -----------------------------------------------------------------------------
//
void CWPAPNapdef::WriteDNSAddressL( /*CApAccessPointItem& aAPItem*/ RCmConnectionMethodExt& aCmItem,
                                    const TDesC& aValue )
	{
    // GPRS access points supports IPv6 only if explicitly stated. 
    // CMManager
    TBool apSupportsIPv6( iPdpType == RPacketContext::EPdpTypeIPv6 | iBearer != KUidPacketDataBearerType );

    // If IPv6 is supported, write the DNS address as IPv6 if
    // bearer supports IPv6 and the address format is IPv6    
    if( FeatureManager::FeatureSupported( KFeatureIdIPv6 )
        && apSupportsIPv6
        && WPAdapterUtil::CheckIPv6( aValue ) )
        {
        // Two name servers per network type
        if( iNumIPv6NameServers < KTwoNameServersPerNetworkType )
            {
            // CMManager
            TRAPD (err, aCmItem.SetStringAttributeL( CMManager::TConnectionMethodCommonAttributes( 
            													( CMManager::ECmIP6NameServer1 ) + iNumIPv6NameServers ), aValue ) );
            User::LeaveIfError( err );
            iNumIPv6NameServers++;
            }
        }
     else if( CheckIPv4values( aValue) )
        {
        // Two name servers per network type
        if( iNumIPv4NameServers < KTwoNameServersPerNetworkType )
            {
            // CMManager 
            TRAPD ( err, aCmItem.SetStringAttributeL( CMManager::TConnectionMethodCommonAttributes( 
            													( CMManager::ECmIPNameServer1 ) + iNumIPv4NameServers ), aValue ) );
            User::LeaveIfError( err );
            aCmItem.SetBoolAttributeL( CMManager::ECmIPDNSAddrFromServer, EFalse);
            iNumIPv4NameServers++;
            }
        }
	}
// -----------------------------------------------------------------------------
// CWPAPNapdef::CheckAddressPart
// -----------------------------------------------------------------------------
//

TBool CWPAPNapdef:: CheckAddressPart( TLex& aLex )
    {
    TBool result( aLex.Peek().IsDigit() );
    aLex.Inc();
    if( aLex.Peek().IsDigit() )
        {
        aLex.Inc();
        if( aLex.Peek().IsDigit() )
            {
            aLex.Inc();
            }
        }

    return result;
    }


// -----------------------------------------------------------------------------
// CWPAPNapdef::CheckIPv4values
// -----------------------------------------------------------------------------
//

TBool CWPAPNapdef::CheckIPv4values( const TDesC& aValue )
{
	
	TLex aLex( aValue );
	return CheckAddressPart( aLex ) 
        && aLex.Get() == '.'
        && CheckAddressPart( aLex ) 
        && aLex.Get() == '.'
        && CheckAddressPart( aLex ) 
        && aLex.Get() == '.'
        && CheckAddressPart( aLex )
        && aLex.Get() == '\0';
     
}
// -----------------------------------------------------------------------------
// CWPAPNapdef::Name
// -----------------------------------------------------------------------------
//
const TDesC& CWPAPNapdef::Name()
	{
	if ( iConnectionName )
		{
		return iConnectionName->Value();
		}
	else
		{
		return KNullDesC;
		}
	}

// -----------------------------------------------------------------------------
// CWPAPNapdef::VisitL
// -----------------------------------------------------------------------------
//
void CWPAPNapdef::VisitL( CWPCharacteristic& aCharacteristic )
	{
	FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::VisitL(CWPCharacteristic)" ) );
	
	TInt type = aCharacteristic.Type();
    FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::VisitL(CWPCharacteristic) type (%d)"), type));
	switch ( type )
		{
		case KWPNapAuthInfo:
		case KWPValidity:				
			{
			FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::VisitL(CWPCharacteristic) normal" ) );
			aCharacteristic.AcceptL( *this );
			break;
			}
		case KWPWLAN:
		case KWPWepKey:
			{
			if( iWlanSupported )
				{
					FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::VisitL(CWPCharacteristic) accept WLAN characteristic" ) );
					aCharacteristic.AcceptL( *this );
				}
			}
			break;
		default:
			{
			if ( ( ( ( aCharacteristic.Name().Compare( KWLAN ) ) == 0 )    ||
			     ( ( aCharacteristic.Name().Compare( SECSSID ) ) == 0 )  ||
			     ( ( aCharacteristic.Name().Compare( KEAP ) ) == 0 )     ||
			     ( ( aCharacteristic.Name().Compare( KCERT ) ) == 0 )    ||
			     ( ( aCharacteristic.Name().Compare( KWEP ) ) == 0 ) ) && iWlanSupported )
			    {
			    if(aCharacteristic.Name().Compare( KEAP ) == 0)
			        {
                    CEapTypeElement* newEap = new (ELeave) CEapTypeElement;
                    newEap->iEAPSettings = new (ELeave) EAPSettings;
               	    newEap->iCertificate = new (ELeave) EapCertificateEntry;
                    iEapTypeArray.AppendL(newEap);
			        }
			    else if(aCharacteristic.Name().Compare( SECSSID ) == 0)
			        {
                    SECssID* newsecSSID = new (ELeave) SECssID;
                    newsecSSID->iSSSID = NULL;
                    newsecSSID->iSUSSID = NULL;
                    iSecSSID.AppendL(newsecSSID);
			        }
    
			    aCharacteristic.AcceptL( *this );
			    }
			}
		}
		
	FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::VisitL(CWPCharacteristic) done" ) );
	}

// -----------------------------------------------------------------------------
// CWPAPNapdef::VisitL
// -----------------------------------------------------------------------------
//
void CWPAPNapdef::VisitL( CWPParameter& aParameter )
	{
	TInt id = aParameter.ID();
	TPtrC value( aParameter.Value() );

	FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::VisitL(CWPParameter) id: %d value: %S"), id, &value));

	switch ( id )
		{
		case EWPParameterName:
			{
			if ( !iConnectionName )
				{
				iConnectionName = &aParameter;
				}
			break;
			}
		case EWPParameterBearer:
			{
			if ( iBearer != 0 || iBearerUnsupported )
				{
				break; // Already received a bearer information
				}

			if ( value == KGSMGPRS )
				{
				iBearer = KUidPacketDataBearerType;
				}
			else if ( value == KWLAN && iWlanSupported )
				{
				iBearer = KUidWlanBearerType;
				MarkReceived( EWPParameterWlan_id );
				}				
			else
				{
				// Unsupported bearer type
				iBearerUnsupported = ETrue;
				}
			break;
			}
		case EWPParameterNapAddress:
			{
			if ( !iAccesspointName )
				{
				iAccesspointName = &aParameter;
				}
			break;
			}
		case EWPParameterLinger:
			{
			iLinger = &aParameter;
			UdateligerValueL( iLinger );
			break;
			}
		case EWPParameterNapAddrType:
			{
			if ( !iAccesspointNameType )
				{
				iAccesspointNameType = &aParameter;
				}
			break;
			}
		case EWPParameterAuthName:
			{
			if ( !iUserName )
				{
				iUserName = &aParameter;
				}
			break;
			}
		case EWPParameterAuthSecret:
			{
			if ( !iPassword )
				{
				iPassword = &aParameter;
				}
			break;
			}
		case EWPParameterAuthType:
			{
			// If already received don't accept
			if (IsReceived(EWPParameterAuthentication_id))
				{
				break;
				}

			if ( value == KCHAP || value == KMD5 )
				{
				iSecureAuthentication = ETrue;
				MarkReceived( EWPParameterAuthentication_id );
				}
			else if ( value == KPAP )
				{
				iSecureAuthentication = EFalse;
				MarkReceived( EWPParameterAuthentication_id );
				}
			else
				{
				// Not supported...
				}
			break;
			}
		case EWPParameterLocalAddr: // iPhoneIPAddress
			{
			if ( !iPhoneIPAddress )
				{
				iPhoneIPAddress = &aParameter;
				}
			break;
			}
		case EWPParameterDNSAddr:
			{
            // All name servers must be stored, as some of them might
            // be IPv6 and some IPv4
            User::LeaveIfError( iNameServers.Append( &aParameter ) );
			break;
			}
        case EWPParameterIfNetworks:
            {
            if( !iIfNetworks )
                {
                iIfNetworks = &aParameter;
                MarkReceived( EWPParameterIfNetworks_id );
                }
            break;
            }
        case EWPParameterInternet:
            {
            MarkReceived( EWPParameterInternet_id );
            break;
            }
//JMan for WLan
        case EWPParameterNapID:
            {
            if ( !iNapID )
                {
                iNapID = &aParameter;
                }
            break;
            }    
        // Here case 0 are handled the WLAN parameters that are extensions  
        // to OMA Client Provisioning parameter set.
        case 0:
            {
        if( iWlanSupported )
        	{
        	HandleWLanParametersL( aParameter );
        	}
        break;
            }
//JMan for WLan ends
		default:
			{
			}
		}
	}

// -----------------------------------------------------------------------------
// CWPAPNapdef::HandleWLanParametersL
// -----------------------------------------------------------------------------
//
void CWPAPNapdef::HandleWLanParametersL( CWPParameter& aParameter )
	{
	FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL" ) );
	
	if( !iWlanSupported )
		{
		FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL WLAN not supported." ) );
		User::Leave( KErrNotSupported );
		}
	
	SECssID* scssid = NULL;
	if( iSecSSID.Count() )
	{
		scssid = iSecSSID[iSecSSID.Count()-1];
	}
	
	TPtrC value( aParameter.Value() );
    if( ( aParameter.Name().Compare( PRISSID ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL iPriSSID" ) );
        if ( !iPriSSID )
            {
            iPriSSID = &aParameter;
            }
        }
    else if( ( aParameter.Name().Compare( PRIUSSID ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL iPriUSSID" ) );
        if ( !iPriUSSID )
            {
            iPriUSSID = &aParameter;
            }
        }// else if

    else if( ( aParameter.Name().Compare( PRIHSSID ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL iPriHSSID" ) );
        if ( !iPriHSSID )
            {
            iPriHSSID = &aParameter;
            }
        }// else if
        
    else if( ( aParameter.Name().Compare( SSSID ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL iPriHSSID" ) );
        if(scssid!=NULL)
        	{
        	scssid->iSSSID = &aParameter;
        	}
			  

        }// else if

    else if( ( aParameter.Name().Compare( SUSSID ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL iPriHSSID" ) );
			  if(scssid!=NULL)
        	{
        	scssid->iSUSSID = &aParameter;
        	}
        }// else if
                        
    else if( ( aParameter.Name().Compare( NETMODE ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL netmode" ) );
        if ( value == KADHOC )
			{
			FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL netmode adhoc" ) );
			iNetMode = CMManager::EAdhoc;
			}
        else // default is INFRA
            {
            FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL netmode infra" ) );
            iNetMode =CMManager::EInfra;
            }
        }

    else if( ( aParameter.Name().Compare( SECMODE ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL secmode" ) );
        if ( value == KWEPSEC )
			{
			FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL secmode KWEPSEC" ) );
			iSecMode = CMManager::EWlanSecModeWep;
			}
        else if( value == K8021XSEC )
            {
            FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL secmode K8021XSEC" ) );
			iSecMode = CMManager::EWlanSecMode802_1x;
            }
        else if( value == KWPASEC )
            {
            FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL secmode KWPASEC" ) );

			iSecMode = CMManager::EWlanSecModeWpa;
            }
        else if( value == KWPA2SEC )
            {
            FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL secmode KWPA2SEC" ) );
			iSecMode = CMManager::EWlanSecModeWpa2;
            }
        else if( value == KWPAPRESSEC )
            {
            FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL secmode KWPAPRESSEC" ) );
			iSecMode = CMManager::EWlanSecModeWpa;
            }
        else if( value == KWPA2PRESSEC )
            {
            FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL secmode KWPA2PRESSEC" ) );
			iSecMode = CMManager::EWlanSecModeWpa2;
            }          
        else
            {
            FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL secmode open" ) );
			iSecMode = CMManager::EWlanSecModeOpen;
            }
        }

    else if( ( aParameter.Name().Compare( WPAPRESKEYASC ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL WPAPRESKEYASC" ) );
        if ( !iWpaPresKeyAsc )
            {
            iWpaPresKeyAsc = &aParameter;
            }
        }// else if
        // indicates the default wepkey index
    else if( ( aParameter.Name().Compare( DEFAULTWEPIND ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL wepind" ) );

        TLex lex( value );
        lex.Val( iDefaultWepKeyIndex );
        if ( iDefaultWepKeyIndex >= 4 )
            {
            iDefaultWepKeyIndex = 0;
            }
        }// else if
        
    else if( ( aParameter.Name().Compare( WEPAUTHMODE ) ) == 0 )
        {
        if ( value == KWEPAUTHMODEOPEN )
			{
			FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL wepauthmode OPEN" ) );
			iAuthentication = EAuthOpen;
			}
		else if ( value == KWEPAUTHMODESHARED )
		    {
		    FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL wepauthmode SHARED" ) );
			iAuthentication = EAuthShared;
		    }
        }// else if
        
    else if( ( aParameter.Name().Compare( WEPKEYLENGTH ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL WEPKEYLENGTH" ) );
        if ( !iWepKeyLength )
            {
            iWepKeyLength = &aParameter;
            }
        }// else if
    else if( ( aParameter.Name().Compare( WEPKEYINDEX ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL WEPKEYINDEX" ) );
        TLex lex( value );
        lex.Val( iWepInd );
        }// else if 
    else if( ( aParameter.Name().Compare( WEPKEYDATA ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL WEPKEYDATA" ) );

        if ( iWepInd <  4 )
            {
            const TUint8 K40Bits = 5;
            const TUint8 K104Bits = 13;
            const TUint8 K232Bits = 29;

            TBuf8<KMaxLengthOfKeyData> buf8;
            StrCopy( buf8, aParameter.Value() );
            
            if(buf8.Length() == K232Bits ||
                buf8.Length() == K104Bits ||
                buf8.Length() == K40Bits  )
                {
                FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL ASCI Wep Key" ) );
                HBufC8* buf8Conv = HBufC8::NewLC( KMaxLengthOfKeyData );
                ConvertAsciiToHex( buf8, buf8Conv );
                iKeyData[iWepInd] = *buf8Conv;
                iKeyFormat[iWepInd] = CWPAPNapdef::EAscii;
                CleanupStack::PopAndDestroy( buf8Conv );
                }
            else if( buf8.Length()==K232Bits*2 ||
                buf8.Length()== K104Bits*2 ||
                buf8.Length() == K40Bits*2)
                {
                FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL HEX Wep Key" ) );
                iKeyData[iWepInd] = buf8;
                iKeyFormat[iWepInd] = CWPAPNapdef::EHexadecimal;
                }
            }
        
        }// else if	
    
    // Handle EAP parameter
    HandleEAPParametersCCL( aParameter );        
    
    FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleWLanParametersL done" ) );
	}

// -----------------------------------------------------------------------------
// CWPAPNapdef::HandleEAPParametersL
// -----------------------------------------------------------------------------
//
void CWPAPNapdef::HandleEAPParametersCCL( CWPParameter& aParameter )
    {
	FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL" ) );
   	
   	CEapTypeElement* eap = NULL;
   	if ( iEapTypeArray.Count() )
   	    {
   	    eap = iEapTypeArray[iEapTypeArray.Count()-1];
   	    }
   	if(!eap)
   	    {
   	    return;
   	    }    
   	
	TPtrC value( aParameter.Value() );
	
    if( ( aParameter.Name().Compare( EAPTYPE ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPTYPE" ) );
              
        ConvertEAPStringToIds( value, eap->iEapTypeString, eap->iEAPSettings->iEAPExpandedType );
        
        }
    else if( ( aParameter.Name().Compare( EAPUSERNAME ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPUSERNAME" ) );
        if ( eap->iEAPSettings->iUsername.Length() == 0 )
            {
            eap->iEAPSettings->iUsername = value;
            eap->iEAPSettings->iUsernamePresent = ETrue;
            eap->iEAPSettings->iUseAutomaticUsernamePresent = ETrue;
            eap->iEAPSettings->iUseAutomaticUsername = EFalse;
            }
        }// else if 
    else if( ( aParameter.Name().Compare( EAPPASSWORD ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPPASSWORD" ) );
        if ( eap->iEAPSettings->iPassword.Length() == 0 )
            {
            eap->iEAPSettings->iPassword = value;
            eap->iEAPSettings->iPasswordPresent = ETrue;
            eap->iEAPSettings-> iShowPassWordPromptPresent = ETrue;
            eap->iEAPSettings-> iShowPassWordPrompt = EFalse;
            }
        }// else if 
    else if( ( aParameter.Name().Compare( EAPREALM ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPREALM" ) );

        if ( eap->iEAPSettings->iRealm.Length() == 0 )
            {
            eap->iEAPSettings->iRealm = value;
            eap->iEAPSettings->iRealmPresent = ETrue;
            eap->iEAPSettings->iUseAutomaticRealmPresent = ETrue;
            eap->iEAPSettings->iUseAutomaticRealm = EFalse;
            }
        }// else if 
    else if( ( aParameter.Name().Compare( EAPUSEPSEUD ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPUSEPSEUD" ) );
        TInt pseudonyms;
        TLex lex( value );
        lex.Val( pseudonyms );
        
        eap->iEAPSettings->iUsePseudonymsPresent = ETrue;
        if ( pseudonyms == 1 )
            {        
            eap->iEAPSettings->iUsePseudonyms = ETrue;
            }
        else
            {
            eap->iEAPSettings->iUsePseudonyms = EFalse;            
            }
        }// else if 
    else if( ( aParameter.Name().Compare( EAPMAXAUTHS ) ) == 0 )
        {
        // not supported
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPMAXAUTHS" ) );
        }// else if 
    else if( ( aParameter.Name().Compare( EAPENCAPS ) ) == 0 )
        {
        
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPENCAPS" ) );
        
        TEapExpandedType dummy; 
        ConvertEAPStringToIds( value, eap->iEncapsulatingExpandedEapId, dummy );
                     
        }// else if    
    else if( ( aParameter.Name().Compare( EAPVERSERREALM ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPVERSERREALM" ) );
        
        TInt serverRealm;
        TLex lex( value );
        lex.Val( serverRealm );
        
        eap->iEAPSettings->iVerifyServerRealmPresent = ETrue;
        
        if ( serverRealm == 1 )
            {        
            eap->iEAPSettings->iVerifyServerRealm = ETrue;
            }
        else
            {
            eap->iEAPSettings->iVerifyServerRealm = EFalse;            
            }
        }// else if    
    else if( ( aParameter.Name().Compare( EAPCLIENTHAUTH ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPCLIENTHAUTH" ) );
        
        TInt clientAuthentication;
        TLex lex( value );
        lex.Val( clientAuthentication );
        
        eap->iEAPSettings->iRequireClientAuthenticationPresent = ETrue;
        
        if ( clientAuthentication == 1 )
            {        
            eap->iEAPSettings->iRequireClientAuthentication = ETrue;
            }
        else
            {
            eap->iEAPSettings->iRequireClientAuthentication = EFalse;            
            }
        }// else if    
    else if( ( aParameter.Name().Compare( EAPSESVALTIME ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPSESVALTIME" ) );
        
        TInt sessionValidityTime;
        TLex lex( value );
        lex.Val( sessionValidityTime );
        eap->iEAPSettings->iSessionValidityTime = sessionValidityTime;
        eap->iEAPSettings->iSessionValidityTimePresent = ETrue;

        }// else if    
    else if( ( aParameter.Name().Compare( EAPCIPSUIT ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPCIPSUIT" ) );
        
        TInt cipherSuites;
        TLex lex( value );
        lex.Val( cipherSuites );
        eap->iEAPSettings->iCipherSuites.Append( cipherSuites );
        eap->iEAPSettings->iCipherSuitesPresent = ETrue;
        
        }// else if
    else if( ( aParameter.Name().Compare( EAPPEAPV0 ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPPEAPV0" ) );
        
        TInt peap;
        TLex lex( value );
        lex.Val( peap );
        
        eap->iEAPSettings->iPEAPVersionsPresent = ETrue;
        if ( peap == 1 )
            {        
            eap->iEAPSettings->iPEAPv0Allowed = ETrue;
            }
        else
            {
            eap->iEAPSettings->iPEAPv0Allowed = EFalse;            
            }
        }// else if     
    else if( ( aParameter.Name().Compare( EAPPEAPV1 ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPPEAPV1" ) );
        TInt peap;
        TLex lex( value );
        lex.Val( peap );
        eap->iEAPSettings->iPEAPVersionsPresent = ETrue;
        if ( peap == 1 )
            {        
            eap->iEAPSettings->iPEAPv1Allowed = ETrue;
            }
        else
            {
            eap->iEAPSettings->iPEAPv1Allowed = EFalse;            
            }
        }// else if
    else if( ( aParameter.Name().Compare( EAPPEAPV2 ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPPEAPV2" ) );
        TInt peap;
        TLex lex( value );
        lex.Val( peap );
        eap->iEAPSettings->iPEAPVersionsPresent = ETrue;
        if ( peap == 1 )
            {        
            eap->iEAPSettings->iPEAPv2Allowed = ETrue;
            }
        else
            {
            eap->iEAPSettings->iPEAPv2Allowed = EFalse;            
            }
        }// else if  
        
    else if( ( aParameter.Name().Compare( EAPISSNAME ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPISSNAME" ) );
        
        eap->iCertificate->SetSubjectName(value);
        eap->iCertificate->SetSubjectNamePresent();
        }// else if        
        
    else if( ( aParameter.Name().Compare( EAPSUBNAME ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPSUBNAME" ) );
        
        eap->iCertificate->SetIssuerName(value);
        eap->iCertificate->SetIssuerNamePresent();       
        }// else if        
        
    else if( ( aParameter.Name().Compare( EAPCERTTYPE ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPCERTTYPE" ) );
        if ( value == EAPCERTCA )
			{
			FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EEapSim" ) );
            
            eap->iCertificate->SetCertType(EapCertificateEntry::ECA);
            eap->iEAPSettings-> iUseAutomaticCACertificatePresent = ETrue;
            eap->iEAPSettings-> iUseAutomaticCACertificate = EFalse;
			}
		else
		    {
		    eap->iCertificate->SetCertType(EapCertificateEntry::EUser);
		    }
		// Certificates must be present since this field was added
		eap->iEAPSettings->iCertificatesPresent = ETrue;
		eap->iCertificate->SetIsEnabledPresent();
		eap->iCertificate->SetIsEnabled(ETrue) ;
        
        }// else if        
    else if( ( aParameter.Name().Compare( EAPSERNUM ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPSERNUM" ) );
        
        eap->iCertificate->SetSerialNumber(value);
        eap->iCertificate->SetSerialNumberPresent();
        }// else if
        
    else if( ( aParameter.Name().Compare( EAPSUBKEYID ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPSUBKEYID" ) );
           
        _LIT(KHexIdLC, "0x");
       	_LIT(KHexIdUC, "0X");
       	TBuf<2> HexIdBuf;
       	TInt keyLen = aParameter.Value().Length();
       	
       	// setting the given key to the key buffer
       	TBuf<KMaxSubKeyLenght> origKey;
       	origKey.SetLength(keyLen);
       	origKey = aParameter.Value();
       	TBuf<KMaxSubKeyLenght> key;
       	TLex tmpByte;
       	TInt err(KErrNone);
       	TUint16 byte;
       	
       	// Remove possible spaces from the beginning
       	origKey.TrimLeft();
       	
       	// if the key is over two chars long, then we have to check
       	// whether there is 0x or 0X in the beginning
       	if (origKey.Length() >= 2)
            {
        	// Copy the two left most characters in to the buffer
        	HexIdBuf.Copy(origKey.Left(2));
        	
        	// If the first characters are 0x or 0X, then they should be ignored
        	if (HexIdBuf.Compare(KHexIdLC) == 0
        			|| HexIdBuf.Compare(KHexIdUC) == 0)
        		{
        		// delete two characters
        		origKey.Delete(0, 2);
        		}
            }
        
       	// looping the subject key through, removing whitespaces
       	for (TInt i = 0; i < keyLen; i++)
       		{
       		// removing white spaces from the left side of the key
       		origKey.TrimLeft();
       		// check that there are characters left
       		if (origKey.Length() >= 2)
       			{
       			// pick the two left most bytes from the key
       			tmpByte = origKey.Left(2);
       			// convert byte into binary format
       			err = tmpByte.Val(byte, EHex);
       			
       			// delete two characters from the left side of the character array in the buffer
       			origKey.Delete(0, 2);
       			
       			// check whether conversion to decimal went ok
       			if (err != KErrNone)
       				{
       				// if there are problems, then leave the loop
       				break;
       				}
       			
       			// store the appended byte into the key variable
       			key.Append(byte);
       			}
       			
       		else if (origKey.Length() == 1)
       			{
       			// pick the left most bytes from the key
       			tmpByte = origKey.Left(1);
       			// convert byte into binary format
       			err = tmpByte.Val(byte, EHex);
       			
       			// delete two characters from the left side of the character array in the buffer
       			origKey.Delete(0, 1);
       			
       			// check whether conversion to decimal went ok
       			if (err != KErrNone)
       				{
       				// if there are problems, then leave the loop
       				break;
       				}
       			
       			// store the appended byte into the key variable
       			key.Append(byte);
       			}
       		else
       			{
       			break;
       			}
       		}
       	TBuf8<KMaxSubKeyLenght> keyIdentifier;
       	keyIdentifier.Copy(key);
       	// store key value only if no errors occurred
       	if (err == KErrNone) 
       		{
       		eap->iCertificate->SetSubjectKeyId(keyIdentifier);
       		eap->iCertificate->SetSubjectKeyIdPresent();
       		eap->iEAPSettings->iCertificatesPresent = ETrue; 
       		}
       	}
       	
        
    else if( ( aParameter.Name().Compare( EAPTHUMBPRINT ) ) == 0 )
        {
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::HandleEAPParametersL EAPTHUMBPRINT" ) );
        eap->iCertificate->SetThumbprintPresent();
        eap->iCertificate->SetThumbprint(value);
        }// else if                  
    }


// -----------------------------------------------------------------------------
// CWPAPNapdef::CWPAPNapdef
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPAPNapdef::CWPAPNapdef( TBool aFollowingLink,
                          const TDesC& aDefaultName, 
                          CWPCharacteristic& aLogicalCharacteristic )
                        : CWPAPItemBase( aDefaultName ),
                          iCharacteristic( aLogicalCharacteristic ),
                          iFollowingLink( aFollowingLink )
	{
	}

// -----------------------------------------------------------------------------
// CWPAPNapdef::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPAPNapdef::ConstructL()
	{
	iLingerValue = NULL;
	iLingerFlag = ETrue;
	iInternetIndicator = -1;
	//checks if Wlan feature is supported
  FeatureManager::InitializeLibL();
  iWlanSupported = FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );
  FeatureManager::UnInitializeLib();
	}

// -----------------------------------------------------------------------------
// CWPAPNapdef::SaveWlanDataL
// -----------------------------------------------------------------------------
//
void CWPAPNapdef::SaveWlanDataL( TUint32 aIapId, CCommsDatabase& aCommsDb )
    {
    FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL APid: (%d)"),  aIapId) );
    
    if ( iBearer == KUidWlanBearerType )
	    {
        CCommsDbTableView* wLanServiceTable;
        
        // CMManager
        // Search the Iap Service Id using the received IapId
        RCmConnectionMethodExt cm = iCm->ConnectionMethodL( aIapId );
        TUint32 serviceId = cm.GetIntAttributeL( CMManager::ECmIapServiceId );
     	
        // The BeginTransaction might fail if someone else has locked CommsDat
        TInt dberr = aCommsDb.BeginTransaction();
        if( dberr == KErrLocked )
            {
            // Try BeginTransaction again to get lock
            TInt retry = KBeginTransRetryCount;            
            while ( retry > 0 && dberr == KErrLocked )
                {                   
                User::After(KBeginTransRetryDelay);
                dberr = aCommsDb.BeginTransaction();
                FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL: Database locked. BeginTransaction retry." ) );
                retry--;
                }
            if(dberr != KErrNone)
                {                            
                // Could not get lock to CommsDat at all
                FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL: BeginTransaction failed completely." ) );
                User::Leave( dberr ); 
                }           
            }                             
        
        TInt indexToSec = 0;
        wLanServiceTable = aCommsDb.OpenViewMatchingUintLC( 
                        TPtrC( WLAN_SERVICE ), TPtrC( WLAN_SERVICE_ID ), serviceId );
        TInt errorCode = wLanServiceTable->GotoFirstRecord();

        if ( errorCode == KErrNone )
            {
            indexToSec = serviceId;
            FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL updaterecord" ) );
            wLanServiceTable->UpdateRecord();
            }
        else
            {
            FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL insertrecord" ) );
            TUint32 dummyUid( 0 );
            User::LeaveIfError( wLanServiceTable->InsertRecord( dummyUid ) );

            FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL WriteUintL WLAN_SERVICE_ID" ) );
            // Save link to LAN service
            wLanServiceTable->WriteUintL( TPtrC( WLAN_SERVICE_ID ), aIapId );
            indexToSec = aIapId;
            }
        
        // Setting the security mode manually, because CMManager does not do that
        wLanServiceTable->WriteUintL( TPtrC( WLAN_SECURITY_MODE ), iSecMode );

    	if( iSecSSID.Count() )
			{
	        CCommsDbTableView* wLanSecserviceTable;
			wLanSecserviceTable = aCommsDb.OpenTableLC( TPtrC( WLAN_SECONDARY_SSID ) ); 
			//CleanupStack::Pop(wLanSecserviceTable); // wLanSecserviceTable
		 	TBuf<4> blank;
		 	blank.Append(KNullDesC);
		    
		    TBool sssid = EFalse;
	        TBool sussid = EFalse;

	        for(TInt i = 0; i < iSecSSID.Count(); i++ )
	        	{
	        	TUint32 id ;
	        	
		        
		        TInt retval = wLanSecserviceTable->InsertRecord(id);
		        wLanSecserviceTable->WriteUintL(TPtrC(WLAN_SEC_SSID_SERVICE_ID), indexToSec );
		        wLanSecserviceTable->WriteUintL(TPtrC(WLAN_SEC_SSID_ID), id );

		 	    if( iSecSSID[i]->iSSSID != NULL)
		 	    sssid = ETrue;
		 	    if(iSecSSID[i]->iSUSSID != NULL)
		 	    sussid = ETrue;
		 	    
		 	    if((sssid && iSecSSID[i]->iSSSID->Value() != blank) || (sussid && iSecSSID[i]->iSUSSID->Value() != blank ))
		 	    {
		 		if( sssid )
		 		wLanSecserviceTable->WriteTextL(TPtrC(WLAN_SEC_SSID_SCANNED_SSID), iSecSSID[i]->iSSSID->Value());
			
				if(sussid)
				wLanSecserviceTable->WriteTextL(TPtrC(WLAN_SEC_SSID_USED_SSID), iSecSSID[i]->iSUSSID->Value());
		        
		 	    }
		 	    wLanSecserviceTable->PutRecordChanges();
		 	    sssid = EFalse;
	            sussid = EFalse;
	        	}
	        	CleanupStack::PopAndDestroy( wLanSecserviceTable );
			}
			
			iSecSSID.ResetAndDestroy();
			iSecSSID.Close();
        if ( iPriSSID )
            {
            FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL WriteTextL NU_WLAN_SSID" ) );
            // Save NU_WLAN_SSID
        	wLanServiceTable->WriteTextL( TPtrC(NU_WLAN_SSID), iPriSSID->Value() );
            }
        if ( iPriUSSID )
            {        
            FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL WriteTextL WLAN_USED_SSID" ) );
            // Save WLAN_USED_SSID	
    	      wLanServiceTable->WriteTextL( TPtrC(WLAN_USED_SSID), iPriUSSID->Value() );
            }
        if ( iPriHSSID )
            {        
    	    FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL WriteTextL WLAN_SCAN_SSID" ) );
            // Save WLAN_SCAN_SSID
    	    wLanServiceTable->WriteBoolL( TPtrC(WLAN_SCAN_SSID), ETrue );
            }
        else
            {
    	    wLanServiceTable->WriteBoolL( TPtrC(WLAN_SCAN_SSID), EFalse );            	
            }

       	FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL WriteUintL WLAN_WEP_INDEX" ) );
        // Save index of key in use
        wLanServiceTable->WriteUintL( TPtrC( WLAN_WEP_INDEX ), 
                                      (TUint32&) iDefaultWepKeyIndex );
        FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL WriteUintL WLAN_WEP_INDEX: (%d)"),  iDefaultWepKeyIndex) );

        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL WriteUintL NU_WLAN_AUTHENTICATION_MODE" ) );
        // Open / Shared (0/1)
        // Save index of key in use
        wLanServiceTable->WriteUintL( TPtrC( NU_WLAN_AUTHENTICATION_MODE ), 
                                     ( TUint32& ) iAuthentication );
        FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL WriteUintL NU_WLAN_AUTHENTICATION_MODE: (%d)"),  iAuthentication) );
        
        
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL WriteTextL NU_WLAN_WEP_KEY1" ) );
        // Save first WEP key
        wLanServiceTable->WriteTextL( TPtrC( NU_WLAN_WEP_KEY1 ), iKeyData[0] );
        wLanServiceTable->WriteUintL( TPtrC( WLAN_WEP_KEY1_FORMAT ), 
                                 ( TUint32& ) iKeyFormat[0] );
        FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL WriteTextL (%S)"), &iKeyData[0] ) );
        
        
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL WriteTextL NU_WLAN_WEP_KEY2" ) );
        // Save second WEP key
        wLanServiceTable->WriteTextL( TPtrC( NU_WLAN_WEP_KEY2 ), iKeyData[1] );
        wLanServiceTable->WriteUintL( TPtrC( WLAN_WEP_KEY2_FORMAT ), 
                                 ( TUint32& ) iKeyFormat[1] );
        FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL WriteTextL (%S)"), &iKeyData[1] ) );
            
        
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL WriteTextL NU_WLAN_WEP_KEY3" ) );
        // Save third WEP key
        wLanServiceTable->WriteTextL( TPtrC( NU_WLAN_WEP_KEY3 ), iKeyData[2] );
        wLanServiceTable->WriteUintL( TPtrC( WLAN_WEP_KEY3_FORMAT ), 
                                 ( TUint32& ) iKeyFormat[2] );
        FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL WriteTextL (%S)"), &iKeyData[2] ) );
            
            
        
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL WriteTextL NU_WLAN_WEP_KEY4" ) );
        // Save fourth WEP key
        wLanServiceTable->WriteTextL( TPtrC( NU_WLAN_WEP_KEY4 ), iKeyData[3] );
        wLanServiceTable->WriteUintL( TPtrC( WLAN_WEP_KEY4_FORMAT ), 
                                 ( TUint32& ) iKeyFormat[3] );
        FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL WriteTextL (%S)"), &iKeyData[3] ) );
            
        
        if ( iWpaPresKeyAsc )
            {     
            FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL convert WPAKEY to ascii" ) );
            // convert unicode to 8-bit ascii
            TBuf8<KMaxWpaPskLength> wpaKey;
            StrCopy( wpaKey, iWpaPresKeyAsc->Value() );
            
            // Enable WPA Pre Shared key mode
            wLanServiceTable->WriteBoolL( TPtrC( WLAN_ENABLE_WPA_PSK ), ETrue );
            
            FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL WriteUintL WLAN_WPA_PRE_SHARED_KEY" ) );
            // Save PreShared Key
            wLanServiceTable->WriteTextL( TPtrC( WLAN_WPA_PRE_SHARED_KEY ), 
                                         wpaKey );
            FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL WriteUintL WLAN_WPA_KEY_LENGTH" ) ); 
            // Save PreShared Key Length
            wLanServiceTable->WriteUintL( TPtrC( WLAN_WPA_KEY_LENGTH ), 
                                         wpaKey.Length() );                                         
            }

        TBuf<KMaxLengthOfEapList> eapList;
        eapList.Copy(KEapAll);

        for (TInt i=0;i<iEapTypeArray.Count();i++)
            {
            FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL save EAP settings" ) );
            CEapTypeElement* eap = iEapTypeArray[i];
            
            if ( eap->iEAPSettings->iCertificatesPresent )
            	{
            	FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL Certificate was present." ) );
            	eap->iEAPSettings->iCertificates.AppendL( eap->iCertificate );	
            	}                  

            // Save EAP list
            if ( eap->iEapTypeString.Length() > 0  
            	 && eap->iEncapsulatingExpandedEapId.Length() == 0) // AND method is not encapsulated
                {
                FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL WriteTextL WLAN_EAPS" ) );
                
                TBuf8<3> buf;
                _LIT8(KFormat, "%d");
                // pick the last byte from the array
                buf.Format(KFormat, eap->iEapTypeString[KExpandedEapIdLength - 1]);
                // search for the correct offset for the eap type from the KEapAll literal
                TInt index = KEapAll().Find(buf);
                
                const TUint8 offset = 2; //length = f "-0" string
                // Add '+' to the correct offset, so that the eap is repsesented activated in the list
                if(index >= offset)
                    {
                    eapList[index-offset] = KEapPlus; 
                    }
                
                }
            // Set iEncapsulatingEapTypes
                 for ( TInt j = 0; j < iEapTypeArray.Count(); j++ )
                     {
                     // Check if any method has this method as the encapsulating method
                     if ( eap->iEapTypeString == iEapTypeArray[j]->iEncapsulatingExpandedEapId )
                         {
                         // Append this method to iEncapsulatedEAPType
                         eap->iEAPSettings->iEnabledEncapsulatedEAPExpandedTypes.Append( iEapTypeArray[j]->iEAPSettings->iEAPExpandedType );
                         eap->iEAPSettings->iEnabledEncapsulatedEAPExpandedTypesPresent = ETrue;
                         }
                     }
                 
            FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL save EAP settings CEapType::NewL" ) );        
            CEapType* eapType = NULL;

            
            TRAPD( error, ( eapType = CEapType::NewL( ELan, serviceId, eap->iEAPSettings->iEAPExpandedType ) ) );

            if ( ( error == KErrNone ) && eapType )
                {
                CleanupStack::PushL( eapType );
                
                // Check if this type is tunneled
                if ( eap->iEncapsulatingExpandedEapId.Length() > 0 )
                	{
                	// It is tunneled. 
                	eapType->SetTunnelingType( eap->iEncapsulatingExpandedEapId);
                	
                	}
                
                FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL save EAP settings SetConfigurationL" ) );
                TRAP_IGNORE( eapType->SetConfigurationL( *eap->iEAPSettings ) );
                
                CleanupStack::PopAndDestroy( eapType );
                
                }
            else
                {
                FTRACE(RDebug::Print(_L("[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL CEapType::NewL err: %d"), error ) );
                }


            FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL save EAP settings done" ) );
            }
            
            
	
	  		// Expanded EAP types in use. Write the eap list to the new columns.
    		
    		// generate appropriate entries in the new enabled and disabled list,
    		// overwriting those values 
    		
    		// count the + and - signs to determine the size of enabled and 
    		// disabled descriptors

    		TLex lex( eapList );
    		TInt numPlus = 0;
    		TInt numMinus = 0;
    		TChar ch;
    		while ( !lex.Eos() )
    		    {
    		    ch = lex.Get();
    		    if ( ch == '+' ) ++numPlus;
    		    else if ( ch == '-' ) ++numMinus;
    		    }
    		// load general EAP settings If
    		CEapGeneralSettings* genSettings;
    		genSettings = CEapGeneralSettings::NewL(ELan, serviceId);
    		CleanupStack::PushL( genSettings );

    		// get lists of enabled/disabled EAPs for the IAP
    		
    		RArray<TEapExpandedType> enabledEapMethods;
    		RArray<TEapExpandedType> disabledEapMethods;

    		enabledEapMethods.Reset();
    		disabledEapMethods.Reset();
   		
    		
    		// each entry consumes 8 bytes in binary format
    		HBufC8* enabledEAPPlugin = HBufC8::NewL( 8 * numPlus );
    		CleanupStack::PushL( enabledEAPPlugin );
    		
    		HBufC8* disabledEAPPlugin = HBufC8::NewL( 8 * numMinus );
    		CleanupStack::PushL( disabledEAPPlugin );
    		    
    		lex.Assign( eapList );
    		
    		while ( !lex.Eos() )
    		    {
    		    // beginning of implementation UID
    		    TInt16 implUid = 0;
    		    
    		    if ( lex.Val( implUid ) != KErrNone || !implUid )
    		        {
    		        // if the old string is corrupted, null out both lists
    		        enabledEAPPlugin->Des().Zero();
    		        disabledEAPPlugin->Des().Zero();
    		        break;
    		        }
    		
    		    // append it to the appropriate list ('+' enabled, '-' disabled)
    		    _LIT8( KPadding, "\xFE\0\0\0\0\0\0" );
    		    _LIT8( KMsChapV2Padding, "\xFE\xFF\xFF\xFF\0\0\0");
    		    const TInt KPlainMsChapV2ImplUid = 99;
    		    
    		    if ( implUid > 0 )
    		        {
    		        enabledEAPPlugin->Des().Append( 
    		                            Abs( implUid ) == KPlainMsChapV2ImplUid? 
    		                                        KMsChapV2Padding: KPadding );
    		        enabledEAPPlugin->Des().Append( Abs( implUid ) );
    		        TPtrC8 data(reinterpret_cast<TUint8*>(&implUid), KExpandedEapIdLength);
    		        TEapExpandedType expandedType;
					      expandedType.SetValue(enabledEAPPlugin,KEapExpandedTypeLength);
					      enabledEapMethods.Append(expandedType);
					      }
	    
    		    // swallow the delimiter (',')
    		    lex.Get();
    		    }
    
    		genSettings->SetEapMethods(enabledEapMethods, disabledEapMethods);  
    		            
                	
	
    		CleanupStack::PopAndDestroy( disabledEAPPlugin );
    		CleanupStack::PopAndDestroy( enabledEAPPlugin );
    		CleanupStack::PopAndDestroy( genSettings );
    		
    
     
            
       iEapTypeArray.ResetAndDestroy();
       wLanServiceTable->PutRecordChanges();
  

        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL PutRecordChanges" ) ); 
        CleanupStack::PopAndDestroy( wLanServiceTable );
        
        FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL done" ) );
	    aCommsDb.CommitTransaction();
	    }
	else
	    {
	    FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::SaveWlanDataL ignored not a WLAN AP" ) );
	    }
	    
    }

//------------------------------------------------------------------------------
// CWPAPNapdef::ConvertAsciiToHex
//------------------------------------------------------------------------------
//
void CWPAPNapdef::ConvertAsciiToHex( const TDesC8& aSource, HBufC8*& aDest )
	{
	FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::ConvertAsciiToHex" ) );
	
	_LIT( hex, "0123456789ABCDEF" );
	TInt size = aSource.Size();
	TPtr8 ptr = aDest->Des();
	for ( TInt ii = 0; ii < size; ii++ )
		{
		TText8 ch = aSource[ii];
		ptr.Append( hex()[(ch/16)&0x0f] );
		ptr.Append( hex()[ch&0x0f] );
		}
    FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::ConvertAsciiToHex Done" ) );
	}

// -----------------------------------------------------------------------------
// CWPAPNapdef::StrCopy
// String copy with lenght check.
// -----------------------------------------------------------------------------
//
void CWPAPNapdef::StrCopy( TDes8& aTarget, const TDesC& aSource )
    {
    FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::StrCopy" ) );
    
	TInt len = aTarget.MaxLength();
    if( len < aSource.Length() ) 
	    {
		aTarget.Copy( aSource.Left( len ) );
		return;
	    }
	aTarget.Copy( aSource );
	
	FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::StrCopy Done" ) );
    }

CEapTypeElement::~CEapTypeElement()
    {
    delete iEAPSettings;
    delete iCertificate;
    }

// -----------------------------------------------------------------------------
// CWPAPNapdef::UpdateLingerL
// 
// -----------------------------------------------------------------------------
//
void CWPAPNapdef::UpdateLingerL(const TUint32 aIapId)
{
		if ( iLinger && ( !iLingerFlag ) )
		{
    	AddLingerL( aIapId , iLingerValue );
		}
		
}

// -----------------------------------------------------------------------------
// CWPAPNapdef::UdateligerValueL
// 
// -----------------------------------------------------------------------------
//
void CWPAPNapdef::UdateligerValueL(CWPParameter* aLinger)
{
		TInt err(KErrNone);
		const TDesC& value = aLinger->Value();
		if( !Notalpha(value))
		{
		TLex lex( value );
		TUint Linger = 0;
		TInt lingerint;
		err = lex.Val( Linger, EDecimal );
		if( err == KErrNone )
		{
		    lingerint = Linger;
			if( lingerint >= 0 )
		    {
		    	iLingerValue = lingerint;
		    	iLingerFlag = EFalse;
		    }
		}
		}
}

// -----------------------------------------------------------------------------
// CWPAPNapdef::UdateligerValueL
// 
// -----------------------------------------------------------------------------
//
TBool CWPAPNapdef::Notalpha(const TDesC& aValue)
{
	TBool NotAlpha = EFalse;
	TChar character;
	for(TInt i = 0 ; i < aValue.Length(); i++)
	{
		character = aValue[i];
		NotAlpha = character.IsAlpha();
		if(NotAlpha)
		break;
	}
	
	return NotAlpha;
}


// ---------------------------------------------------------------------------
// Adds iapID and linger interval to CentralRepository
// ---------------------------------------------------------------------------
//
void CWPAPNapdef::AddLingerL( const TInt aIapId, const TInt aLingerInterval )
    {
    TInt    err( KErrNone );
    
    // Open repository
    CRepository* repository = CRepository::NewLC( KCRUidPDPContextManager );
    
    // Find if an entry for "iapId" already exists in CentRep 
    RArray< TUint32 > foundKeys;
    foundKeys.Reset();
    
    err = repository->FindEqL( KIapColumn,       // partial key
                               KColumnMask,      // key mask
                               aIapId,           // value
                               foundKeys );      // found keys
    
    if ( err == KErrNone || err == KErrNotFound )
        {
        if ( foundKeys.Count() == 0 )
            {
            // Add a new entry
            TInt arrayCount( 0 );
            
            // Get number of iapId&linger entries in Centrep
            err = repository->Get( KPdpContextManagerLingerArrayCount, 
                                   arrayCount );
        
            if ( err == KErrNone )
                {
                arrayCount++;
                
                err = repository->Create( ( KIapColumn | arrayCount ), aIapId ); 
                
                if ( err == KErrNone )
                    {
                    err = repository->Create( ( KLingerColumn | arrayCount ), 
                                               aLingerInterval );     
                    }
                
                if ( err == KErrNone )
                    {
                    // Update number of iapId&linger entries in Centrep
                    err = repository->Set( KPdpContextManagerLingerArrayCount, 
                                           arrayCount );
                    }
                }
            }
        else
            {
            // Update existing entry on current row
            TInt32 row = foundKeys[ 0 ] & KRowMask;
            
            err = repository->Set( ( KLingerColumn | row ), aLingerInterval ); 
            }         
        }
        
    foundKeys.Close();
    CleanupStack::PopAndDestroy( repository );
    
    User::LeaveIfError( err );
    
    }

void CWPAPNapdef::ConvertEAPStringToIds( const TDesC& aEAPString, TDes8& aExpandedId, TEapExpandedType& aID)
	{
    if ( aEAPString == KEAPSIM )
		{	
		FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::ConvertEAPStringToIds EEapSim" ) );
		aExpandedId.Copy( KEapSimTypeId, KExpandedEapIdLength );
		aID = *EapExpandedTypeSim.GetType();
		}
    else if( aEAPString == KEAPAKA )
        {
  		FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::ConvertEAPStringToIds EEapAka" ) );
		aExpandedId.Copy( KEapAkaTypeId, KExpandedEapIdLength );	
		aID = *EapExpandedTypeAka.GetType();	
        }
    else if( aEAPString == KEAPTLS )
        {
		FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::ConvertEAPStringToIds EEapTls" ) );
		aExpandedId.Copy( KEapTlsTypeId, KExpandedEapIdLength );	
		aID = *EapExpandedTypeTls.GetType();	
        }
    else if( aEAPString == KEAPPEAP )
        {
		FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::ConvertEAPStringToIds EEapPeap" ) );
		aExpandedId.Copy( KEapPeapTypeId, KExpandedEapIdLength );	
		aID = *EapExpandedTypePeap.GetType();	
        }
    else if( aEAPString == KEAPTTLS )
        {
		FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::ConvertEAPStringToIds EEapTtls" ) );
		aExpandedId.Copy( KEapTtlsTypeId, KExpandedEapIdLength );		
		aID = *EapExpandedTypeTtls.GetType();
        }
   else if( aEAPString == KEAPLEAP )
        {
		FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::ConvertEAPStringToIds EEapLeap" ) );
		aExpandedId.Copy( KEapLeapTypeId, KExpandedEapIdLength );
		aID = *EapExpandedTypeLeap.GetType();		
        }                                    
   else if( aEAPString == KEAPMSCHAPV2 )
        {
		FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::ConvertEAPStringToIds EEapMschapv2" ) );
		aExpandedId.Copy( KEapMschapv2TypeId, KExpandedEapIdLength );		
		aID = *EapExpandedTypeMsChapv2.GetType();
        }     
   else if( aEAPString == KEAPGTC )
        {
		FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::ConvertEAPStringToIds EEapGtc" ) );
		aExpandedId.Copy( KEapGtcTypeId, KExpandedEapIdLength );
		aID = *EapExpandedTypeGtc.GetType();
        }
   else if( aEAPString == KEAPFAST )
        {
		FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::ConvertEAPStringToIds EEapFast" ) );
		aExpandedId.Copy( KEapFastTypeId, KExpandedEapIdLength );
		aID = *EapExpandedTypeFast.GetType();
        }

    else
        {
		FLOG( _L( "[AccesspointAdapter] CWPAPNapdef::ConvertEAPStringToIds EEapNone" ) );
		aExpandedId.Copy( KEapNoneId, KExpandedEapIdLength );
		aID = *EapExpandedTypeNone.GetType();
        }

	}
 
// ---------------------------------------------------------------------------
// Returns the NAP-ID
// ---------------------------------------------------------------------------
//
const TDesC& CWPAPNapdef::NapId()
    {
	return iNapID->Value();	
    }

// ---------------------------------------------------------------------------
// Returns the bearer value
// ---------------------------------------------------------------------------
//    
TUint CWPAPNapdef::Bearer()
	{
	return iBearer;
	}
    
// ---------------------------------------------------------------------------
// Returns the bearer value
// ---------------------------------------------------------------------------
//    
void CWPAPNapdef::SetCMManager( RCmManagerExt* aCm )
	{
	iCm = aCm;
	}
    
    
    
//  End of File
