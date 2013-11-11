/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*    Phone logger
*
*/


#ifndef __CPHONELOGGER_H
#define __CPHONELOGGER_H

// INCLUDES
#include <e32std.h>
#include <coemain.h>
#include "phoneconstants.h"
#include "stringpool.h"

// FORWARD DECLARATIONS
class RFileLogger;
class MPEEngineInfo;
class MPEPhoneModel;

enum TPhoneUILoggingLevel
    {
    ENo,           // No logging at all.
    EOnlyFatal, // Only fatal errors etc. are logged
    EBasic,    // Normal logging (fatals + PE mesages etc),
    EMethodStartEnd,   // All above & function starts and ends are logged 
                       // (where added)
    EAll               // All above & extra logging
    };
    
enum TPhoneUILoggingComponent
    {
    EPhoneAbstractFactoryClient,
    EPhoneControl,
    EPhonePhoneapp,
    EPhoneUIView,
    EPhoneUIStates,
    EPhoneUIUtils,
    PhoneUIVoIPExtension,
    EPhoneMediatorCenter,
    EPhoneAmountOfLoggingComponents     
    };
    
// CLASS DECLARATION

/** 
    @private
    @class          CPhone
    @since          7.0
    A utility used in phone application for logging in debug builds.  Log 
    files are placed in the directory %EPOCROOT%/epoc32/wins/c/logs/phoneapp
 */
class CPhoneLogger : public TDesOverflow , public CCoeStatic
    {
    public: // New methods

        /**
        * C++ constructor
        * @param aUid Uid for CCoeStatic uid.
        */
        IMPORT_C CPhoneLogger(TUid aUid);

        /**
        * C++ destructor. Closes the log file.
        */
        IMPORT_C virtual ~CPhoneLogger();

        /**
        * Formats a log message by given numerous parameters and writes it in 
        * the log file.
        * @param aLevel     is the logging level. log messages below chosen 
        *                   level are not printed out.
        * @param aLoggingComponent identifies the component where the log 
        *                   message originates from. 
        * @param aFmt       is the log message, it may include formatting 
        * @return None.
        */
        IMPORT_C void LogIt(
            TPhoneUILoggingLevel aLevel, 
            TPhoneUILoggingComponent aLoggingComponent, 
            TRefByValue<const TDesC> aFmt, ...);

        /**
        * Checks if logging components level is enough to log.
        * @aLevel is level of the log message whose printage needs to be judged
        * @aLoggingComponent is the component where the log message 
        *     originates from
        * @return ETrue if logging is needed, based on aLevel, 
        *     aLoggingComponent and current log level. 
        */
        TBool CheckLogLevel(
            TPhoneUILoggingLevel aLevel, 
            TPhoneUILoggingComponent aLoggingComponent );
         
        /**
        * Writes comment to log file.
        *
        * @param aComment   - A comment descriptor to be logged.
        * @return None.
        */
        void WriteComment( const TDesC& aComment );

        /**
        * Creates a specified log message by given parameters and writes it 
        * in the log file.
        *
        * @param aMessage   - Ordinal of enumerated phone engine command. 
        *     Converts it in to descriptor.
        * @param aCallId    - ID of current call.
        * @return None.
        */
        IMPORT_C void LogMsgFromControlToView ( 
            const TInt aMessage, 
            const TInt aCallId );

        /**
        * Creates a PE specified log message by given parameters and writes 
        * it in the log file.
        *
        * @param aMessage   - Ordinal of enumerated phone engine command. 
        *     Converts it in to descriptor.
        * @param aCallId    - ID of current call.
        * @return None.
        */
        IMPORT_C void LogMsgFromPEToPhoneUI ( 
            const TInt aMessage, 
            const TInt aCallId );

        /**
        * Creates a PE specified log message by given TPEMessage id and call 
        * id and writes it in the log file.
        *
        * @param aMessage     - Ordinal of enumerated phone engine command. 
        *     Converts it in to descriptor.
        * @param aCallId    - ID of current call.
        * @return None.
        */
        IMPORT_C void LogMsgFromPhoneUIToPE( 
            const TInt aMessage, 
            const TInt aCallId );

        /**
        * Logs message from Phone Engine to Phoneapp execution ending. 
        * (When message is executed succesfully, this function is called).
        *
        * @param aMessage     - Ordinal of enumerated phone engine command. 
        *                       Converts it in to descriptor.
        * @return None.
        */
        IMPORT_C void LogMsgFromPEToPhoneUIEnd( const TInt aMessage );

        /**
        * Logs state change.
        *
        * @param aCurrentState Current state of phoneapp.
        * @param aNewState Next state of phoneapp.
        * @return None.
        */
        IMPORT_C void LogStateChange( 
            const TInt aCurrentState, 
            const TInt aNewState );
            
        /**
        * Sets pointer to Phone Engine
        * @param aPhoneEngine pointer to Phone Engine instance
        */
        IMPORT_C void SetPhoneEngine( MPEPhoneModel* aPhoneEngine );

    private:    // Methods from TDesOverflow

        /**
        * Formats a log message by given numerous parameters and writes 
        * it in the log file.
        *
        * @param aFmt       - 
        * @return None.
        */
        void LogIt( TRefByValue<const TDesC> aFmt, ...);
        
        /**
        * Handles the overflow. This function is called when the 
        * TDes16::AppendFormat() variant, which takes an overflow handler 
        * argument, fails. TDes16::AppendFormat() variant called by LogIt().
        *
        * @param aDes       - The 16 bit modifiable descriptor whose overflow 
        *                     results in the call to this overflow handler.
        * @return None.
        */
        IMPORT_C virtual void Overflow(TDes16& aDes);

    private: // New methods

        /**
        * Provides string representation for TPEMessagesFromPhoneEngine.
        *
        * @param aDes       - Descriptor for copying string representation.
        * @return None.
        */
        void GetPEToPhoneUIStringById( TDes16& aDes, const TInt aID );

        /**
        * Provides string representation for TPACommandId.
        *
        * @param aDes        - Descriptor for copying string representation.
        * @param aID         - Descriptor for copying string representation.
        * @return None.
        */
        void GetPhoneUIToPEStringById( TDes16& aDes, const TInt aID );

        /**
        * Provides string representation for MPhoneViewCommandId.
        *
        * @param aDes        - Descriptor for copying string representation.
        * @param aID         - Descriptor for copying string representation.
        * @return None.
        */
        void GetPhoneUIToViewStringById( TDes16& aDes, const TInt aID );

        /**
        * Recounts TPhoneStateId enumeration values for logger.
        *
        * @param aID         - TPhoneStateId enumeration value
        * @return            - Value of recounted enumeration id 
        */
        TInt RecountStateId( TInt aId );

        /**
        * Recounts TPhoneStateId GSM-variant enumeration values for logger.
        *
        * @param aID         - TPhoneStateId enumeration value
        * @return            - Value of recounted enumeration id 
        */
        TInt RecountStateIdGSM( TInt aId );

        /**
        * Provides string representation for state
        *
        * @param aDes        - Descriptor for copying string representation.
        * @param aID         - Id of the state.
        * @return            - None.
        */
        void GetStateName( TDes16& aDes, const TInt aID );
                    
        /** Array of logging levels for each component */
        TFixedArray<TPhoneUILoggingLevel, EPhoneAmountOfLoggingComponents> 
            iLogLevelArray;

    private: // attributes

        // String pool handle
        RStringPool iPool;
        
        /**
        * Pointer to Phone Engine - uses relation
        */
        MPEPhoneModel* iPhoneEngine;
    };
    
