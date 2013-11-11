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
* Description:  HTI NCN notification creating handler.
*
*/


#ifndef CHTINOTIFICATIONHANDLER_H
#define CHTINOTIFICATIONHANDLER_H


// INCLUDES
#include <HtiServicePluginInterface.h>
#include <etelmm.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
#include <AknNotifyStd.h>       //TAknSoftNotificationType
#include <AknSoftNotifier.h>    //CAknSoftNotifier
#include <AknSoftNotificationParameters.h>  //CAknSoftNotificationParameters

/**
*  HTI NCN notification creating handler.
*/
class CHtiNotificationHandler : public CActive
    {
    public:

        /**
        * Two-phased constructor.
        */
        static CHtiNotificationHandler* NewL();

        /**
        * Called when there is a message to be processed by this service.
        * @param aMessage message body destinated to the servive
        * @param aPriority message priority
        */
        void ProcessMessageL( const TDesC8& aMessage,
                              THtiMessagePriority aPriority );

        /**
        * Destructor.
        */
        virtual ~CHtiNotificationHandler();

        /**
        * Sets the dispatcher to send outgoing messages to.
        * @param aDispatcher pointer to dispatcher instance
        */
        void SetDispatcher( MHtiDispatcher* aDispatcher );
    
    protected:
        /*! from CActive */
        void RunL();
        /* from CActive */
        void DoCancel();
        /*! from CActive */
        TInt RunError(TInt aError);

    private:

        /**
        * C++ default constructor.
        */
        CHtiNotificationHandler();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private:
        /**
        * Indicator support values.
        */
    enum TNcnIndicator
        {
        ENcnIndicatorFax,
        ENcnIndicatorEmail,
        ENcnIndicatorOther,
        ENcnIndicatorVMLine1,
        ENcnIndicatorVMLine2,
        
        ENcnIndicatorLast // This must be last            
        }; // TNcnIndicator 
    
    enum TNcnCommand
        {
        ENcnSetIndicator,
        ENcnResetIndicator
        }; //TNcnCommand
    
    struct TNcnReqQueueElement
        {
        TNcnReqQueueElement(TNcnCommand aCmd, TNcnIndicator aIndicator, TUint aMsgCount, TBool aEnable)
            {
            iCmd = aCmd;
            iIndicator = aIndicator;
            iMsgCount = aMsgCount;
            iEnable = aEnable;
            }
        TNcnCommand iCmd;
        TNcnIndicator iIndicator;
        TUint iMsgCount; 
        TBool iEnable;
        };
    
    private: // helpers

        void HandleCreateVoiceMessageNotificationL( const TDesC8& aData );
        void HandleClearAllNotificationsL( const TDesC8& aData );
        
        CAknSoftNotificationParameters* CreateNotificationParametersLC( TBool aAmountKnown );
        void ShowNewItemsL( CAknSoftNotifier* aNotifier, 
                const TAknSoftNotificationType aNotificationType, const TInt aAmount );
        void SetNotificationL(TUint aAmount);
        void SetIndicator(const TNcnIndicator aIndicator, TUint aMsgCount, TBool aEnable);
        void ResetIndicator();

        void SendOkMsgL( const TDesC8& aData );
        void SendErrorMessageL( TInt aError, const TDesC8& aDescription );

    private:  // Data
        // Needed to initialize mobilePhone
        RTelServer iTelServer;
        
        // Needed to initialize mobilePhone
        RTelServer::TPhoneInfo iPhoneInfo;
            
        // Used to store indicator flags and message counts. 
        RMobilePhone iMobilePhone;
        
        // Struct is a container for indicator flags and message counts
        RMobilePhone::TMobilePhoneMessageWaitingV1      iMsgWaiting;
        
        // Package for iMsgWaitingV1                      
        RMobilePhone::TMobilePhoneMessageWaitingV1Pckg  iMsgWaitingPckg;
        
        // Pointer to the dispatcher (referenced)
        MHtiDispatcher* iDispatcher; // referenced
        
        TUint iVoiceMailsInLine1;
        
        RArray<TNcnReqQueueElement> iReqQueue;          
    };


#endif // CHTINOTIFICATIONHANDLER_H
