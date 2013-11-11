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
* Description:  mmssettings implementation
*
*/


   
// INCLUDE FILES   
#include <centralrepository.h>
#include <msvids.h>
#include <msvapi.h>
#include <MessagingInternalCRKeys.h>

// USERINCLUDE FILES
#include "mmssettings.h"
#include "mmsconst.h"
#include "mmserrors.h"
#include "MmsEnginePrivateCRKeys.h"
#include "mmsdebuglogging.h"
#include "mmsmessagestoreutils.h"
#include "mmsconninit.h"
#include "mmsversion.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
_LIT( KMmsApplicationFolder, "MmsApplication");
// MACROS

// LOCAL CONSTANTS AND MACROS
const TInt KMmsSettingsVersion = 30; // secure version
const TInt KMmsSecondsInAnHour = 3600;
#ifdef __WINS__
const TInt KMmsRowBufferLength = 128;
#endif


// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ============================== LOCAL FUNCTIONS ==============================

// ============================== MEMBER FUNCTIONS =============================

// -----------------------------------------------------------------------------
// CMmsSettings::CMmsSettings
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmsSettings::CMmsSettings()   
    {
#ifdef __WINS__
    iLocalModeInDir = KMmsDefaultLocalModeDir;
    iLocalModeOutDir = KMmsDefaultLocalModeDir;    
#else
    iLocalModeDir = NULL;
#endif
    }

// -----------------------------------------------------------------------------
// CMmsSettings::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmsSettings::ConstructL()
    {
    LOG(_L("CMmsSettings::ConstructL"));
    Reset();
    
    // member iMmsRepository is created only when needed    
#ifdef __WINS__
    ReadLocalModeDirectoriesL();
#endif // __WINS__
    }

