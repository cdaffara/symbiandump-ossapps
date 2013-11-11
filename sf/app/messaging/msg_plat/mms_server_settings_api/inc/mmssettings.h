/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  mmssettings  declaration
*
*/



#ifndef __MMSSERVERSETTINGS_H
#define __MMSSERVERSETTINGS_H

// INCLUDES
#include <e32base.h>

// USERINCLUDE FILES
#include "mmsconst.h"

// CONSTANTS
// Default folder for local mode (this is NOT __WINS__ only on purpose)
_LIT( KMmsDefaultLocalModeDir, "c:\\mmstest\\" );
// For testing purpose - simulated folder for mmbox view in local mode
_LIT( KMmsMMBoxDirectory, "c:\\mmsmmbox\\");

#ifdef __WINS__
// Support for localmode polling in WINS emulator
// This ini-file is used only in local mode and in WINS
// There is no need for data caging in this case as the user
// may edit any file in WINS anyway.
// And this directory is for testing only
_LIT( KMmsLocalModeConfigFile,      "c:\\system\\data\\localmode.ini" );
_LIT( KMmsLocalmodeInDirectory,     "mmsin" );
_LIT( KMmsLocalmodeOutDirectory,    "mmsout" );
_LIT( KMmsLocalmodePollingInterval, "pollinginterval" );
#endif

// Aproximated amount of disk space needed to store one more access point
// needed in diskspace checks
const TInt KMmsAccessPointDiskSpaceNeed = 128;

// Default value for maximum expirytime: 168 hours (one week) expressed in seconds
const TInt KMmsMaxExpiry = 168 * 3600;

// Default values for how much data can be sent and received
const TInt KMmsMaxReceiveSize = 304*1024;
const TInt KMmsMaxSendSize = 300*1024;

const TInt KMmsMaxNumberOfRegisteredApplications = 50;

const TInt KMmsExpiryOvershoot = 300; // 5 min

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CMsvSession;
class CRepository;

