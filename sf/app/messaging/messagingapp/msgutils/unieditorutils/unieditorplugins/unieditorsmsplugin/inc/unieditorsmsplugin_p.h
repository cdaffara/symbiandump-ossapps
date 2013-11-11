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

#ifndef __UNIEDITORSMSPLUGINPRIVATE_H
#define __UNIEDITORSMSPLUGINPRIVATE_H

// INCLUDES
#include <msvapi.h>
#include <gsmuelem.h>
#include <mtclbase.h>
#include "unieditorplugininterface.h"
#include "UniSendingSettings.h"

// DATA TYPES

// FORWARD DECLARATIONS
class CClientMtmRegistry;
class CSmsClientMtm;
class CSmsHeader;
class CParaFormatLayer;
class CCharFormatLayer;
class CRichText;
class CMsgSmsEmailOverSmsFunc;
class CSmsNumber;
class CSmsMessage;
class ConvergedMessage;
class UniEditorGenUtils;

// CLASS DECLARATION

/**
* UniEditorSmsPluginPrivate
*/ 
class UniEditorSmsPluginPrivate :public CBase, public MMsvSessionObserver
	{
	public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static UniEditorSmsPluginPrivate* NewL();
 		
        /**
        * Destructor.
        */
        virtual ~UniEditorSmsPluginPrivate();

    public: // From CUniEditorPlugin

        /**
         * load sms headers either from store, if provided,
         * or from global settings
         * @param aStore message store
         */
        void LoadHeadersL( CMsvStore* aStore );

        /**
         * Converts message from message store into ConvergedMessage 
         * based on the operation
         * @param TMsvId id
         * @param aOperation operation type
         * @return ConvergedMessage object is returned in successfull cases and 
         *         NULL is returned in failure cases. The ownership of the object
         *         is transferred to the caller. 
         */
        ConvergedMessage* ConvertFromL( TMsvId aId,
            UniEditorPluginInterface::EditorOperation aOperation );

        /**
         * convert convergedmessage type to a message entry in store
         * @param message convergedmessage
         * @return message entry id
         */
        TMsvId ConvertToL( ConvergedMessage* message );

        /**
         * send a message
         * @aId message entry id
         */
        void SendL( TMsvId aId );

        /**
         * validate settings
         * @param aEmailOverSms if email over sms is supported or not 
         */
        TBool ValidateServiceL( TBool aEmailOverSms = EFalse );

        /**
         * validate the service
         */
        TBool IsServiceValidL();
        
        /**
         * set unicode mode to true/false
         */
        inline void SetUnicodeMode( TBool aUnicodeMode );
        
        //Turkish SMS(PREQ2265) specific...
        void SetEncodingSettingsL(TBool aUnicodeMode, TSmsEncoding aAlternativeEncodingType, TInt charSupportType);
       
        /**
         * for deciding on reduced or full charset support
         */
        void GetNumPDUsL (
               TDesC& aBuf,
               TInt& aNumOfRemainingChars,
               TInt& aNumOfPDUs,
               TBool& aUnicodeMode, 
               TSmsEncoding& aAlternativeEncodingType);

        /**
         * delete entry aId from drafts folder
         * @param aId entry to be deleted
         */
        void DeleteDraftsEntryL( TMsvId aId );

	private: // Constructors
	
	    /**
        * C++ default constructor.
        */
        UniEditorSmsPluginPrivate();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();
		
		/**
		* Returns pointer to iSmsMtm member variable
		*/
		CSmsClientMtm* SmsMtmL();

        /**
        * Moves message into outbox for sending and creates copies of SMS messages
        * into outbox in case of multiple recipients
        */
        void MoveMessagesToOutboxL();

        /**
        * Called in MoveMessagesToOutboxL, calls the actual moving
        * function
        */
        TMsvId MoveMessageEntryL( TMsvId aTarget );

        
        /**
        * Creates messages into the Outbox if there are several recipients
        * @param TMsvEntry& a reference to entry to be copied
        * @param CSmsNumber& a reference to CSmsNumber to be copied
        * @param CRichText& a reference to msg body contents
        * @return TMsvId of the created msg entry
        */
        TMsvId CreateMessageInOutboxL(
            const TMsvEntry& aEntry, 
            const CSmsNumber& aRecipient, 
            const CRichText& aBody );
            
        /**
        * Creates messages into the Outbox if there are several recipients
        * and the message is E-Mail over SMS message
        * @param aEntry a reference to entry to be copied
        * @param aAddress a reference to address to be copied
        * @return TMsvId of the created msg entry
        */
        virtual TMsvId CreateMessageInOutboxL(
            const TMsvEntry& aEntry, 
            const TDesC& aAddress );    
        

        /**
        * Calls the actual sending method of SMS Client MTM
        */
        void SetScheduledSendingStateL( CMsvEntrySelection* aSelection );
        
        /**
        * Extracts name and address
        */
        void NameAndAddress( const TDesC& aMsvAddress, TPtrC& aName, TPtrC& aAddress );

        /**
        * Creates text for TMsvEntry::iDescription. It could be subject or
        * if subject does not exist a beginning of the message body 
        */
        void ExtractDescriptionFromMessageL(
            const CSmsMessage& aMessage, 
            TDes& aDescription, 
            TInt aMaxLength);
            
        /**
        * Copies text from attachment into the body of the SMS
        */
        void CreatePlainTextSMSL( RFile& aFile );
        
        /** 
        * Insert subject into the body of the SMS
        */
        void InsertSubjectL( CSmsHeader& aHeader, CRichText& aText );
        
        /**
        * Copies data from VCard attachment into body of the SMS
        */
        void CreateVCardSMSL( RFile& aFile );
        
        /**
        * Copies data from VCCalendar attachment into body of the SMS
        */
        void CreateVCalSMSL( RFile& aFile );
                
        /**
        * Checks service center address 
        */
        TBool ValidateSCNumberL();
        
        /**
        * Checks service center address for E-Mail over SMS messages
        */
        TBool ValidateSCNumberForEmailOverSmsL();
        
        /**
        * Saves E-Mail specific information in header
        */
        void FillEmailInformationDataL( CSmsHeader& aHeader,  const TPtrC& aAddress );
    
        /**
        * Checks if the address is E-Mail address
        */
        TBool IsEmailAddress( const TPtrC& aAddress ) const;
            
        /**
         * Converts sms message to uni message format.
         * @param aId message id
         * @param [OUT]aMessage convergedmessage
         * @param aOperation operation type
         */
        void DoConvertFromL( TMsvId aId, 
            ConvergedMessage* aMessage,
            UniEditorPluginInterface::EditorOperation aOperation );

        /**
         * create Sms entry into draft folder
         */
        TMsvId CreateNativeSmsL();
     
        /**
         * set/update sms settings 
         */
        void SetSmsSettingsL(ConvergedMessage* message);
     
        /**
         * set data to the sms
         */
        void SetSmsDataL(ConvergedMessage* message);
     
        /**
         * set recipient / subject to the message entry 
         */
        void SetSmsHeaderL(ConvergedMessage* message);
        
        /**
         * set body text from converged message into
         * message entry in store
         */
        void SetSmsBodyL(ConvergedMessage* message);
        
        /**
         * set attachment info from converged message into
         * message entry in store
         */
        void SetSmsAttachmentsL(ConvergedMessage* message);

        /**
         * create attachment in message store for the 
         * message under composition
         * @param aFilePath file for which attachment is created
         * @param aStore message store
         */
        void CreateAttachmentL(CMsvStore* aStore, const TDesC& aFilePath);
        
        /**
         * Populates address feilds into converged message
         */
        void populateRecipientsL(ConvergedMessage* aMessage);
        
        /**
         * Populates body into converged message
         */
        void populateMessageBodyL(ConvergedMessage* aMessage,
            TMsvEntry &aEntry);
			
		/**
	     * @see MMsvSessionObserver
    	 */
	    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2,
                             TAny* aArg3);
	    
	    /**
	     * Populate recipients for reply case
	     */
	    void populateMessageForReplyL(ConvergedMessage* aMessage);
	    
	    /**
	     * Extract from address from Deliver PDU
	     */
	    void fromAddress(QString& messageAddress);

    private:  // Data    

        /**
         * message service session
         * Owned
         */
        CMsvSession* iSession;
        
        /**
         * Mtm registry
         * Owned
         */
        CClientMtmRegistry* iMtmRegistry;
        
        /**
         * sms mtm
         * Owned
         */
        CSmsClientMtm* iSmsMtm;
        
        /**
         * sms service id
         */
        TMsvId iSmsServiceId;
        
        /**
         * sms header
         * Owned
         */
        CSmsHeader* iSmsHeader;
        
        /**
         * Owned
         */
        CParaFormatLayer* iParaFormatLayer;
        
        /**
         * Owned
         */
        CCharFormatLayer* iCharFormatLayer;
        
        /**
         * Owned
         */
        CRichText* iRichText;
        
        /**
         * recipient array
         * Owned
         */
        CDesCArray* iRecipients;
        
        /**
         * flag for biomsg distinction
         */
       	TBool iBioMsg;
        
        enum TUniPluginPanic
            {
            EIllegalArguments  
            }; 
    
        /**
         * email gateway address/alias
         * Owned
         */
        CSmsNumber* iEmailOverSmsC;
        
        /**
         * flag for setting unicodemode true/false
         */
        TBool iUnicodeMode;
        
        /**
         * feature flag to check if offline is supported or not
         */
        TBool iOfflineSupported;
        
        //Turkish SMS(PREQ2265) specific...
        TInt iCharSupportType;
        TSmsEncoding  iAlternativeEncodingType;
        TBool iNLTFeatureSupport;
        
        /**
         * general utility class
         * Owned
         */
        UniEditorGenUtils* iGenUtils;
    };

inline void UniEditorSmsPluginPrivate::SetUnicodeMode( TBool aUnicodeMode )
    {
    iUnicodeMode = aUnicodeMode;
    }

#endif      // __UNIEDITORSMSPLUGINPRIVATE_H

// End of File