// -----------------------------------------------------------------------------
// CMmsSettings::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMmsSettings* CMmsSettings::NewL()
    {
    CMmsSettings* self = new ( ELeave ) CMmsSettings;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CMmsSettings::~CMmsSettings()
    {
    LOG(_L("~CMmsSettings"));
    delete iMmsRepository;
   
#ifndef __WINS__
    // local mode directory is a pointer in hardware only
    delete iLocalModeDir;
#endif    
    iAccesspointArray.Close();
    }

// -----------------------------------------------------------------------------
// Reset
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsSettings::Reset()
    {
    LOG(_L("CMmsSettings::Reset"));
    iSettingsVersion =        KMmsSettingsVersion;
    
    // This is the version number to be used if none is found from
    // central repository, or if the value in central repository is illegal
    iMmsVersion = KMmsDefaultVersion;
    
    iDefaultServiceId =       KMsvNullIndexEntryId;
    iNotificationFolderId =   KMsvNullIndexEntryId;
    iMMBoxFolderId =          KMsvNullIndexEntryId;
    iApplicationFolderId =    KMsvNullIndexEntryId;
    iAccesspointArray.Reset();
    iReceivingModeHome =      EMmsReceivingAutomatic;
    iReceivingModeForeign =   EMmsReceivingAutomatic;
    iAcceptPersonal =         ETrue;
    iAcceptAdvertisement =    ETrue;
    iAcceptInformational =    ETrue;
    iAcceptAnonymous =        ETrue;
    iDeliveryReportWanted =   KMmsNo;
    iDeliveryReportAllowed =  KMmsYes;
    iReadReplyReportWanted =  KMmsNo;
    iReadReplyReportAllowed = ETrue;
    iMessageClass =           EMmsClassPersonal;
    iExpiryInterval =         0;
    iSenderVisibility =       EMmsSenderVisibilityDefault;
    iMessagePriority =        EMmsPriorityNormal;
    iMaxSendSize =            KMmsMaxSendSize;
    iMaxReceiveSize =         KMmsMaxReceiveSize;
    iImageWidth =             0;
    iImageHeight =            0;
    iMoveToSent =             ETrue;
    iLogEmailRecipients =     EFalse;
    iExpiryOvershoot =        KMmsExpiryOvershoot;
    iCreationMode =           EMmsCreationModeWarning;
    iFetchOverride =          EFalse;
    iShowSentTime =           EFalse;
    
#ifdef __WINS__
    iLocalMode =              ETrue;
#else
    iLocalMode =              EFalse;
    delete iLocalModeDir;
    iLocalModeDir = NULL;
#endif // __WINS__
    
    }

// -----------------------------------------------------------------------------
// CopyL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsSettings::CopyL( const CMmsSettings& aMmsSettings )
    {
    LOG(_L("CMmsSettings::Copy"));
#ifdef __WINS__
    iLocalModeInDir  = aMmsSettings.iLocalModeInDir;
    iLocalModeOutDir = aMmsSettings.iLocalModeOutDir;
#else
    iLocalModeDir = NULL;
    if ( aMmsSettings.iLocalModeDir )
        {
        iLocalModeDir = aMmsSettings.iLocalModeDir->AllocL();
        }
#endif
    iSettingsVersion = aMmsSettings.SettingsVersion();
    iMmsVersion = aMmsSettings.MmsVersion();
    iDefaultServiceId = aMmsSettings.Service();
    iNotificationFolderId = aMmsSettings.NotificationFolder();
    iMMBoxFolderId = aMmsSettings.MMBoxFolder();
    iApplicationFolderId = aMmsSettings.ApplicationFolder();      
    iReceivingModeHome = aMmsSettings.ReceivingModeHome(); 
    iReceivingModeForeign = aMmsSettings.ReceivingModeForeign();
    iAcceptPersonal = aMmsSettings.AcceptPersonalMessages();
    iAcceptAdvertisement = aMmsSettings.AcceptAdvertisementMessages();
    iAcceptInformational = aMmsSettings.AcceptInformationalMessages();
    iAcceptAnonymous = aMmsSettings.AcceptAnonymousMessages();
    iDeliveryReportWanted = aMmsSettings.DeliveryReportWanted();
    iDeliveryReportAllowed = aMmsSettings.DeliveryReportSendingAllowed();
    iReadReplyReportWanted = aMmsSettings.ReadReplyReportWanted();
    iReadReplyReportAllowed = aMmsSettings.ReadReplyReportSendingAllowed();
    iMessageClass = aMmsSettings.MessageClass();
    iExpiryInterval = aMmsSettings.ExpiryInterval();
    iSenderVisibility = aMmsSettings.SenderVisibility();
    iMessagePriority = aMmsSettings.MessagePriority();
    iMaxSendSize = aMmsSettings.MaximumSendSize();
    iMaxReceiveSize = aMmsSettings.MaximumReceiveSize();
    iImageWidth = aMmsSettings.ImageWidth();
    iImageHeight = aMmsSettings.ImageHeight();
    iMoveToSent = aMmsSettings.MoveToSent();
    iLogEmailRecipients = aMmsSettings.LogEmailRecipients();
    iExpiryOvershoot = aMmsSettings.ExpiryOvershoot();    
    iCreationMode = aMmsSettings.CreationMode();
    iFetchOverride = aMmsSettings.FetchOverride();
    iLocalMode = aMmsSettings.LocalMode();
    iShowSentTime = aMmsSettings.ShowSentTime();

    LOG(_L("- removing old ap array"));
    iAccesspointArray.Reset();
    
    for( TInt index = 0; index < aMmsSettings.AccessPointCount(); index++ )
        {
        LOG2(_L("- copying index %d"), index );
        User::LeaveIfError( 
            iAccesspointArray.Append( aMmsSettings.AccessPoint( index ) ) );        
        }
    }

// -----------------------------------------------------------------------------
// LoadSettingsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsSettings::LoadSettingsL()
    {
    LOG(_L("CMmsSettings::LoadSettingsL"));

    // Connect CenRep if needed
    if( !iMmsRepository )
        {
        iMmsRepository = ConnectCenRepL( KUidMmsServerMtm );
        }
        
    // default values for any keys that are not found in CenRep    
    Reset();

    //
    // Start reading the settings
    // If reading of a setting fails -> original value will remain
    //
    LOG(_L("- loading regular settings"));
    TInt temp = 0;
    TInt retval = 0;
    
    // Following are stored as integer codes
    retval = iMmsRepository->Get( KMmsEngineSettingsVersion, temp );
    if( retval == KErrNone ) iSettingsVersion = temp;
    retval = iMmsRepository->Get( KMmsEngineDefaultServiceId, temp );
    if( retval == KErrNone ) iDefaultServiceId = (TMsvId) temp;
    retval = iMmsRepository->Get( KMmsEngineNotificationFolderId, temp );
    if( retval == KErrNone ) iNotificationFolderId = (TMsvId) temp;
    retval = iMmsRepository->Get( KMmsEngineMMBoxFolderId, temp );
    if( retval == KErrNone ) iMMBoxFolderId = (TMsvId) temp;
    retval = iMmsRepository->Get( KMmsEngineApplicationFolderId, temp );
    if( retval == KErrNone ) iApplicationFolderId = (TMsvId) temp;
    retval = iMmsRepository->Get( KMmsEngineReceivingModeHome, temp );
    if( retval == KErrNone ) iReceivingModeHome = temp;
    retval = iMmsRepository->Get( KMmsEngineReceivingModeForeign, temp );
    if( retval == KErrNone ) iReceivingModeForeign = temp;
    retval = iMmsRepository->Get( KMmsEngineMessageClass, temp );
    if( retval == KErrNone ) iMessageClass = temp;
    retval = iMmsRepository->Get( KMmsEngineMessagePriority, temp );
    if( retval == KErrNone ) iMessagePriority = temp;
    // change expiry interval from hours to seconds
    retval = iMmsRepository->Get( KMmsEngineExpiryInterval, temp );
    if( retval == KErrNone ) iExpiryInterval = temp * KMmsSecondsInAnHour;
    retval = iMmsRepository->Get( KMmsEngineExpiryOvershoot, temp );
    if( retval == KErrNone ) iExpiryOvershoot = temp;
    retval = iMmsRepository->Get( KMmsEngineSenderVisibility, temp );
    if( retval == KErrNone ) iSenderVisibility = temp;
    retval = iMmsRepository->Get( KMmsEngineMaximumSendSize, temp );
    if( retval == KErrNone ) iMaxSendSize = temp;
    retval = iMmsRepository->Get( KMmsEngineMaximumReceiveSize, temp );
    if( retval == KErrNone ) iMaxReceiveSize = temp;
    retval =  iMmsRepository->Get( KMmsEngineImageWidth, temp );
    if( retval == KErrNone ) iImageWidth = temp;
    retval = iMmsRepository->Get( KMmsEngineImageHeight, temp );
    if( retval == KErrNone ) iImageHeight = temp;
	retval = iMmsRepository->Get( KMmsEngineCreationMode, temp );
	if( retval == KErrNone ) iCreationMode = temp;
    retval = iMmsRepository->Get( KMmsEngineDeliveryReportWanted, temp );
    if( retval == KErrNone ) iDeliveryReportWanted = temp;
    retval = iMmsRepository->Get( KMmsEngineDeliveryReportSendingAllowed, temp );
    if( retval == KErrNone ) iDeliveryReportAllowed = temp;
    retval = iMmsRepository->Get( KMmsEngineReadReplyReportWanted, temp );
    if( retval == KErrNone ) iReadReplyReportWanted = temp;
	// Following are stored as boolean
    retval = iMmsRepository->Get( KMmsEngineReadReplyReportSendingAllowed, temp );
    if( retval == KErrNone ) iReadReplyReportAllowed = temp;
    retval = iMmsRepository->Get( KMmsEngineLogEmailRecipients, temp );
    if( retval == KErrNone ) iLogEmailRecipients = temp;
    retval = iMmsRepository->Get( KMmsEngineAllowPersonal, temp );
    if( retval == KErrNone ) iAcceptPersonal = temp;
    retval = iMmsRepository->Get( KMmsEngineAllowAdvertisement, temp );
    if( retval == KErrNone ) iAcceptAdvertisement = temp;
    retval = iMmsRepository->Get( KMmsEngineAllowInformational, temp );
    if( retval == KErrNone ) iAcceptInformational = temp;
    retval = iMmsRepository->Get( KMmsEngineAllowAnonymous, temp );
    if( retval == KErrNone ) iAcceptAnonymous = temp;
    retval = iMmsRepository->Get( KMmsEngineLocalMode, temp );
    if( retval == KErrNone ) iLocalMode = temp;
    retval = iMmsRepository->Get( KMmsEngineShowSentTime, temp );
    if ( retval == KErrNone )
        {
        iShowSentTime = temp;
        }
      
    // MMS encapsulation version variation is in central repository
    // We check limits to prevent someone trying to define illegal values
    retval = iMmsRepository->Get( KMmsOmaEncapsulationVersion, temp );
    if ( retval == KErrNone &&
        temp >= KMmsMinimumSupportedVersion &&
        temp <= KMmsMaximumSupportedVersion )
        {
        iMmsVersion = temp;
        }
 
    
	// This setting is always created and maintained "on the fly"
    iFetchOverride = EFalse;

    // AP settings are very important:
    //   if some key/value is not found, a leave will occur.
    iAccesspointArray.Reset();
    TInt arrayLength = 0;
    TInt value = 0;
    // cannot leave even if error: if no access points, count is 0
    retval = iMmsRepository->Get( KMmsEngineAccessPointCount, arrayLength ) ;
    if ( retval == KErrNotFound )
        {
        SaveSetting( KMmsEngineAccessPointCount, 0 );
        }
    LOG2(_L("- Loading AP array of length %d"), arrayLength );
    for( TInt index = 0; index < arrayLength; ++index )
        {
        retval = iMmsRepository->Get( KMmsAccesspointArrayBase + index, value ) ;
        if ( retval == KErrNone )
            {
            // If we are out of memory, appending the element may fail.
            User::LeaveIfError( iAccesspointArray.Append( value ) );
            // array index is safe if the appending succeeded
            LOG3(_L("- array[%d] == %d"), index, iAccesspointArray[ index ] );
            }
        else
            {
            LOG3(_L("- Loading AP array item %d returned error %d"), index, retval );
            }
        }
    // Clean up
    delete iMmsRepository;
    iMmsRepository = NULL;

    //
    // Read MoveToSent setting from general messaging repository
    //
    LOG(_L("- Reading SentItemsInUse key from different repository"));
    TRAP( retval, iMmsRepository = ConnectCenRepL( KCRUidMuiuSettings ) );
    if( retval == KErrNone && iMmsRepository )
        {
        retval = iMmsRepository->Get( KMuiuSentItemsInUse, temp );
        if( retval == KErrNone )
            {
            iMoveToSent = temp;
            }                
        }
    // Clean up
    delete iMmsRepository;
    iMmsRepository = NULL;

    LOG(_L("- settings loaded"));
    
#ifdef _DEBUG
    DebugLogSettings();
#endif
    }

// -----------------------------------------------------------------------------
// SaveSettingsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsSettings::SaveSettingsL()
    {
    LOG(_L("CMmsSettings::SaveSettingsL"));
    
    // Connect CenRep if needed
    if( !iMmsRepository )
        {
        iMmsRepository = ConnectCenRepL( KUidMmsServerMtm );
        }
        
    // MMS Encapsulation version is read-only, cannot be saved.    

    SaveSetting( KMmsEngineDefaultServiceId, iDefaultServiceId );
    SaveSetting( KMmsEngineNotificationFolderId, iNotificationFolderId );
    SaveSetting( KMmsEngineMMBoxFolderId, iMMBoxFolderId );
    SaveSetting( KMmsEngineApplicationFolderId, iApplicationFolderId );
    SaveSetting( KMmsEngineReceivingModeHome, iReceivingModeHome );
    SaveSetting( KMmsEngineReceivingModeForeign, iReceivingModeForeign );
    SaveSetting( KMmsEngineMessageClass, iMessageClass );
    SaveSetting( KMmsEngineMessagePriority, iMessagePriority );
    // Change expiry interval from seconds to hours
    SaveSetting( KMmsEngineExpiryInterval, iExpiryInterval / KMmsSecondsInAnHour );
    SaveSetting( KMmsEngineSenderVisibility, iSenderVisibility );
    SaveSetting( KMmsEngineImageWidth, iImageWidth );
    SaveSetting( KMmsEngineImageHeight, iImageHeight );
    SaveSetting( KMmsEngineCreationMode, iCreationMode );
    SaveSetting( KMmsEngineDeliveryReportWanted, iDeliveryReportWanted );
    SaveSetting( KMmsEngineDeliveryReportSendingAllowed, iDeliveryReportAllowed );
    SaveSetting( KMmsEngineReadReplyReportWanted, iReadReplyReportWanted );
    SaveSetting( KMmsEngineReadReplyReportSendingAllowed, iReadReplyReportAllowed );
    SaveSetting( KMmsEngineLogEmailRecipients, iLogEmailRecipients );
    SaveSetting( KMmsEngineAllowPersonal, iAcceptPersonal );
    SaveSetting( KMmsEngineAllowAdvertisement, iAcceptAdvertisement );
    SaveSetting( KMmsEngineAllowInformational, iAcceptInformational );
    SaveSetting( KMmsEngineAllowAnonymous, iAcceptAnonymous );
    SaveSetting( KMmsEngineLocalMode, iLocalMode );

    // AP array
    SaveAccesspointArrayL();

    // Clean up
    delete iMmsRepository;
    iMmsRepository = NULL;
    
    LOG(_L("- saving OK"));
    }

// -----------------------------------------------------------------------------
// AccessPoint
// -----------------------------------------------------------------------------
//
EXPORT_C TInt32 CMmsSettings::AccessPoint( TInt aIndex ) const
    {
    if( aIndex < 0 || aIndex >= iAccesspointArray.Count() )
        {
        return KErrNotFound;
        }
    return iAccesspointArray[ aIndex ];
    }

// -----------------------------------------------------------------------------
// SaveSetting
//
// This internal method must be called under condition that connection
// to CenRep exists
//
// -----------------------------------------------------------------------------
//
void CMmsSettings::SaveSetting( TUint32 aKey, TInt aValue )
    {
    // If the setting does not exist, it is automatically created
    // It will never return "KErrNotFound"
    // On failure the repository is unmodified.
    // The saving will fail if capacity is not sufficient.
    TInt retval = iMmsRepository->Set( aKey, aValue );
    if( retval != KErrNone ) // retval is also not equal to KErrNotFound
        {
        LOG3( _L("-ERROR saving setting %d: %d"), aKey, retval );
        }
    }

// -----------------------------------------------------------------------------
// SaveAccesspointArrayL
//
// This internal method must be called under condition that connection
// to CenRep exists
//
// -----------------------------------------------------------------------------
//
void CMmsSettings::SaveAccesspointArrayL()
    {
    LOG(_L("CMmsSettings::SaveAccesspointArrayL"));
    
    // Compare array lengths in CenRep and in memory
    TInt origCRArrayCount = 0;
    User::LeaveIfError( iMmsRepository->Get( KMmsEngineAccessPointCount, origCRArrayCount ) );
    TInt localArrayCount = iAccesspointArray.Count();
    // If CenRep array is longer -> delete extra elements from it
    for( TInt index = localArrayCount; index < origCRArrayCount; ++index )
        {
        LOG(_L("- deleting extra element from CenRep"));
        iMmsRepository->Delete( KMmsAccesspointArrayBase + index );
        }
        
    // Now arrays have the same length OR localarray is longer
    // (i.e. elements have been added)

    // Save new array length to CenRep
    LOG2(_L("- saving AP array of length (%d)"), localArrayCount );
    SaveSetting( KMmsEngineAccessPointCount, localArrayCount );
    
    // Save the array itself
    for( TInt index = 0; index < localArrayCount; ++index )
        {
        SaveSetting( KMmsAccesspointArrayBase + index, (TInt)iAccesspointArray[ index ] ); 
        LOG3(_L("- saved array[%d] with value %d"), index, iAccesspointArray[ index ]);
        }
    }

// -----------------------------------------------------------------------------
// ValidateSettings
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMmsSettings::ValidateSettings()
    {
    LOG(_L("CMmsSettings::ValidateSettings"));
    //
    // Check basic settings and return error if something is NOK
    //
    if( iExpiryInterval < 0 || 
        iExpiryInterval > KMmsMaxExpiry )
        {
        LOG(_L("- NOK: iExpiryInterval"));
        return KMmsErrorInvalidSettings;
        }
    if( iSenderVisibility != 0 &&
        ( iSenderVisibility < EMmsMinimumSenderVisibility || 
          iSenderVisibility > EMmsMaximumSenderVisibility ) )
        {
        LOG(_L("- NOK: iSenderVisibility"));
        return KMmsErrorInvalidSettings;
        }

    if( iDeliveryReportWanted != KMmsYes && 
        iDeliveryReportWanted != KMmsNo )
        {
        LOG(_L("- NOK: iDeliveryReportWanted"));
        return KMmsErrorInvalidSettings;
        }
    if( iDeliveryReportAllowed != KMmsYes && 
        iDeliveryReportAllowed != KMmsNo )
        {
        LOG(_L("- NOK: iDeliveryReportAllowed"));
        return KMmsErrorInvalidSettings;
        }
    if( iReadReplyReportWanted != KMmsYes && 
        iReadReplyReportWanted != KMmsNo )
        {
        LOG(_L("- NOK: iReadReplyReportWanted"));
        return KMmsErrorInvalidSettings;
        }
    if( iReceivingModeHome < EMmsReceivingMin || 
        iReceivingModeHome > EMmsReceivingMax )
        {
        LOG(_L("- NOK: iReceivingModeHome"));
        return KMmsErrorInvalidSettings;
        }
    if( iReceivingModeForeign < EMmsReceivingMin ||
        iReceivingModeForeign > EMmsReceivingMax )
        {
        LOG(_L("- NOK: iReceivingModeForeign"));
        return KMmsErrorInvalidSettings;
        }
    if( iMessagePriority < EMmsMinimumPriority || 
        iMessagePriority > EMmsMaximumPriority )
        {
        LOG(_L("- NOK: iMessagePriority"));
        return KMmsErrorInvalidSettings;
        }
    if( iMessageClass < EMmsMimimumMessageClass ||
        iMessageClass > EMmsMaximumMessageClass )
        {
        LOG(_L("- NOK: iMessageClass"));
        return KMmsErrorInvalidSettings;
        }
        
    //
    // Check AP Array (not checked in localmode)
    //
    TInt error = KErrNone;
    if( !LocalMode() )
        {
        if( iAccesspointArray.Count() == 0 )
            {
            LOG(_L("- NOK: AP array length == 0"));
            error = KMmsErrorNoWAPAccessPoint;
            }
        else // AP array not empty:
            {
            
            // Accesspoint array is checked by CMmsConnectionInitiator
            // All defined accesspoints should contain proper values in CommDb
            
            for( TInt index = 0; index < iAccesspointArray.Count(); ++index )
                {
                TUint32 ap = 0;
                HBufC* serverUri = NULL;
                HBufC8* proxyAddress = NULL;
                TBool proxyInUse = EFalse;
                TRAP( error, CMmsConnectionInitiator::GetParametersL(
                    (TUint32)iAccesspointArray[ index ],
                    ap,
                    serverUri,
                    proxyInUse,
                    proxyAddress ) );
                if( error == KErrNone ) // call did not leave                   
                    {
                    if ( ap == 0 )
                        {
                        error = KMmsErrorAP1Invalid;
                        }
                    else if ( !serverUri )
                        {
                        error = KMmsErrorNoURI1;
                        }
                    else if ( serverUri->Length() == 0 )
                        {
                        error = KMmsErrorNoURI1;
                        }
                    else
                        {
                        // keep LINT happy.
                        }
                    }
                if( serverUri )
                    {
                    delete serverUri;
                    }
                if( proxyAddress )
                    {
                    delete proxyAddress;
                    }
                if( error != KErrNone )
                    {
                    return error;
                    }
                } // for loop
            }
        }
    return error;
    }
        
// -----------------------------------------------------------------------------
// RestoreFactorySettingsL
// -----------------------------------------------------------------------------

EXPORT_C void CMmsSettings::RestoreFactorySettingsL(
    CMsvSession& aSession, TMmsFactorySettingsLevel /* aLevel */  )
    {
    LOG(_L("CMmsSettings::RestoreFactorySettingsL "));
        
    // Connect CenRep
    if( !iMmsRepository )
        {
        iMmsRepository = ConnectCenRepL( KUidMmsServerMtm );
        }

    // Reset the repository
    LOG(_L("- reseting repository"));
    iMmsRepository->Reset();
    
    CreateNewServiceL( aSession );
    
    // LoadSettings resets any undefined values to hard-coded defaults
    LoadSettingsL();
    
#ifdef __WINS__
	// Connect CenRep
    if( !iMmsRepository )
        {
        iMmsRepository = ConnectCenRepL( KUidMmsServerMtm );
        }
    iLocalMode = ETrue;
    SaveSetting( KMmsEngineLocalMode, iLocalMode );
#endif // __WINS__    
    
    // Clean up
    delete iMmsRepository;
    iMmsRepository = NULL;
    }

// -----------------------------------------------------------------------------
// AddAccessPointL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsSettings::AddAccessPointL( TUint32 aAccessPointId, TInt aIndex )
    {
    LOG3(_L("CMmsSettings::AddAccessPointL: array[%d] = %d"), aIndex, aAccessPointId );
    // Basic checks for the arguments
    // aAccessPoint can't be negative as it is TUint32
    if( aIndex < 0 || aAccessPointId == 0)
        {
        LOG(_L("- KErrArgument"));
        User::Leave( KErrArgument );
        }
        
    // If index points past the current array, insert new id as last one
    TInt count = iAccesspointArray.Count();
    if( aIndex > count )
        {
        LOG(_L("- index too big, adjusting.."));
        aIndex = count;
        }

    // If the same reference already exists, ignore adding and just return
    for(TInt index = 0; index < count; ++index )
        {
        if( iAccesspointArray[ index ] == aAccessPointId )
            {
            LOG(_L("- reference already in the array, not adding."));
            return;
            }
        }
    
    // Add
    iAccesspointArray.Insert( aAccessPointId, aIndex );
    }

// -----------------------------------------------------------------------------
// DeleteAccessPointL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsSettings::DeleteAccessPointL( TInt aIndex )
    {
    LOG2(_L("CMmsSettings::DeleteAccessPointL: array[%d]"), aIndex );
    if( aIndex >= 0 && aIndex < iAccesspointArray.Count() )
        {
        iAccesspointArray.Remove( aIndex );
        }
    else
        {
        LOG(_L("- KErrArgument"));
        User::Leave( KErrArgument );
        }
    }

// -----------------------------------------------------------------------------
// CreateNewServiceL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsSettings::CreateNewServiceL( CMsvSession& aSession )
    {
    LOG(_L("CMmsSettings::CreateNewServiceL"));
    TMsvId newId = KMsvRootIndexEntryId;

    // Connect CenRep if needed
    if( !iMmsRepository )
        {
        iMmsRepository = ConnectCenRepL( KUidMmsServerMtm );
        }

#ifdef __WINS__
    // In WINS, localmode is the default
    TInt retval = iMmsRepository->Set( KMmsEngineLocalMode, ETrue );
    if( retval != KErrNone )
        {
        LOG(_L("-ERROR: Could not set localmode ON"));
        }
    SetLocalMode( ETrue );
#endif // __WINS__    

    // Create utility class to handle message store operations
    CMmsMessageStoreUtils* messageStoreUtils = CMmsMessageStoreUtils::NewL( aSession );
    CleanupStack::PushL( messageStoreUtils ); // ***
    
    const TInt KMmsMaxTries = 50; // Just to keep Codescanner happy
    TInt maxTries = KMmsMaxTries;
    TBool done( EFalse );
    TInt error = KErrNone;
    while ( maxTries && !done )
        {
        error = iMmsRepository->StartTransaction( CRepository::EReadWriteTransaction );
        if ( error == KErrNone )
            {
            // Handle service entry
            iMmsRepository->CleanupCancelTransactionPushL();
            newId = messageStoreUtils->CreateServiceEntryL();
            iDefaultServiceId = newId;
            error =  iMmsRepository->Set( KMmsEngineDefaultServiceId, (TInt)newId );
            LOG2(_L("- Service entry created: %d"), newId );
        
            // Handle notification folder
            if ( error == KErrNone )
                {
                newId = messageStoreUtils->CreateFolderEntryL( KMMSNotificationFolder );
                iNotificationFolderId = newId;
                error = iMmsRepository->Set( KMmsEngineNotificationFolderId, (TInt)newId );
                LOG2(_L("- Notification folder created: %d"), newId );
                }

            // Handle mmbox folder
            if ( error == KErrNone )
                {
                newId = messageStoreUtils->CreateFolderEntryL( KMmsMmboxFolder );
                iMMBoxFolderId = newId;
                error = iMmsRepository->Set( KMmsEngineMMBoxFolderId, (TInt)newId );
                LOG2(_L("- MMBox folder created: %d"), newId );
                }

            // Handle application folder
            if ( error == KErrNone )
                {
                newId = messageStoreUtils->CreateFolderEntryL( KMmsApplicationFolder );
                iApplicationFolderId = newId;
                error = iMmsRepository->Set( KMmsEngineApplicationFolderId, (TInt)newId );
                LOG2(_L("- Application folder created: %d"), newId );
                }
            TUint32 keyInfo = 0; 
            CleanupStack::Pop(); // CleanupCancelTransactionPushL
            TInt commitError = KErrNone;
            commitError = iMmsRepository->CommitTransaction( keyInfo );
            if ( error == KErrNone && commitError == KErrNone )
                {
                done = ETrue;
                }
            else
                {
                LOG3(_L("- error and commit error: %d %d"), error, commitError );
                maxTries--;
                }
            }
        else
            {
            // Another thread or process is probably trying simultaneous access.
            // Wait a little bit and retry.
            LOG2(_L("- Start transaction returned error: %d"), error );
            maxTries--;
            const TInt KMmsCreateServiceWait = 300000;  // 0.3 seconds
            User::After( KMmsCreateServiceWait );
            }
        }

    // Clean up
    CleanupStack::PopAndDestroy( messageStoreUtils );
    delete iMmsRepository;
    iMmsRepository = NULL;
    User::LeaveIfError( error );
    }

// -----------------------------------------------------------------------------
// CreateFolderEntryL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMmsSettings::CreateFolderEntryL( 
    CMsvSession& aSession, TMsvId aParentFolder, const TDesC& aFolderName, TMsvId& aFolderId )
    {
    CMmsMessageStoreUtils* messageStoreUtils = CMmsMessageStoreUtils::NewL( aSession );
    CleanupStack::PushL( messageStoreUtils );
    
    TInt error = messageStoreUtils->CreateFolderEntryL( aParentFolder, aFolderName, aFolderId );
    
    CleanupStack::PopAndDestroy( messageStoreUtils );
    return error;    
    }

