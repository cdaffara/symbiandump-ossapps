/*
* Copyright (c) 2002, 2003 Nokia Corporation and/or its subsidiary(-ies).
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
*     Wrapper for MMS specials in TMsvEntry.
*     There is no single MMS specific flag defined,
*     the methods are rather for ease of usage.
*     There is a lot of free flags available.
*
*/



#ifndef TMMSMSVENTRY_H
#define TMMSMSVENTRY_H

//  INCLUDES
#include <msvstd.h>
#include "mmsconst.h"


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Wrapper for MMS specials in TMsvEntry.
*/
class TMmsMsvEntry :public TMsvEntry
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TMmsMsvEntry();
        
        /**
        * Constructor. This causes memory copy to take place, so use rather type
        * casting when you do not need copy of the object.
        * Note that this does not copy the contents of iDetails and iDescriptor of
        * aGenericEntry..
        * The destructor of the original CMsvEntry entry deletes the
        * buffers pointed to by the TPtrC members.
        * @param aGenericEntry an entry in the Message Server index
        */
        IMPORT_C TMmsMsvEntry( const TMsvEntry& aGenericEntry );
       

    public: // New functions
        
        /**
        * Compare operation.
        */
        IMPORT_C TBool operator==( const TMsvEntry& aEntry ) const;

        /**
        * Compare operation
        */
        IMPORT_C TBool operator==( const TMmsMsvEntry& aEntry ) const;

        /**
        * Subject mutator
        * @param aSubject message subject
        */
        IMPORT_C void SetSubject( const TDesC& aSubject );

        /**
        * Subject accessor
        * @return message Subject
        */
        IMPORT_C const TDesC& Subject() const;

        /**
        * Attachment File name mutator
        * @param aFile attachment file name in message binary file storage
        */
        IMPORT_C void SetAttachmentFile( const TDesC& aFile );

        /**
        * Attachment File name accessor
        * @return attachment file name in message binary file storage
        */
        IMPORT_C const TDesC& AttachmentFile() const;

        /**
        * If the message entry is a mobile terminated message.
        * @return ETrue if the entry is a mobile terminated message.
        */
        inline TBool IsMobileTerminated() const;
        
        /**
        * Mutator for mobile terminated message flag.
        * @param aParam ETrue if the entry is a mobile terminated message.
        */
        inline void SetMobileTerminated( TBool aParam );

        /**
        * If the message entry is forwarded message.
        * @return ETrue if the entry is a forwarded message.
        */
        inline TBool IsForwardedMessage() const;
        
        /**
        * Mutator for forwarded message flag.
        * @aParam aParam ETrue if the entry is a mobile terminated message.
        */
        inline void SetForwardedMessage( TBool aParam );

        // The following flags are mainly for Server MTM only.

        /**
        * If the message entry is m-send-req.
        * @return ETrue if the entry is m-send-req.
        */
        inline TBool IsMSendReq() const;

        /**
        * If the message entry is m-notification-ind.
        * @return ETrue if the entry is m-notification-ind.
        */
        inline TBool IsMNotificationInd() const;

        /**
        * If the message entry is m-retrieve-conf.
        * @return ETrue if the entry is m-retrieve-conf.
        */
        inline TBool IsMRetrieveConf() const;

        /**
        * If the message entry is m-delivery-ind.
        * @return ETrue if the entry is m-notification-ind.
        */
        inline TBool IsMDeliveryInd() const;

        /**
        * Mutator for m-send-req flag.
        */
        inline void SetMSendReq( );

        /**
        * Mutator for m-notification-ind.
        */
        inline void SetMNotificationInd( );

        /**
        * Mutator for m-retrieve-conf.
        */
        inline void SetMRetrieveConf( );

        /**
        * Mutator for m-delivery-ind.
        */
        inline void SetMDeliveryInd( );

        /**
        * M-NotifyResp.ind is sent for this M-Notification.ind
        * @return ETrue if response is sent.
        */
        inline TBool MNotifyRespIndSent() const;

        /**
        * Mutator for M-NotifyResp.ind  Sent flag
        * @param aParam ETrue if M-NotifyResp.ind is sent.
        */
        inline void SetMNotifyRespIndSent( TBool aParam );

        /**
        * Editor Oriented flag accessor.
        */
        inline TBool EditorOriented() const;

        /**
        * Editor Orieneted flag mutator.
        * @param aParam flag value 
        */
        inline void SetEditorOriented (TBool aParam );

        
    public: // Functions from base classes
        
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:


    public:     // Data
    
    protected:  // Data

    private:    // Data


    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes


    };

#include "mmsmsventry.inl"

#endif      // TMMSMSVENTRY_H   
            
// End of File
