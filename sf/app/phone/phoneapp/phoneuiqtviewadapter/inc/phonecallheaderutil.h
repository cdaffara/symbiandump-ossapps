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
* Description:  A parameter class for setting call header information
*
*/


#ifndef PHONECALLHEADERUTIL_H
#define PHONECALLHEADERUTIL_H

//INCLUDES
#include    "mpeengineinfo.h"

//FORWARD
class MPEEngineInfo;

/**
 * A parameter class for setting call header information.
 *
 *  @since S60 v10.1
 */
class PhoneCallHeaderUtil
    {
    public: // Constructors & destructors.
        
        enum ClippingDirection
            {
            ERight = 0,
            ELeft
            };
    
        /**
        * C++ constructor.
        */
        PhoneCallHeaderUtil( MPEEngineInfo& engineInfo );
        
        virtual ~PhoneCallHeaderUtil();
        
    public: // New functions.
        
        /**
        * Returns the call header label text
        * @return Returns the label
        */
        const TDesC& LabelText( TInt aCallId );

        /**
        * Gets the call cli texts.
        */
        void GetCliTexts( 
                TInt aCallId,
                TDes& aCliText,
                ClippingDirection &aCliClip,
                TDes& aSecondaryCliText,
                ClippingDirection &aSecondaryCliClip );
        
        /**
        * Gets the cli text.
        * @return Returns cli text.
        */
        void GetCli( TInt aCallId, TDes& aCliText, 
                ClippingDirection &aClipping );

        /**
        * Gets the secondary cli text.
        */
        void GetSecondaryCli( TInt aCallId, 
                TDes& aSecondaryCliText,
                ClippingDirection &aClipping );
        
        /**
         * Remote phone number.
         * @return Phone number for remote end.
         */
        const TDesC& RemotePhoneNumber( TInt aCallId ) const;
        
        /**
        * Returns the call type.
        * @return Returns call type.
        */
        TInt CallType( const TInt aCallId );
        
        /**
        * Returns service id.
        * @return Returns service id.
        */
        TInt ServiceId( const TInt aCallId );
        
        /**
        * Returns flag is the call forwarded.
        * @return Returns is forwarded.
        */
        TBool IsCallForwarded( TInt aCallId );
        
        /**
        * Returns secure specified status.
        * @return Returns secure specified status.
        */
        TBool SecureSpecified();
        
        /**
        * Returns ciphering status.
        * @return Returns ciphering status.
        */
        TBool Ciphering( TInt aCallId );
        
        /**
        * Returns the call state.
        * @return Returns the call state
        */
        TInt CallState( TInt aCallId ) const;
        
        /**
        * Returns the caller image.
        * @return Returns the caller image.
        */  
        const TDesC& CallerImage( const TInt aCallId );
       
        /**
        * Returns emergency call header text.
        * @return Returns emergency call header text.
        */ 
        const TDesC& EmergencyHeaderText();
        
        /**
        * Returns emergency call label text.
        * @return Returns emergency call label text.
        */ 
        const TDesC& AttemptingEmergencyText();
        
        /**
        * Check if the call is video call.
        * @param aCallid call id
        */
        TBool IsVoiceCall(TInt aCallId) const;
                
        /**
        * Check if the call is video call.
        * @param aCallid call id
        */
        TBool IsVideoCall(int aCallId ) const;
        
    private:
        
        /**
        * Sets label text for incoming call.
        * @param aCallId - Call Id.
        */
        void SetCallHeaderLabelTextForRingingCall( TInt aCallId );
        
        /**
        * Check if remote name info is available
        * (RemoteName, RemotePartyName or RemoteCompanyName),
        * if available ETrue is returned otherwise EFalse.
        * @param aCallid call id
        */
        TBool RemoteNameAvailable( const TInt aCallId ) const;
        
        /**
        * Gets remote info data
        * @param aCallid call id
        * @param aData the returned remote info data
        */
        void GetRemoteInfoData( 
                const TInt aCallId,
                TDes& aData ) const;        
    
        /**
        * Checks is the secondary cli allowed for the call.
        * @param aCallid call id
        * @return ETrue if allowed to show the secondary cli. 
        */
        TBool IsSecondaryCliAllowed( TInt aCallId );
        
        /**
        * Check if the call is waiting call.
        * @param aCallid call id
        */
        TBool IsWaitingCall(int callId) const;
        
        /**
        * Loads resource text.
        * @return Returns loaded text.
        */
        HBufC* LoadResource( const TInt aResource ) const;
                
        /**
        * Loads recource text.
        */
        void LoadResource( TDes& aData, const TInt aResource ) const;
        
    private:
        
        MPEEngineInfo& m_engineInfo;
        HBufC *iLabelText;
        HBufC *iCliText;
        HBufC *iSecondaryCliText;
        HBufC *iCallerImage;
        HBufC *iEmergencyHeaderText;
        HBufC *iAttemptingEmergencyText;
            
    };

#endif // PHONECALLHEADERUTIL_H
