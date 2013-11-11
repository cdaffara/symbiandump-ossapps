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
*     MMS server settings
*
*/



// -----------------------------------------------------------------------------
// SettingsVersion
// -----------------------------------------------------------------------------
//
inline TInt32 CMmsSettings::SettingsVersion() const
    {
    return iSettingsVersion;
    }

// -----------------------------------------------------------------------------
// Service
// -----------------------------------------------------------------------------
//
inline TMsvId CMmsSettings::Service() const
    {
    return iDefaultServiceId;
    }
    
// -----------------------------------------------------------------------------
// NotificationFolder
// -----------------------------------------------------------------------------
//
inline TMsvId CMmsSettings::NotificationFolder() const
    {
    return iNotificationFolderId;
    }

// -----------------------------------------------------------------------------
// MMBoxFolder
// -----------------------------------------------------------------------------
//
inline TMsvId CMmsSettings::MMBoxFolder() const
    {
    return iMMBoxFolderId;
    }

// -----------------------------------------------------------------------------
// ApplicationFolder
// -----------------------------------------------------------------------------
//
inline TMsvId CMmsSettings::ApplicationFolder() const
    {
    return iApplicationFolderId;
    }
    
// -----------------------------------------------------------------------------
// AccessPointCount
// -----------------------------------------------------------------------------
//
inline TInt CMmsSettings::AccessPointCount() const
    {
    return iAccesspointArray.Count();
    }

// -----------------------------------------------------------------------------
// AcceptPersonalMessages
// -----------------------------------------------------------------------------
//
inline TBool CMmsSettings::AcceptPersonalMessages() const
    {
    return iAcceptPersonal;
    }
    
// -----------------------------------------------------------------------------
// AcceptAdvertisementMessages
// -----------------------------------------------------------------------------
//
inline TBool CMmsSettings::AcceptAdvertisementMessages() const
    {
    return iAcceptAdvertisement;
    }

// -----------------------------------------------------------------------------
// AcceptInformationalMessages
// -----------------------------------------------------------------------------
//
inline TBool CMmsSettings::AcceptInformationalMessages() const
    {
    return iAcceptInformational;
    }

// -----------------------------------------------------------------------------
// AcceptAnonymousMessages
// -----------------------------------------------------------------------------
//
inline TBool CMmsSettings::AcceptAnonymousMessages() const
    {
    return iAcceptAnonymous;
    }

// -----------------------------------------------------------------------------
// SetAcceptPersonalMessages
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetAcceptPersonalMessages( TBool aPersonalAllowed )
    {
    iAcceptPersonal = aPersonalAllowed;
    }

// -----------------------------------------------------------------------------
// SetAcceptAdvertisementMessages
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetAcceptAdvertisementMessages( TBool aAdvertisementsAllowed )
    {
    iAcceptAdvertisement = aAdvertisementsAllowed;
    }

// -----------------------------------------------------------------------------
// SetAcceptInformationalMessages
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetAcceptInformationalMessages( TBool aInformationalAllowed )
    {
    iAcceptInformational = aInformationalAllowed;
    }

// -----------------------------------------------------------------------------
// SetAcceptAnonymousMessages
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetAcceptAnonymousMessages( TBool aAnonymousAllowed )
    {
    iAcceptAnonymous = aAnonymousAllowed;
    }

// -----------------------------------------------------------------------------
// ExpiryInterval
// -----------------------------------------------------------------------------
//
inline TInt32 CMmsSettings::ExpiryInterval() const
    {
    return iExpiryInterval;
    }

// -----------------------------------------------------------------------------
// SetExpiryInterval
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetExpiryInterval( TInt32 aExpiryInterval )
    {
    iExpiryInterval = aExpiryInterval;
    }

// -----------------------------------------------------------------------------
// SenderVisibility
// -----------------------------------------------------------------------------
//
inline TInt32 CMmsSettings::SenderVisibility() const
    {
    return iSenderVisibility;
    }

// -----------------------------------------------------------------------------
// SetSenderVisibility
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetSenderVisibility( TMmsMessageSenderVisibility aSenderVisibility )
    {
    iSenderVisibility = aSenderVisibility;
    }

// -----------------------------------------------------------------------------
// DeliveryReportWanted
// -----------------------------------------------------------------------------
//
inline TInt32 CMmsSettings::DeliveryReportWanted() const
    {
    return iDeliveryReportWanted;
    }