class CPhoneMethodLogger: public CBase
    {
    public:

        /** Method logger constructor. constructor.
          * Writes time lable and "Enter: " + the given string.
          */
        IMPORT_C CPhoneMethodLogger( 
            TPhoneUILoggingComponent aLoggingComponent, 
            TPtrC aString);

        /** Destructor.
          * Writes time lable and "Leave: " + the given string.
          */
        IMPORT_C ~CPhoneMethodLogger();

    private:

        TBuf<128> iString;
        TPhoneUILoggingComponent iLoggingComponent;
    };      

// These are empty logging macros when PHONEUI_LOGGING_OUTPUT==0

#if (PHONEUI_LOGGING_OUTPUT==0)
    #define __PHONELOG(A, B, C)
    #define __PHONELOG1(A,B, C, X)
    #define __PHONELOG2(A,B, C, X, Y)
    #define __PHONELOG3(A,B,  C, X, Y, Z)
    #define __PHONELOGENGINEMSG( C, X )
    #define __PHONELOGENGINEMSGEND( C)
    #define __PHONELOGENGINECMD( C)   
    #define __PHONELOGSTATECHANGE( C, X)
    #define __LOGMETHODSTARTEND(A,C)
    #define __PHONELOGVIEWMSG( C, X )   

// These are the real logging macros that uses CPhoneLogger class.
#else

//If logging method == 2 define log file name.
     
    #define __PHONELOG(A, B, C)             (static_cast<CPhoneLogger*> \
        (CCoeEnv::Static(KUidPhoneUILoggerSingleton)))-> \
            LogIt(A, B, _L(C) );
        
    #define __PHONELOG1(A, B, C, X)         (static_cast<CPhoneLogger*> \
        (CCoeEnv::Static(KUidPhoneUILoggerSingleton)))-> \
            LogIt(A, B, _L(C), X);
        
    #define __PHONELOG2(A, B, C, X, Y)      (static_cast<CPhoneLogger*> \
        (CCoeEnv::Static(KUidPhoneUILoggerSingleton)))-> \
            LogIt(A, B, _L(C), X, Y);
        
    #define __PHONELOG3(A, B, C, X, Y, Z)   (static_cast<CPhoneLogger*> \
        (CCoeEnv::Static(KUidPhoneUILoggerSingleton)))-> \
            LogIt(A, B, _L(C), X, Y, Z);

    #define __PHONELOGENGINEMSG( C, X )     (static_cast<CPhoneLogger*> \
        (CCoeEnv::Static(KUidPhoneUILoggerSingleton)))-> \
            LogMsgFromPEToPhoneUI( (TInt)C, X);
        
    #define __PHONELOGENGINEMSGEND( C )      (static_cast<CPhoneLogger*> \
        (CCoeEnv::Static(KUidPhoneUILoggerSingleton)))-> \
            LogMsgFromPEToPhoneUIEnd( (TInt)C );
    
    #define __PHONELOGENGINECMD( C )         (static_cast<CPhoneLogger*> \
        (CCoeEnv::Static(KUidPhoneUILoggerSingleton)))-> \
            LogMsgFromPhoneUIToPE( (TInt)C, 0 );
    
    #define __PHONELOGVIEWMSG( C, X )       (static_cast<CPhoneLogger*> \
        (CCoeEnv::Static(KUidPhoneUILoggerSingleton)))-> \
            LogMsgFromControlToView( (TInt)C, X);
    
    #define __PHONELOGSTATECHANGE( C, X )    (static_cast<CPhoneLogger*> \
        (CCoeEnv::Static(KUidPhoneUILoggerSingleton)))-> \
            LogStateChange( (TInt)C, TInt(X) );

    #define __LOGMETHODSTARTEND( A, C )     CPhoneMethodLogger __log( A, ( _L(C) ) );


#endif   // else --> #if (PHONE_LOGGING_METHOD==0 )

#endif   // #ifndef __CPhoneLogger_H__

