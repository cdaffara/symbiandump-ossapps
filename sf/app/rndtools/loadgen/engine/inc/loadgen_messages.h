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
* Description: 
*
*/


#ifndef LOADGEN_MESSAGES_H
#define LOADGEN_MESSAGES_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "loadgen_loadbase.h"
#include "loadgen_loadattributes.h"


// FORWARD DECLARATIONS
class CSmsHandler;
class CMmsHandler;

const TInt EMaxMessageLength = 512;

// CLASS DECLARATIONS

class CMessages : public CLoadBase
    {
public:
    static CMessages* NewL( TMessageAttributes& aAttributes, TInt aReferenceNumber );
    virtual ~CMessages();
        
private: // Constructors
    CMessages( TMessageAttributes& aAttributes, TInt aReferenceNumber );
    void ConstructL();  

public:  // New methods
    virtual void Resume();
    virtual void Suspend();
    virtual void SetPriority();
    virtual void Close();
    virtual TPtrC Description();
    inline TMessageAttributes& Attributes() { return iAttributes; }
	inline void SetAttributes(const TMessageAttributes& aAttributes) { iAttributes = aAttributes; }

public:  // New static methods
    static TInt ThreadFunction( TAny* aThreadArg );

private:  // New static methods
    static void GenerateLoad( TMessageAttributes& aAttributes );

private: // Data
    TMessageAttributes            iAttributes;    
    RThread                     iThread;

    };


class CMessageManager : public CActive
    { 
private:
    enum TState
        {
        EStateIdle = 0,
        EStateSend,
        EStateSending        
        };
public:
    static CMessageManager* NewL( TMessageAttributes& aAttributes );
    virtual ~CMessageManager();


private:
    CMessageManager( TMessageAttributes& aAttributes );
    void ConstructL();

private:
    void RunL();
    void DoCancel();

private:
    static TInt PeriodicTimerCallBack( TAny* aAny );
    void HandleMessageSending();
    void DoDial();
    void DoHangup();
    void CreateMessage();    

public:
    inline CPeriodic* PeriodicTimer() { return iPeriodicTimer; }
    void HandleStatus( TInt aErr );

private:
    TMessageAttributes&                   iAttributes;
    CPeriodic*                          iPeriodicTimer;
    TInt                                iState;
    CSmsHandler*                        iSmsHandler;
    CMmsHandler*                        iMmsHandler;
    TInt                                iMessageCounter;
    HBufC*                                iMessage;
    }; 

//  INCLUDES
#include <e32base.h>
#include <msvapi.h>
#include <mtuireg.h>
#include <txtrich.h>
// CONSTANTS
const TInt KBfrLength = 20;
const TInt KTelephoneNumberMaxLength = 100;
 
// FORWARD DECLARATIONS
class CSmsAppUi;
class CClientMtmRegistry;
class CSmsClientMtm;
 
// CLASS DECLARATION
/**
* CSmsHandler application engine class.
* Takes care of sending and receiveing SMS messages using the SMS client MTM.
* Interacts with the application UI class.
*/
class CSmsHandler : public CActive, public MMsvSessionObserver
    {
    public: // Constructors and destructor
 
        /**
        * NewL.
        * Two-phased constructor.
        * @param aSmsAppUi Pointer to AppUi instance.
        * @return Pointer to the created instance of CSmsHandler.
        */
        static CSmsHandler* NewL( CMessageManager& aManager );
 
        /**
        * NewLC.
        * Two-phased constructor.
        * @param aSmsAppUi Pointer to AppUi instance.
        * @return Pointer to the created instance of CSmsHandler.
        */
        static CSmsHandler* NewLC( CMessageManager& aManager );
 
        /**
        * ~CSmsHandler
        * Destructor.
        */
        virtual ~CSmsHandler();
 
    public: // New functions
        /**
        * SendL.
        * Starts the process of creating and sending an SMS message.
        * @param aRecipientNumber The number of the recipent.
        * @param aMessageText The message text.
        * @return ETrue if successful, EFalse if not.
        */
       TBool SendL( const TDesC& aRecipientNumber,
                    const TDesC& aMessageText );
 
    public: // Functions from base classes
 
        /**
        * From MMsvSessionObserver, HandleSessionEventL.
        * Handles notifications of events from the Message Server.
        * @param aEvent The event that has taken place
        * @param aArg1 Event type-specific argument value
        * @param aArg2 Event type-specific argument value
        * @param aArg3 Event type-specific argument value
        */
        void HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1,
                                  TAny* aArg2, TAny* aArg3 );
 
    protected: // Functions from base classes
 
        /**
        * From CActive, DoCancel.
        * Cancels any outstanding requests.
        */
        void DoCancel();
 
        /**
        * From CActive, RunL.
        * Handles an active object’s request completion event.
        */
        void RunL();
 
    private: // Constructors
 
        /**
        * CSmsHandler.
        * C++ default constructor.
        * @param aSmsAppUi Pointer to AppUi instance.
        */
        CSmsHandler( CMessageManager& aManager );
 
        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();
 
    private: // New functions
 
        /**
        * AccessMtmL.
        * Access the MTM Registry and create an SMS specific Client MTM instance.
        */
        void AccessMtmL();
 
        /**
        * CreateMsgL.
        * Create an SMS message.
        * @return ETrue if successful, EFalse is unsuccessful.
        */
        TBool CreateMsgL();
 
        /**
        * ScheduleL.
        * Schedule an SMS message for sending.
        */
        void ScheduleL();
 
        /**
        * ValidateL.
        * Validate an SMS message.
        * @return ETrue if successful, EFalse is unsuccessful.
        */
        TBool ValidateL();
 
    private: // Enumeration
 
        /**
        * TState, enumeration for the state of the handler, used by RunL().
        */
        enum TState
            {
            EWaitingForMoving = 1,
            EWaitingForScheduling
            };
 
    private: // Data
 
        /**
        * iState, the state of the handler.
        */
        TState iState;
 
        /**
        * iSession, the contact database.
        * Owned by CSmsHandler object.
        */
        CMsvSession* iSession;
 
        /**
        * iMtmRegistry, client MTM registry.
        * Owned by CSmsHandler object.
        */
        CClientMtmRegistry* iMtmRegistry;
 
        /**
        * iSmsMtm, SMS specific Client MTM.
        * Owned by CSmsHandler object.
        */
        CSmsClientMtm* iSmsMtm;
 
        /**
        * iOperation, the current message server operation.
        * Owned by CSmsHandler object.
        */
        CMsvOperation* iOperation;
 
        /**
        * iRecipientNumber, telephone number of the recipient.
        */
        TBuf<KTelephoneNumberMaxLength> iRecipientNumber;
 
        /**
        * iMessageText, SMS message text.
        */
        TBuf<EMaxMessageLength> iMessageText;

 
        /**
        * iSmsAppUi, application UI
        * Not owned by CSmsHandler object.
        */
        CSmsAppUi* iSmsAppUi;
 
        /**
        * iMtmUiRegistry, User Interface MTM Registry.
        * Owned by CSmsHandler object.
        */
        CMtmUiRegistry* iMtmUiRegistry;
        
        CMessageManager& iManager;
 
    };


