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
#include <coemain.h>
CCoeStatic * CCoeEnv::Static(TUid) 
    {    
    return NULL;
    }

CCoeEnv * CCoeEnv::Static() {}

#include <eikenv.h>
void CEikonEnv::DismissTaskList() {}


#include <featmgr.h>
TInt FeatureManager::FeatureSupported( TInt ) {}

#include <w32std.h>
void RWsSession::SimulateKeyEvent(TKeyEvent) {}
void RWsSession::Flush() {}

#include <APGTASK.h>
TApaTaskList::TApaTaskList(RWsSession &aSession ) : iWsSession( aSession ) {}
TApaTask TApaTaskList::FindApp(TUid) {}
TInt TApaTask::Exists() const {return EFalse;}

#include <MProfileEngine.h>
MProfileEngine * CreateProfileEngineL() {}


#include "cphonelogger.h"
CPhoneMethodLogger::CPhoneMethodLogger(
        TPhoneUILoggingComponent /*aLoggingComponent*/,
        TPtrC /*aTring*/ )
    {
    }
CPhoneMethodLogger::~CPhoneMethodLogger()
    {
    }

#include  "CPhonePubSubProxy.h"
CPhonePubSubProxy* CPhonePubSubProxy::Instance()
    {
    return NULL;
    }
void CPhonePubSubProxy::ChangePropertyValue( 
    const TUid& /*aCategory*/,
    const TUint /*aKey*/, 
    const TInt /*aValue*/ )
    {
    }

#include <StringLoader.h>
// Constant
_LIT( KCreateListVoice, "Voice call" );
_LIT( KCreateListVideo, "Video call" );
_LIT( KCreateListInternet, "Internet call" );
_LIT( KHoldText, "Hold" );
_LIT( KTestString,"TestString" );

void StringLoader::Load( TDes& /*aDest*/, TInt /*aResourceId*/,
    CCoeEnv* /*aLoaderEnv*/ )
    {
    
    }

HBufC* StringLoader::LoadLC( TInt aResourceId,
                     CCoeEnv* /*aLoaderEnv*/ )
    {
    /*if ( R_CALLUI_CREATE_LIST_VOICE == aResourceId )
        {
        return KCreateListVoice().AllocLC();
        }
    if ( R_CALLUI_CREATE_LIST_VIDEO == aResourceId )
        {
        return KCreateListVideo().AllocLC();
        }
    if ( R_CALLUI_CREATE_LIST_INTERNET == aResourceId )
        {
        return KCreateListInternet().AllocLC();
        }*/
    return KNullDesC().AllocLC();
    };
    
HBufC* StringLoader::LoadL( TInt aResourceId, TInt /*aInt*/,
                         CCoeEnv* aLoaderEnv )
    {
    HBufC* ret = LoadLC( aResourceId, aLoaderEnv );
    CleanupStack::Pop();
    return ret;
    };

void StringLoader::Format( TDes& aDest, const TDesC& aSource,
                     TInt aPosition, const TDesC& aSubs )
    {
    aDest.Copy( KTestString );
    }

#include "CPhoneMainResourceResolver.h"

CPhoneMainResourceResolver::CPhoneMainResourceResolver()
    {
    
    }
CPhoneMainResourceResolver::~CPhoneMainResourceResolver()
    {
    Dll::SetTls( NULL ) ;
    }

CPhoneMainResourceResolver* CPhoneMainResourceResolver::Instance()
    {
    CPhoneMainResourceResolver* singleton;

    if ( Dll::Tls() == NULL )
          {
          // TLS is still null, which means that no CMySingleton has
          // been instantiated yet.  Do so now, and return that
          // instance:
          singleton = new ( ELeave ) CPhoneMainResourceResolver();
          // Store a pointer to the new instance in thread local storage:
          TInt err = Dll::SetTls( static_cast<TAny*>( singleton ) );
          if ( err == KErrNone )
              {
              return singleton;
              }
          else
              {
              delete singleton;
              User::Leave( err );
              return NULL;
              }
          }
      else
          {
        
          singleton = static_cast<CPhoneMainResourceResolver*>( Dll::Tls() );
          return singleton;
          }

    }

TInt CPhoneMainResourceResolver::ResolveResourceID( const TInt& /*aResource*/ ) const
    {
    return 0;
    }

void CPhoneMainResourceResolver::RegisterResolver( 
    MPhoneResourceResolver* /*aResolver*/ )
    {
    
    }


//#include <AknUtils.h>
class AknTextUtils 
    {
    static ConvertDigitsTo(  TDes& /*aDes*/, TDigitType /*aDigitType*/ )
        {
    
            }
    };

    
#include "cphonemediatorfactory.h"

CPhoneMediatorFactory* CPhoneMediatorFactory::Instance()
    {
    
    }
    
CPhoneMediatorSender* CPhoneMediatorFactory::Sender()
    {

    }

#include "cphonemediatorsender.h"
    
TInt CPhoneMediatorSender::IssueCommand( 
    const TUid /*aDomain*/, 
    const TUid /*aCategory*/, 
    const TInt /*aCommandId*/,
    const TVersion /*aVersion*/, 
    const TDesC8& /*aData*/,
    MPhoneShutdownCommand* /*aShutdownCommand*/ )
    {
    
    }