// CLASS DECLARATION
/**
* CMmsSettings contains all Mms Engine's settings. Loading and Saving methods
* are offered as well as accessors for all setting values. Mutators are offered
* to most of the settings, but there are a few 'read-only' settings.
* @lib mmsserversettings.lib
* @since 3.0
*/
class CMmsSettings : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMmsSettings* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CMmsSettings();

    public: // New functions

        ///////////////////////////////////////////////////////////////////////
        // General Methods
        ///////////////////////////////////////////////////////////////////////
         
        /**
        * LoadSettingsL loads the settings from Central Repository
        */
        IMPORT_C void LoadSettingsL();

        /**
        * SaveSettingsL saves the settings to Central Repository
        */
        IMPORT_C void SaveSettingsL();

        /**
        * Validate current MMS settings
        * Checks that access points point to valid entries in commsdb
        * @return error code
        */
        IMPORT_C TInt ValidateSettings();

        /**
        * Reset clears the member variables representing Mms settings
        * (the method does not modify data in CenRep).
        */
        IMPORT_C void Reset();

        /*
        * RestoreFactorySettingsL
        * Restore factory settings into CMmsSettings class.
        * Only instance data is changed. The data must be
        * stored onto disk afterwards (call SaveSettingsL)
        * @param aSession session
        * @param aLevel level of restoration // Not supported
        * Possible values for the level:
        *   EMmsFactorySettingsLevelNormal,
        *   EMmsFactorySettingsLevelDeep
        */
        IMPORT_C void RestoreFactorySettingsL(
        	CMsvSession& aSession,
            TMmsFactorySettingsLevel aLevel );
        
        /**
        * SettingsVersion returns the version information of the settings.
        */
        inline TInt32 SettingsVersion() const;
        
        /**
        * CopyL copies the contents (member values) from aMmsSettings.
        * @param aMmsSettings refernce to another CMmsSettings object
        */
        IMPORT_C void CopyL( const CMmsSettings& aMmsSettings );        

        ///////////////////////////////////////////////////////////////////////
        // Service related Methods
        ///////////////////////////////////////////////////////////////////////

        /**
        * Creates a new service entry into messagestore and saves the entryid
        * into CenRep. The invisible Notification folder and MMBox folder will
        * also be created.
        * Before creating new entries, MessageStore is first searched for 
        * old entries. If old entries are found, they will be used.
        * Note that the method can be called only from client side of 
        * the Messaging architecture.
        * @param messageserver session
        */
        IMPORT_C void CreateNewServiceL( CMsvSession& aSession );

        /**
        * Gets default service for MMS
        * @return TMsvId of the default MMS service entry.
        * If no service defined KMsvNullIndexEntryId is returned.
        */
        inline TMsvId Service() const;
        
        
        ///////////////////////////////////////////////////////////////////////
        // Folders  related methods
        // 
        ///////////////////////////////////////////////////////////////////////
        
        /**
        * Gets notification folder id
        * @return TMsvId of the notification folder
        */
        inline TMsvId NotificationFolder() const;

        /**
        * Gets mmbox folder id
        * @return TMsvId of the mmbox folder
        */
        inline TMsvId MMBoxFolder() const;
        
        /**
        * Gets application folder id
        * @return TMsvId of the application folder
        */
        inline TMsvId ApplicationFolder() const;
        
        /**
        * CreateFolderEntryL creates a new folder entry under a given folder 
        * with 'aFolderName' name.
        * @param aSession messageserver session
        * @param aParentFolder a folder, under which the new folder is created.
        * @param aFolderId entry id of the new folder or 
        * if the folder already exists, the folder id of the existing folder
        * @return KErrNone if successful.
        * KErrAlreadyExists, if a folder with same name exists under aParentFoder
        */
        IMPORT_C TInt CreateFolderEntryL( 
            CMsvSession& aSession, TMsvId aParentFolder, const TDesC& aFolderName, TMsvId& aFolderId );
         
        /**
        * Gets a folder entry id under a given folder id  
        * with 'aFolderName' name.
        * @param aSession messageserver session
        * @param aParentFolder a folder, under where the folder is searched
        * @param aFolderName name of folder entry
        * @return entry id of the folder 
        */
        IMPORT_C TMsvId FolderEntryL( CMsvSession& aSession, TMsvId aParentFolder, const TDesC& aFolderName );

        ///////////////////////////////////////////////////////////////////////
        // Accesspoint array related methods
        // (the array contains APs in preference order)
        ///////////////////////////////////////////////////////////////////////
        
        /**
        * AccessPointCount
        * @return number of accesspoints in the accesspoint array
        */
        inline TInt AccessPointCount() const;

        /**
        * AccessPoint is accessor for accesspoint array
        * @param array index of the element that is requested.
        *   No checks are made for the given parameter so the caller is
        *   responsible for parameter 
        *   ( i.e. aIndex should be between 0..AccessPointCount() )
        * @return accesspoint reference to commdb
        *   or KErrNotFound if given index is incorrect
        */
        IMPORT_C TInt32 AccessPoint( TInt aIndex ) const;

        /**
        * Adds an accesspoint id to a specific index in the array
        * @param aAccessPointId specifies the accesspoint id
        * @param aIndex specifies the array index
        */
        IMPORT_C void AddAccessPointL( TUint32 aAccessPointId, TInt aIndex );

        /**
        * Deletes an accesspoint id from a specific index in the array
        * @param aIndex specifies the array index
        */
        IMPORT_C void DeleteAccessPointL( TInt aIndex );
         
        ///////////////////////////////////////////////////////////////////////
        // Receiving mode related methods
        ///////////////////////////////////////////////////////////////////////
        
        /**
        * ReceivingModeHome
        * @return enumerated value representing receiving mode in home network
        * Possible values:
        *   EMmsReceivingAutomatic (default),
        *   EMmsReceivingManual,
        *   EMmsReceivingPostpone,
        *   EMmsReceivingReject
        */
        inline TMmsReceivingMode ReceivingModeHome() const;

        /**
        * SetReceivingModeHome sets the receiving mode in home network
        * @param aMode
        * Possible values as in ReceivingModeHome
        */
        inline void SetReceivingModeHome( TMmsReceivingMode aMode );

        /**
        * ReceivingModeForeign
        * @return enumerated value representing receiving mode in foreign network
        * Possible values as in ReceivingModeHome
        */
        inline TMmsReceivingMode ReceivingModeForeign() const;

        /**
        * SetReceivingModeForeign sets the receiving mode in foreign network
        * @param aMode
        * Possible values as in ReceivingModeHome
        */
        inline void SetReceivingModeForeign( TMmsReceivingMode aMode );

        ///////////////////////////////////////////////////////////////////////
        // Accepted message types settings
        // (default values are 'ETrue')
        ///////////////////////////////////////////////////////////////////////
        
        /**
        * Get accept personal messages flag.
        * @return ETrue if personal messages accepted
        */
        inline TBool AcceptPersonalMessages() const;

        /**
        * Set acceptance flag for personal messages.
        * @param aPersonalAllowed
        */
        inline void SetAcceptPersonalMessages( TBool aPersonalAllowed = ETrue );

        /**
        * Get accept advertisement messages flag.
        * @return ETrue if advertisements accepted
        */
        inline TBool AcceptAdvertisementMessages() const;

        /**
        * Set acceptance flag for advertisement messages.
        * @param aAdvertisementsAllowed
        */
        inline void SetAcceptAdvertisementMessages( TBool aAdvertisementsAllowed = ETrue );

        /**
        * Get accept informational messages flag.
        * @return ETrue if informational messages accepted
        */
        inline TBool AcceptInformationalMessages() const;

        /**
        * Set acceptance flag for informational messages
        * @param aInformationalAllowed
        */
        inline void SetAcceptInformationalMessages( TBool aInformationalAllowed = ETrue );

        /**
        * Get accept anonymous messages flag
        * (anonymous message == message without From: field).
        * @return ETrue if anonymous messages accepted.
        */
        inline TBool AcceptAnonymousMessages() const;

        /**
        * Set acceptance flag for anonymous messages
        * @param aAnonymousAllowed
        */
        inline void SetAcceptAnonymousMessages( TBool aAnonymousAllowed = ETrue );

        ///////////////////////////////////////////////////////////////////////
        // Rest of the settings
        ///////////////////////////////////////////////////////////////////////

        /**
        * ExpiryInterval refers to messages expiration interval in the network.
        * @return message expiration interval in seconds
        */
        inline TInt32 ExpiryInterval() const;

        /**
        * SetExpiryInterval
        * @param aExpiryInterval message expiration interval in seconds
        * Recommended values:
		* 0 ( = maximum allowed by MMSC )
		* 1 * 3600( 1 hour )
		* 6 * 3600( 6 hours )
		* 24 * 3600( 24 hours )
		* 72 * 3600( 3 days = 72 hours )
		* 168 * 3600 ( 1 week = 168 hours )
        * Default value 0
        */
        inline void SetExpiryInterval( TInt32 aExpiryInterval );
        
        /**
        * SenderVisibility
        * @return sender visibility code
        */
        inline TInt32 SenderVisibility() const;

        /**
        * Set sender visibility code
        * @param aSenderVisibilty sender visibility code
        * Possible values:
        *   EMmsSenderVisibilityDefault, (default)
        *   EMmsSenderVisibilityHide,
        *   EMmsSenderVisibilityShow 
        */
        inline void SetSenderVisibility( TMmsMessageSenderVisibility aSenderVisibility );

        /**
        * Get delivery report wanted code.
        * @return delivery report code
        */
        inline TInt32 DeliveryReportWanted() const;

        /**
        * Set delivery report wanted
        * @param aDeliveryReport delivery report code
        * Possible values:
        *   KMmsYes,
        *   KMmsNo (default)
        */
        inline void SetDeliveryReportWanted( TInt32 aDeliveryReportWanted );

        /**
        * Get Delivery report sending allowed
        * @return delivery report sending allowed code
        */
        inline TInt32 DeliveryReportSendingAllowed() const;

        /**
        * Set Delivery report sending allowed
        * @param aReportAllowed delivery report sending allowed code
        * Possible values:
        *   KMmsYes (default),
        *   KMmsNo
        */
        inline void SetDeliveryReportSendingAllowed( TInt32 aDeliveryReportSendingAllowed );

        /**
        * Get Read report wanted setting.
        * Specifies if the user wants a report for each message when the recipient
        *     opens the message the first time
        * @return read report wanted value. Possible values:
        *   KMmsYes
        *   KMmsNo (default)
        */
        inline TInt32 ReadReplyReportWanted() const;

        /**
        * Set Read report wanted setting.
        * Specifies if the user wants a report for each message when the recipient
        *     opens the message the first time
        * @param aReadReplyReportWanted read report wanted code
        * Possible values:
        *   KMmsYes,
        *   KMmsNo
        */
        inline void SetReadReplyReportWanted( TInt32 aReadReplyReportWanted );

        /**
        * Get read report sending allowed value.
        * Specifies if the terminal is allowed to send a read report when the message
        *     is opened the first time and the sender has requested a read report
        * @return read reply report allowed value. Possible values:
        *   ETrue
        *   EFalse (default)
        */
        inline TBool ReadReplyReportSendingAllowed() const;

        /**
        * Set read report sending allowed code.
        * Specifies if the terminal is allowed to send a read report when the message
        *     is opened the first time and the sender has requested a read report
        * @param aReadReplyReportSendingAllowed read report sending allowed value
        *     (ETrue or EFalse).
        */
        inline void SetReadReplyReportSendingAllowed( TBool aReadReplyReportSendingAllowed );

        /**
        * Get message priority.
        * @return message priority code
        */
        inline TInt32 MessagePriority() const;

        /**
        * Set message priority
        * @param aPriority message priority code
        * Possible values:
        *   EMmsPriorityLow,
        *   EMmsPriorityNormal (default),
        *   EMmsPriorityHigh
        */
        inline void SetMessagePriority( TMmsMessagePriority aMessagePriority );

        /**
        * Get default message class.
        * @return message class code
        */
        inline TInt32 MessageClass() const;

        /**
        * Set default message class.
        * @param aMessageClass message class code
        * Possible values:
        *   EMmsClassPersonal (default),
        *   EMmsClassAdvertisement,
        *   EMmsClassInformational
        */
        inline void SetMessageClass( TMmsMessageClass aMessageClass );

        /**
        * Get maximum size of messages that can be sent in bytes.
        * @return maximum message size (0 = any size)
        */
        inline TInt32 MaximumSendSize() const;
        
        /**
        * Get maximum size of messages that can be received in bytes.
        * @return maximum message size (0 = any size)
        */
        inline TInt32 MaximumReceiveSize() const;

        /**
        * Get image width.
        * @return image width in pixels
        */
        inline TInt32 ImageWidth() const;

        /**
        * Set image width.
        * @param aImageWidth image width in pixels
        */
        inline void SetImageWidth( TInt32 aImageWidth );

        /**
        * Get image height.
        * @return image height in pixels
        */
        inline TInt32 ImageHeight() const;

        /**
        * Set image height.
        * @param aImageHeight image height in pixels
        */
        inline void SetImageHeight( TInt32 aImageHeight );

        /**
        * MoveToSent returns a boolean specifying whether messages are moved
        * to sent folder after they have been successfully sent.
        * If not, the message will be deleted.
        * This setting is common to all messaging types.
        * @return ETrue, if messages are moved, EFalse, if they are deleted.
        */
        inline TBool MoveToSent() const;

        /**
        * FetchOverride specifies whether message should be retrieved regardless of
        * what has been set to receiving mode value. This setting is not saved in
        * Central Repository, but instead it is handled "on the fly".
        * @return ETrue if messages are to be fetched even if receivingmode
        *   is off or deferred.
        * Default value EFalse.
        */
        inline TBool FetchOverride() const;

        /**
        * SetFetchOverride
        * @param aOverride:
        *   ETrue: if messages are to be fetched in spite of receivingmode
        *   being off or deferred.
        *   EFalse: if receivingmode states are valid
        */
        inline void SetFetchOverride( TBool aOverride );

        /**
        * Get logging state of messages to Email recipients
        * @return
        *   ETrue: if sent messages to email recipients are logged
        *   EFalse: if messages only to phone numbers are logged
        */
        inline TBool LogEmailRecipients() const;

        /**
        * Set logging state of messages to Email recipients
        * @param aEmailLogging:
        *   ETrue: if sent messages to email recipients are logged
        *   EFalse: if messages only to phone numbers are logged
        */
        inline void SetLogEmailRecipients( TBool aEmailLogging );

        /**
        * Get the value (in seconds) that the expiry interval can be exceeded
        * and message fetching will still be attempted.
        * @return expiry overshoot value in seconds
        */
        inline TInt ExpiryOvershoot() const;

        /**
        * Get MMS creation mode
        * @return MMS creation mode
        */
        inline TInt32 CreationMode() const;

        /**
        * Set MMS Creation Mode
        * @param aCreationMode
        * Possible values:
        *   EMmsCreationModeRestricted, 
        *   EMmsCreationModeWarning (default),
        *   EMmsCreationModeFree
        */
        inline void SetCreationMode( TMmsCreationMode aCreationMode );
        
        //
        // Following localMode related methods are for testing purposes only
        //
        
        /**
        * Get MMS local mode
        * (Localmode is for test purposes only)
        * @return MMS creation mode
        */
        inline TBool LocalMode() const;

        /**
        * Set MMS local mode
        * (Localmode is for test purposes only)
        * @param aLocalMode specifying whether normal or local mode is in use.
        */
        inline void SetLocalMode( TBool aLocalMode );
        
        /**
        * LocalModeIn returns the directory path for incoming localmode messages
        */
        inline const TDesC& LocalModeIn();

        /**
        * SetLocalModeIn sets the directory path for incoming localmode messages
        */
        inline void SetLocalModeIn( const TDesC& aPath );

        /**
        * LocalModeOut returns the directory path for outgoing localmode messages
        */
        inline const TDesC& LocalModeOut();

        /**
        * SetLocalModeOut sets the directory path for incoming localmode messages
        */
        inline void SetLocalModeOut( const TDesC& aPath );
        
        /**
        * ShowSentTime returns the value that tells if sending time or
        *     receiving time should be shown for incoming messages
        *     This value is stored in iDate member in TMsvEntry
        *     ETrue = show sending time
        *     EFalse = show time the message arrived at the terminal (default)
        */
        inline TBool ShowSentTime() const;
        
        /**
        * Get the current MMS encapsulation versio
        * @since 3.2
        * @return MMS encapsulation version as hex.
        *     Value in low byte, low half is minor version, high half is major version
        *     0x10 = 1.0
        *     0x11 = 1.1
        *     0x12 = 1.2 etc.
        */
        inline TInt16 MmsVersion() const;
        
    public: // Functions from MMsvSessionObserver       

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * By default EPOC constructor is private.
        * @return void
        */
        void ConstructL();

        /**
        * C++ default constructor.
        */
        CMmsSettings();
        
        /**
        * Handles saving of one integer value to CenRep.
        * First, settings tried to be set normally, but if it turns out that
        * such key does not exist, it will be created.
        * The method assumes that connection to CenRep has been already created.
        * @param aKey refers to CenRep key
        * @param aValue contains the value to be saved
        */
        void SaveSetting( TUint32 aKey, TInt aValue );
        
        /**
        * Handles saving of AP array
        * The method assumes that connection to CenRep has been already created.
        */
        void SaveAccesspointArrayL();

        /**
        * Utility to connect to a particular CenRep
        * @param repositoryId
        */
        CRepository* ConnectCenRepL( const TUid aRepositoryId );

        #ifdef __WINS__
        // (FOR TESTING PURPOSES ONLY)
        void ReadLocalModeDirectoriesL();        
        #endif // __WINS__

        #ifdef _DEBUG
        // (FOR TESTING PURPOSES ONLY)
        void DebugLogSettings();
        #endif // _DEBUG

    public:     // Data
    protected:  // Data
    private:    // Data
        // Specifies directories related to localmode
        // (FOR TESTING PURPOSES ONLY)