// INCLUDES
#include <msvapi.h>                         // for MMsvSessionObserver 
#include <mmsclient.h>                      // for CMmsClientMtm
#include <coecntrl.h>

// Forward declarations
class CClientMtmRegistry;
class CMsvSession;

//
// Container class to draw text on screen
//
class CMmsHandler : public CBase, public MMsvSessionObserver
    {
public: // Constructors and destructor

    /**
    * NewL.
    * Two-phased constructor.
    * @param aSmsAppUi Pointer to AppUi instance.
    * @return Pointer to the created instance of CSmsHandler.
    */
    static CMmsHandler* NewL( CMessageManager& aManager );

    /**
    * NewLC.
    * Two-phased constructor.
    * @param aSmsAppUi Pointer to AppUi instance.
    * @return Pointer to the created instance of CSmsHandler.
    */
    static CMmsHandler* NewLC( CMessageManager& aManager );

    /**
    * ~CSmsHandler
    * Destructor.
    */
    virtual ~CMmsHandler();

private: // Constructors

    /**
    * CSmsHandler.
    * C++ default constructor.
    * @param aSmsAppUi Pointer to AppUi instance.
    */
    CMmsHandler( CMessageManager& aManager );

    /**
    * ConstructL.
    * 2nd phase constructor.
    */
    void ConstructL();

public: // New functions
    
    /*
     * Creates client MTM registry when session is ready for use. 
     * This completes model construction and is called after 'server
     * ready' event is received after async opening of CMsvSession.
     */ 
    void CompleteConstructL();
       
    /*
     * CMmsHandler::CreateNewMessageL()
     * Creates a new message server entry and set up default values.
     * In case the attachment file does not found method return EFalse
     * otherwise ETrue.
     * There are differenses how to add attachment file between 2nd and 3rd edition.
     */  
     TBool CreateMsgL();

    /**
     * Send the message.
     * Return values: ETrue or EFalse
     */
    TBool SendMessageL();
        
     /**
      * SendL.
      * Starts the process of creating and sending an SMS message.
      * @param aRecipientNumber The number of the recipent.
      * @param aMessageText The message text.
      * @return ETrue if successful, EFalse if not.
      */
     TBool SendL( const TDesC& aRecipientNumber,
                 const TDesC& aMessageText );     
           
private:    
    // from MMsvSessionObserver
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
  
    void SetMessageBodyL();

private:

    CMsvSession* iSession;          // Client session on the message server
    CMmsClientMtm* iMmsMtm;         // Message Type Module (MMS)
    CClientMtmRegistry* iMtmReg;    // Mtm client registry for creating new mtms

    /**
    * iRecipientNumber, telephone number of the recipient.
    */
    TBuf<KTelephoneNumberMaxLength> iRecipientNumber;

    /**
    * iMessageText, SMS message text.
    */
    TBuf<EMaxMessageLength> iMessageText;
    
    CMessageManager& iManager;
    
    };
#endif // LOADGEN_MESSAGES_H