// -----------------------------------------------------------------------------
// SetDeliveryReportWanted
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetDeliveryReportWanted( TInt32 aDeliveryReportWanted )
    {
    iDeliveryReportWanted = aDeliveryReportWanted;
    }
            
// -----------------------------------------------------------------------------
// DeliveryReportSendingAllowed
// -----------------------------------------------------------------------------
//
inline TInt32 CMmsSettings::DeliveryReportSendingAllowed() const
    {
    return iDeliveryReportAllowed;
    }

// -----------------------------------------------------------------------------
// SetDeliveryReportSendingAllowed
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetDeliveryReportSendingAllowed( 
    TInt32 aDeliveryReportSendingAllowed )
    {
    iDeliveryReportAllowed = aDeliveryReportSendingAllowed;
    }

// -----------------------------------------------------------------------------
// ReadReplyReportWanted
// -----------------------------------------------------------------------------
//
inline TInt32 CMmsSettings::ReadReplyReportWanted() const
    {
    return iReadReplyReportWanted; 
    }

// -----------------------------------------------------------------------------
// SetReadReplyReportWanted
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetReadReplyReportWanted( TInt32 aReadReplyReportWanted )
    {
    iReadReplyReportWanted = aReadReplyReportWanted;
    }

// -----------------------------------------------------------------------------
// ReadReplyReportSendingAllowed
// -----------------------------------------------------------------------------
//
inline TBool CMmsSettings::ReadReplyReportSendingAllowed() const
    {
    return iReadReplyReportAllowed;
    }

// -----------------------------------------------------------------------------
// SetReadReplyReportSendingAllowed
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetReadReplyReportSendingAllowed( 
    TBool aReadReplyReportSendingAllowed )
    {
    iReadReplyReportAllowed = aReadReplyReportSendingAllowed;
    }

// -----------------------------------------------------------------------------
// MessagePriority
// -----------------------------------------------------------------------------
//
inline TInt32 CMmsSettings::MessagePriority() const
    {
    return iMessagePriority;
    }

// -----------------------------------------------------------------------------
// SetMessagePriority
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetMessagePriority( TMmsMessagePriority aMessagePriority )
    {
    iMessagePriority = aMessagePriority;
    }

// -----------------------------------------------------------------------------
// MessageClass
// -----------------------------------------------------------------------------
//
inline TInt32 CMmsSettings::MessageClass() const
    {
    return iMessageClass;
    }

// -----------------------------------------------------------------------------
// SetMessageClass
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetMessageClass( TMmsMessageClass aMessageClass )
    {
    iMessageClass = aMessageClass;
    }

// -----------------------------------------------------------------------------
// MaximumSendSize
// -----------------------------------------------------------------------------
//
inline TInt32 CMmsSettings::MaximumSendSize() const
    {
    return iMaxSendSize;
    }

// -----------------------------------------------------------------------------
// MaximumReceiveSize
// -----------------------------------------------------------------------------
//
inline TInt32 CMmsSettings::MaximumReceiveSize() const
    {
    return iMaxReceiveSize;
    }

// -----------------------------------------------------------------------------
// ImageWidth
// -----------------------------------------------------------------------------
//
inline TInt32 CMmsSettings::ImageWidth() const
    {
    return iImageWidth;
    }

// -----------------------------------------------------------------------------
// SetImageWidth
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetImageWidth( TInt32 aImageWidth )
    {
    iImageWidth = aImageWidth;
    }

// -----------------------------------------------------------------------------
// ImageHeight
// -----------------------------------------------------------------------------
//
inline TInt32 CMmsSettings::ImageHeight() const
    {
    return iImageHeight;
    }

// -----------------------------------------------------------------------------
// SetImageHeight
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetImageHeight( TInt32 aImageHeight )
    {
    iImageHeight = aImageHeight;
    }

// -----------------------------------------------------------------------------
// MoveToSent
// -----------------------------------------------------------------------------
//
inline TBool CMmsSettings::MoveToSent() const
    {
    return iMoveToSent;
    }

// -----------------------------------------------------------------------------
// FetchOverride
// -----------------------------------------------------------------------------
//
inline TBool CMmsSettings::FetchOverride() const
    {
    return iFetchOverride;
    }

// -----------------------------------------------------------------------------
// SetFetchOverride
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetFetchOverride( TBool aOverride )
    {
    iFetchOverride = aOverride;
    }