#ifdef __WINS__
        TFileName iLocalModeInDir;  // Emulator might have separate localmode directories
        TFileName iLocalModeOutDir; // for incoming and outgoing messages
#else
//        TFileName iLocalModeDir; // HW version has only one localmode dir
        HBufC* iLocalModeDir; // only use needed amount of space
#endif

        CRepository* iMmsRepository; // contains mms settings

        TInt32  iSettingsVersion;       // Version of settings
        TMsvId  iDefaultServiceId;      // entry id to MMS Service entry
        TMsvId  iNotificationFolderId;  // entry id to Notification folder
        TMsvId  iMMBoxFolderId;         // entry id to MMBox folder
        TMsvId  iApplicationFolderId;   // entry id to Application folder
        RArray<TUint32> iAccesspointArray; // Accesspoint array (array is in preference order)
        TInt32  iReceivingModeHome;     // receiving mode in home network
        TInt32  iReceivingModeForeign;  // receiving mode in foreign network
        TBool   iAcceptPersonal;        // whether to accept personal messages
        TBool   iAcceptAdvertisement;   // whether to accept advertisement messages
        TBool   iAcceptInformational;   // whether to accept informational messages
        TBool   iAcceptAnonymous;       // whether to accept anonymous messages
        TInt32  iDeliveryReportWanted;  // Yes/No
        TInt32  iDeliveryReportAllowed; // Yes/No
        TInt32  iReadReplyReportWanted; // Yes/No
        TBool   iReadReplyReportAllowed;// ETrue/EFalse (this is not sent in any PDU..)
        TInt32  iMessageClass;          // Personal/Advertisement/Informational
        TInt32  iExpiryInterval;        // Default expiry interval for messages
        TInt32  iSenderVisibility;      // Default/Show/Hide
        TInt32  iMessagePriority;       // Low/Normal/High
        TInt32  iMaxSendSize;           // default 0 = any size (value is bytes)
        TInt32  iMaxReceiveSize;        // default 0 = any size (value is bytes)
        TInt32  iImageWidth;            // 
        TInt32  iImageHeight;           // 
        TBool   iMoveToSent;            // global setting for all message types
        TBool   iLogEmailRecipients;    // log e-mail recipients or not
        TInt    iExpiryOvershoot;       // interval in seconds, we try to fetch
                                        // even if expiry time has been passed
                                        // by this much time
        TInt32  iCreationMode;          // Restricted/Warning/Free
        TBool   iFetchOverride;         // ETrue: fetch message even if fetching
                                        // is off or deferred
        TBool   iLocalMode;             // Local/Normal mode
        TBool   iShowSentTime;          // Show sending/receiving time for received messages
                                        // ETrue: show time the message was sent
                                        // EFalse: show time the message was received
        TInt16  iMmsVersion;            // MMS encapsulation version                                        
    };

#include "mmssettings.inl"

#endif // __MMSSERVERSETTINGS_H   
            
// End of File
