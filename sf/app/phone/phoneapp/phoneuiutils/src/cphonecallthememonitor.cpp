/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  See class description.
*
*/


// INCLUDE FILES
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <pslninternalcrkeys.h>

#include "phoneconstants.h"
#include "cphonecallthememonitor.h"
#include "phonelogger.h"
#include "cphonecenrepproxy.h"
#include "mphonecenrepobserver.h"
#include "phoneui.pan"

// Values for KThemesCallImageSettings CR key
const TInt KPslnCRUserDefinedCallImage = 1;

 
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhoneCallThemeMonitor::CPhoneCallThemeMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhoneCallThemeMonitor::CPhoneCallThemeMonitor()
    {
    }

// -----------------------------------------------------------------------------
// CPhoneCallThemeMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPhoneCallThemeMonitor::ConstructL()
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CPhoneCallThemeMonitor::ConstructL ()" );    

    // Get all current values
    TRAP_IGNORE( ReadAllL() );    
    }

// -----------------------------------------------------------------------------
// CPhoneCallThemeMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhoneCallThemeMonitor* CPhoneCallThemeMonitor::NewL()
    {
    CPhoneCallThemeMonitor* self = 
        new (ELeave) CPhoneCallThemeMonitor();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// Destructor
EXPORT_C CPhoneCallThemeMonitor::~CPhoneCallThemeMonitor()
    {
    delete iCallerImageThemePath;
    }

// -----------------------------------------------------------------------------
// CPhoneCallThemeMonitor::CallImageThemeSettings
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPhoneCallThemeMonitor::CallImageThemeSettings() const
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CPhoneCallThemeMonitor::CallImageThemeSettings ()" );    
    return iThemeCallImageSettings;
    }    

// -----------------------------------------------------------------------------
// CPhoneCallThemeMonitor::CallImageThemeFilePath
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CPhoneCallThemeMonitor::CallImageThemeFilePath() const
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CPhoneCallThemeMonitor::CallImageThemeFilePath ()" );    
    if ( iCallerImageThemePath )    
        {
        return *iCallerImageThemePath;    
        }
    else
        {
        return KNullDesC();    
        }        
    }

// -----------------------------------------------------------------------------
// CPhoneCallThemeMonitor::SetCallThemeSettingsObserver
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneCallThemeMonitor::SetCallThemeSettingsObserver(
    MPhoneCallThemeSettingsObserver* aObserver )
    {
    __ASSERT_DEBUG( iObserver == NULL, Panic( KErrNotSupported ) );
    iObserver = aObserver;
    }

// -----------------------------------------------------------
// CPhoneCallThemeMonitor::HandleCenRepChangeL
// -----------------------------------------------------------
//
void CPhoneCallThemeMonitor::HandleCenRepChangeL( 
    const TUid& aUid,
    const TUint aId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneCallThemeMonitor::HandleCenRepChangeL( ) ");
    if ( aUid == KCRUidThemes ) 
        {
        if (aId == KThemesCallImagePath || aId == KThemesCallImageSetting )    
            {
            ReadAllL();
            if ( iObserver )
                {
                iObserver->CallThemeSettingsChanged();
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CPhoneCallThemeMonitor::ReadAllL
// -----------------------------------------------------------------------------
//
void CPhoneCallThemeMonitor::ReadAllL()
    {
    __LOGMETHODSTARTEND( EPhoneUIUtils, "CPhoneCallThemeMonitor::ReadAllL ()" );

    // Call image status 
    TInt value( KErrNone );
    TInt err( CPhoneCenRepProxy::Instance()->GetInt(
        KCRUidThemes,
        KThemesCallImageSetting,
        value ) );
    
    if ( err == KErrNone )
        {
        iThemeCallImageSettings = value;    
        }
  
    if ( iThemeCallImageSettings == KPslnCRUserDefinedCallImage )
        {
        delete iCallerImageThemePath;
        iCallerImageThemePath = NULL;
        iCallerImageThemePath = HBufC::NewL( KMaxFilePathLength );
        TPtr imagePtr ( iCallerImageThemePath->Des() );
            
        __PHONELOG1( EBasic, EPhoneUIUtils,
            "CPhoneCallThemeMonitor::ReadAllL -> image path = %S", &imagePtr );
        } 
    else
        {
        delete iCallerImageThemePath;
        iCallerImageThemePath = NULL;
        }             
    }

//  End of File  
