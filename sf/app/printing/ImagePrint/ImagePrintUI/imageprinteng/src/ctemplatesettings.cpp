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


#include "ctemplatesettings.h"

#include "ctemplatedata.h"
#include "tprinter.h"
#include "mdiscoveryobserver.h"
#include "crealfactory.h"
#include "mprintsettings.h"
#include "clog.h"

// BT Template ID's
const TInt KOneUpBorderless( 1554 );	
const TInt KOneUpBorder( 1555 );	
const TInt KOneUpStick( 1565 );	
const TInt KFourUpStick( 1564 );	
const TInt KSixteenUpStick( 1516 );		
const TInt KOneUp( 1501 );			


//  CONSTRUCTION
CTemplateSettings* CTemplateSettings::NewL( CRealFactory* aFactory )
    {    
    CTemplateSettings* self = CTemplateSettings::NewLC( aFactory );
    CleanupStack::Pop();   // self 
    return self;
    }

CTemplateSettings* CTemplateSettings::NewLC( CRealFactory* aFactory )
    {
    CTemplateSettings* self = new ( ELeave ) CTemplateSettings( aFactory );
    CleanupStack::PushL( self );
    return self;
    }

// Default constructor
CTemplateSettings::CTemplateSettings( CRealFactory* aFactory )
    {
    iActiveTemplateUid = KMaxTUint32;
    iFactory = aFactory;
    }

// Destructor
CTemplateSettings::~CTemplateSettings()
    {
    iTemplates.ResetAndDestroy();
    }

// Clones current template settings
CTemplateSettings* CTemplateSettings::CloneL()
    {
    TBool dropTemplates = EFalse;
    CTemplateSettings* self = CTemplateSettings::NewLC( iFactory );

    self->iActiveTemplateUid = iActiveTemplateUid;
    TInt num( iTemplates.Count() );
    TInt templateID;

    if( (iFactory->SettingsIF()->GetCurrentPrinterProtocol() == MDiscoveryObserver::EWLAN)  ||
        (iFactory->SettingsIF()->GetCurrentPrinterProtocol() == MDiscoveryObserver::EBPP)   ||
        (iFactory->SettingsIF()->GetCurrentPrinterProtocol() == MDiscoveryObserver::EMMC)   ||
        (iFactory->SettingsIF()->GetCurrentPrinterProtocol() == MDiscoveryObserver::EUSB) )
        {
        LOG("[CTemplateSettings] Templates dropped");
        dropTemplates = ETrue;
        }
    
    LOG1("CTemplateSettings: Number of templates received from engine: %d",num);
    
    for ( TInt i( 0 ); i < num; i++ )
        {
        templateID = iTemplates[i]->iUid;
      
        
        if( (dropTemplates &&  (templateID == KOneUpBorderless || templateID == KOneUpBorder || templateID == KOneUpStick || templateID == KFourUpStick || templateID == KSixteenUpStick || templateID == KOneUp )) || !dropTemplates)
            {
            LOG1("CTemplateSettings: Template ID: %d ADDED", templateID);    
            User::LeaveIfError( self->iTemplates.Append( iTemplates[i]->CloneL() ) );       
            }
        else
            {
            LOG1("CTemplateSettings: Template ID: %d SKIPPED",templateID);    
            }        
        }

    CleanupStack::Pop();	// self
    return self;
    }

// Sets the new active template
void CTemplateSettings::SetActiveTemplateL( 
    TInt aUid )
    {
    LOG1( "CTemplateSettings:SetActiveTemplateL || aUid = %d", aUid );
    TInt err( KErrNotFound );

    TInt num( iTemplates.Count() );
    for ( TInt i( 0 ); i < num && err != KErrNone; i++ )
        {        
        if ( aUid == iTemplates[i]->iUid )
            {
            iActiveTemplateUid = aUid;
            err = KErrNone;            
            }
        }
	LOG2( "CTemplateSettings:SetActiveTemplateL uid: %d, err: %d", aUid, err );
    // Leave when not found
    User::LeaveIfError( err );
    }

//  End of File