// -----------------------------------------------------------------------------
// FolderEntryL
// -----------------------------------------------------------------------------
//    
EXPORT_C TMsvId CMmsSettings::FolderEntryL(
    CMsvSession& aSession, TMsvId aParentFolder, const TDesC& aFolderName )
    {
    CMmsMessageStoreUtils* messageStoreUtils = CMmsMessageStoreUtils::NewL( aSession );
    CleanupStack::PushL( messageStoreUtils );
    
    TMsvId folderId = messageStoreUtils->FolderEntryL( aParentFolder, aFolderName );
    
    CleanupStack::PopAndDestroy( messageStoreUtils );
    return folderId;
        
    }

// -----------------------------------------------------------------------------
// ConnectCenRepL
// -----------------------------------------------------------------------------
//
CRepository* CMmsSettings::ConnectCenRepL( const TUid aRepositoryId )
    {
    CRepository* repository = NULL;
    repository = CRepository::NewL( aRepositoryId );
    return repository;
    }

#ifdef __WINS__ // following code is for testing/development purposes
// ---------------------------------------------------------
// ReadLocalModeDirectoriesL
//
// ---------------------------------------------------------
//
void CMmsSettings::ReadLocalModeDirectoriesL()
    {
    LOG(_L("CMmsSettings::ReadLocalModeDirectoriesL"));
    // Establish FileSystem connection
    RFs fs;
    User::LeaveIfError( fs.Connect() ); // ***
    CleanupClosePushL( fs );

    // Create readstream
    RFileReadStream reader;
    TInt err = reader.Open( fs, KMmsLocalModeConfigFile, EFileShareReadersOnly );
    if( err != KErrNone )
        {
        CleanupStack::PopAndDestroy( &fs ); // close fs
        return;
        }
    else
        {
        CleanupClosePushL( reader ); // ***
        }
    
    // Start reading data
    TChar delim = 0x000A;
    TBuf<KMmsRowBufferLength> rowBuffer;
    FOREVER
        {
        TRAP( err, reader.ReadL( rowBuffer, delim ) );
        if( err == KErrEof )
            {
            CleanupStack::PopAndDestroy( &reader ); // close reader
            CleanupStack::PopAndDestroy( &fs ); // close fs
            return;
            }
        TInt length = rowBuffer.Length();
        if( length > 2 )
            {
            // Check for comment line
            if( rowBuffer[0] == 0x0023 ) // 0x23 == '#'
                {
                continue;
                }
            // Check for start of file (BOM)
            if( rowBuffer[0] == 0xFEFF )
                {
                rowBuffer.Delete( 0, 1 );
                length = rowBuffer.Length();
                }
            // Drop CR+LF from the end of line
            rowBuffer.Delete( length - 2, 2 );
            // Trim spaces
            rowBuffer.Trim();
  
            TInt separatorPosition = 0;
            separatorPosition = rowBuffer.Find( _L("=") );
            if( separatorPosition > 0 )
                {
                if( rowBuffer.Left( separatorPosition ).CompareF( KMmsLocalmodeInDirectory ) == 0 )
                    {
                    iLocalModeInDir = rowBuffer.Mid( separatorPosition+1 );
                    }
                if( rowBuffer.Left( separatorPosition ).CompareF( KMmsLocalmodeOutDirectory )
                    == 0 )
                    {
                    iLocalModeOutDir = rowBuffer.Mid( separatorPosition+1 );
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy( &reader ); // close reader
    CleanupStack::PopAndDestroy( &fs ); // close fs
    }
#endif // __WINS__

#ifdef _DEBUG // Following is only for testing purposes
void CMmsSettings::DebugLogSettings()
    {
    // Log goes to logs\mmssettings\settings.txt
    LOG(_L("SETTINGS CONTENT:"));
    LOG2(_L("    iSettingsVersion = %d"), iSettingsVersion);
    LOG3(_L("    iMmsVersion = %d.%d"), ( iMmsVersion & 0xF0 ) >> 4, iMmsVersion & 0x0F );
    LOG2(_L("    iDefaultServiceId =     %d"), iDefaultServiceId);
    LOG2(_L("    iNotificationFolderId = %d"), iNotificationFolderId);
    LOG2(_L("    iMMBoxFolderId =        %d"), iMMBoxFolderId);
    LOG2(_L("    iApplicationFolderId = %d"), iApplicationFolderId);
    LOG2(_L("    iAccesspointArray len = %d"), iAccesspointArray.Count());
    LOG2(_L("    iReceivingModeHome = %d"), iReceivingModeHome);
    LOG2(_L("    iReceivingModeForeign = %d"), iReceivingModeForeign);
    LOG2(_L("    iAcceptPersonal = %d"), iAcceptPersonal);
    LOG2(_L("    iAcceptAdvertisement = %d"), iAcceptAdvertisement);
    LOG2(_L("    iAcceptInformational = %d"), iAcceptInformational);
    LOG2(_L("    iAcceptAnonymous = %d"), iAcceptAnonymous);
    LOG2(_L("    iDeliveryReportWanted = %d"), iDeliveryReportWanted);
    LOG2(_L("    iDeliveryReportAllowed = %d"), iDeliveryReportAllowed);
    LOG2(_L("    iReadReplyReportWanted = %d"), iReadReplyReportWanted);
    LOG2(_L("    iReadReplyReportAllowed = %d"), iReadReplyReportAllowed);
    LOG2(_L("    iMessageClass = %d"), iMessageClass);
    LOG2(_L("    iExpiryInterval = %d"), iExpiryInterval);
    LOG2(_L("    iSenderVisibility = %d"), iSenderVisibility);
    LOG2(_L("    iMessagePriority = %d"), iMessagePriority);
    LOG2(_L("    iMaxSendSize = %d"), iMaxSendSize);
    LOG2(_L("    iMaxReceiveSize = %d"), iMaxReceiveSize);
    LOG2(_L("    iImageWidth = %d"), iImageWidth);
    LOG2(_L("    iImageHeight = %d"), iImageHeight);
    LOG2(_L("    iMoveToSent = %d"), iMoveToSent);
    LOG2(_L("    iLogEmailRecipients = %d"), iLogEmailRecipients);
    LOG2(_L("    iExpiryOvershoot = %d"), iExpiryOvershoot);
    LOG2(_L("    iCreationMode = %d"), iCreationMode);
    LOG2(_L("    iFetchOverride = %d"), iFetchOverride);
    LOG2(_L("    iShowSentTime = %d"), iShowSentTime);
    }
#endif // _DEBUG

// =========================== OTHER EXPORTED FUNCTIONS ========================

//  End of File  