// -----------------------------------------------------------------------------
// LogEmailRecipients
// -----------------------------------------------------------------------------
//
inline TBool CMmsSettings::LogEmailRecipients() const
    {
    return iLogEmailRecipients;
    }

// -----------------------------------------------------------------------------
// SetLogEmailRecipients
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetLogEmailRecipients( TBool aEmailLogging )
    {
    iLogEmailRecipients = aEmailLogging;
    }

// -----------------------------------------------------------------------------
// ExpiryOvershoot
// -----------------------------------------------------------------------------
//
inline TInt CMmsSettings::ExpiryOvershoot() const
    {
    return iExpiryOvershoot;
    }

// -----------------------------------------------------------------------------
// CreationMode
// -----------------------------------------------------------------------------
//
inline TInt32 CMmsSettings::CreationMode() const
    {
    return iCreationMode;
    }

// -----------------------------------------------------------------------------
// SetCreationMode
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetCreationMode( TMmsCreationMode aCreationMode )
    {
    iCreationMode = aCreationMode;
    }

// -----------------------------------------------------------------------------
// SetReceivingModeHome
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetReceivingModeHome( TMmsReceivingMode aReceivingMode )
    {
    iReceivingModeHome = aReceivingMode;
    }

// -----------------------------------------------------------------------------
// ReceivingModeHome
// -----------------------------------------------------------------------------
//
inline TMmsReceivingMode CMmsSettings::ReceivingModeHome() const
    {
    return (TMmsReceivingMode)iReceivingModeHome;
    }

// -----------------------------------------------------------------------------
// SetReceivingModeForeign
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetReceivingModeForeign( TMmsReceivingMode aReceivingMode )
    {
    iReceivingModeForeign = aReceivingMode;
    }

// -----------------------------------------------------------------------------
// ReceivingModeForeign
// -----------------------------------------------------------------------------
//
inline TMmsReceivingMode CMmsSettings::ReceivingModeForeign() const
    {
    return (TMmsReceivingMode)iReceivingModeForeign;
    }

// -----------------------------------------------------------------------------
// LocalMode
// -----------------------------------------------------------------------------
//
inline TBool CMmsSettings::LocalMode() const
    {
    return iLocalMode;
    }

// -----------------------------------------------------------------------------
// SetLocalMode
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetLocalMode( TBool aLocalMode )
    {
    iLocalMode = aLocalMode;
    }
    
// -----------------------------------------------------------------------------
// LocalModeIn
// -----------------------------------------------------------------------------
//
inline const TDesC& CMmsSettings::LocalModeIn()
    {
    #ifdef __WINS__
    return iLocalModeInDir;
    #else
    return iLocalModeDir ? iLocalModeDir->Des() : TPtrC( KMmsDefaultLocalModeDir );
    #endif
    }

// -----------------------------------------------------------------------------
// SetLocalModeIn
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetLocalModeIn( const TDesC& aPath )
    {
    #ifdef __WINS__
    iLocalModeInDir = aPath;
    #else
    // In hardware this function is needed only for message variation
    // In this case MMS server MTM 
    delete iLocalModeDir;
    iLocalModeDir = NULL;
    if ( aPath.Length() > 0 )
        {
        // if allocation fails, iLocalModeDir = NULL
        // The value will remain the default
        iLocalModeDir = aPath.Alloc();
        }
    #endif
    }

// -----------------------------------------------------------------------------
// LocalModeOut
// -----------------------------------------------------------------------------
//
inline const TDesC& CMmsSettings::LocalModeOut()
    {
    #ifdef __WINS__
    return iLocalModeOutDir;
    #else
    // only one directory in hardware
    return LocalModeIn();
    #endif    
    }

// -----------------------------------------------------------------------------
// SetLocalModeOut
// -----------------------------------------------------------------------------
//
inline void CMmsSettings::SetLocalModeOut( const TDesC& aPath )
    {
    #ifdef __WINS__
    iLocalModeOutDir = aPath;
    #else
    // only one directory in hardware
    SetLocalModeIn( aPath );
    #endif    
    }


// -----------------------------------------------------------------------------
// ShowSentTime
// -----------------------------------------------------------------------------
//
inline TBool CMmsSettings::ShowSentTime() const
    {
    return iShowSentTime;
    }
    

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TInt16 CMmsSettings::MmsVersion() const
    {
    return iMmsVersion;
    }


// End of File
