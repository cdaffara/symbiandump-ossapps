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
#include "MPhoneViewCommandHandle.h"
#include "TPhoneCommandParam.h"
#include "TPhoneCmdParamBoolean.h"
#include "TPhoneCmdParamInteger.h"
#include "TPhoneCmdParamEmergencyCallHeaderData.h"
#include "TPhoneCmdParamCallStateData.h"

#include "TPhoneCmdParamCallHeaderData.h"

struct TCmds
     {
public:
     TInt iCmd;
     TPhoneUICommandParam* iP;
     }; 

class CPhoneViewController : public MPhoneViewCommandHandle, public CBase
    {
   
    
    public:  // Constructors and destructor
        
        CPhoneViewController(){}
      
        static CPhoneViewController*  NewL()
            {
            CPhoneViewController* self = new (ELeave) CPhoneViewController();
            return self;
            }
        
        
        /**
        * Destructor.
        */
        ~CPhoneViewController()
            {
            if(iCmds)
                {
                for(TInt i = 0; i<iCmds->Count(); i++)
                    {
                    TCmds t = iCmds->At(i);
                    delete t.iP;
                
                    }
                
                iCmds->Reset();
                }
            
            delete iCmds;
            };

    public: // New functions
        
        /**
        * Execute Command with given Command Id
        * @param aCmdId Command Id
        * @return None
        */
        void ExecuteCommandL( 
            TPhoneViewCommandId aCmdId )
            {
            if(iCmds == NULL)
                iCmds = new (ELeave) CArrayFixFlat<TCmds> (10);
            TCmds cmd;
            cmd.iCmd = aCmdId;
            cmd.iP = NULL;
            iCmds->AppendL( cmd );
            }

        /**
        * Execute Command with given Command Id  and Call Id
        * @param aCmdId Command Id
        * @param aCallId Call Id
        * @return None
        */
         void ExecuteCommandL( 
            TPhoneViewCommandId aCmdId, 
            TInt /*aCallId*/ ) 
            {
            ExecuteCommandL( aCmdId );
            
            }

         /**
        * Execute Command with given command id, call id and command parameter.
        * @param aCmdId Command Id
        * @param aCommandParam Command parameter
        * @return None
        */
         void ExecuteCommandL( 
            TPhoneViewCommandId aCmdId, 
            TPhoneCommandParam* aCommandParam ) 
             {
             if( iCmds == NULL )
                 iCmds = new (ELeave) CArrayFixFlat<TCmds> (10);
            TCmds cmd;
            cmd.iCmd = aCmdId;
            
            switch ( aCmdId )
                {
                case EPhoneViewGetCountOfActiveCalls:
                    {
                    if ( aCommandParam->ParamId() == 
                            TPhoneCommandParam::EPhoneParamIdInteger )
                        {
                        TPhoneCmdParamInteger* intParam = 
                            static_cast<TPhoneCmdParamInteger*>( aCommandParam );
                        
                        intParam->SetInteger( iActiveCallCount );
                        }
                    }
                    break;
                                   
                case EPhoneViewGetCallExistsInConference:
                    {
                    if ( aCommandParam->ParamId() == 
                            TPhoneCommandParam::EPhoneParamIdBoolean )
                        {
                        static_cast<TPhoneCmdParamBoolean*>(aCommandParam)->SetBoolean( iCallExistsInConference ); 
                        }
                    }
                    break;
                }
            
            if ( aCommandParam->ParamId() == TPhoneCommandParam::EPhoneParamIdBoolean )
                {
                TPhoneCmdParamBoolean* value =
                                            static_cast<TPhoneCmdParamBoolean*>( aCommandParam );
                TPhoneCmdParamBoolean* value2 = new (ELeave) TPhoneCmdParamBoolean (*value);
                cmd.iP = value2;
                }
            else if ( aCommandParam->ParamId() == TPhoneCommandParam::EPhoneParamIdInteger )
                {
                TPhoneCmdParamInteger* value =
                             static_cast<TPhoneCmdParamInteger*>( aCommandParam );
                TPhoneCmdParamInteger* value2 = new (ELeave) TPhoneCmdParamInteger (*value);
                cmd.iP = value2;             
                }
            else if ( aCommandParam->ParamId() == TPhoneCommandParam::EPhoneParamIdCallHeaderData )
                {
                TPhoneCmdParamCallHeaderData* value =
                            static_cast<TPhoneCmdParamCallHeaderData*>( aCommandParam );
                TPhoneCmdParamCallHeaderData* value2 = new (ELeave) TPhoneCmdParamCallHeaderData( *value);  
                cmd.iP = value2;             
                }
            else if(  aCommandParam->ParamId() == TPhoneCommandParam::EPhoneParamIdEmergencyCallHeaderData)
                {
                TPhoneCmdParamEmergencyCallHeaderData* value =
                                            static_cast<TPhoneCmdParamEmergencyCallHeaderData*>( aCommandParam );
                TPhoneCmdParamEmergencyCallHeaderData* value2 = new (ELeave) TPhoneCmdParamEmergencyCallHeaderData( *value);      
                cmd.iP = value2;       
                }
             else
                {
                cmd.iP = NULL;
                }
            
            iCmds->AppendL( cmd );
            }

         /**
        * Execute Command with given command id, call id and command parameter.
        * @param aCmdId Command Id
        * @param aCallId Call Id
        * @param aCommandParam Command parameter
        * @return None
        */
         void ExecuteCommandL( 
            TPhoneViewCommandId aCmdId, 
            TInt /*aCallId*/, 
            TPhoneCommandParam* aCommandParam ) 
             {
             ExecuteCommandL( aCmdId, aCommandParam );
             }

        /**
        * Execute Command with given command id, call id and message.
        * @param aCmdId Command Id
        * @param aCallId Call Id
        * @param aMessage Command message
        */
        void ExecuteCommandL( 
            TPhoneViewCommandId /*aCmdId*/, 
            TInt /*aCallId*/, 
            TDesC& /*aMessage*/ ){}

        /**
        * From MPhoneViewCommandHandle
        */    
        void ExecuteCommand( 
                   TPhoneViewCommandId /*aCmdId*/ ){}
                   
        /**
        * From MPhoneViewCommandHandle
        */
        void ExecuteCommand( 
                   TPhoneViewCommandId aCmdId, 
                   TPhoneCommandParam* /*aCommandParam*/ )
            {
            ExecuteCommandL( aCmdId );
            }

        /**
        * Handle Command with given Command Id
        * @param aCmdId Command Id
        * @return TPhoneViewResponseId
        */
        TPhoneViewResponseId HandleCommandL( 
            TPhoneViewCommandId /*aCmdId*/ ) { return iViewResponseId; }

        /**
        * Handle Command with given command id and command parameter.
        * @param aCmdId Command Id
        * @param aCommandParam Command parameter
        * @return TPhoneViewResponseId
        */
        TPhoneViewResponseId HandleCommandL( 
            TPhoneViewCommandId /*aCmdId*/, 
            TPhoneCommandParam* aCommandParam ) 
            {
            
            if ( aCommandParam->ParamId() == 
                 TPhoneCommandParam::EPhoneParamIdCallStateData )
                {
                TPhoneCmdParamCallStateData* stateParam = 
                          static_cast<TPhoneCmdParamCallStateData*>( aCommandParam );
                     stateParam->SetCallId( iCallId );
                }
                
            return iViewResponseId; }
       
        /**
        * Execute Command with given Command Id
        */     
         const TDesC& FetchContent() { return KNullDesC();}     
   
  //helper functions:       
         
         TInt VerifyViewCommand( const TInt& aCmd )
             {
             _LIT(KDesC, "Checking command id = %d ");
             EUNIT_PRINT( KDesC(), aCmd );
             
             TInt count = iCmds->Count();
             TBool cmdFound = EFalse;
             TInt i = 0;
             for ( ; i < count; i++ )
                 {
                 TCmds cmd = iCmds->At(i);
                 if( cmd.iCmd == aCmd )  
                     {
                     cmdFound = ETrue;
                     break;
                     }
                 }
             
             EUNIT_ASSERT_DESC( cmdFound, "View Command not ok" );
             return i;
             }

         void VerifyViewCommandBoolean( const TInt& aCmd, const TBool& aValue )
             {
             TBool paramOk = EFalse;
             
             for( TInt j = 0 ; j<iCmds->Count(); j++)
                  {
                  TCmds cmd = iCmds->At(j);
                  if( cmd.iCmd == aCmd )  
                      {
                      _LIT(KDesC, "Checking command id = %d ");
                      EUNIT_PRINT( KDesC(), aCmd );
                      
                      TPhoneUICommandParam* p = cmd.iP;
                      TPhoneCmdParamBoolean* booleanValue =
                                        static_cast<TPhoneCmdParamBoolean*>( p );
                      TBool val = booleanValue->Boolean();
                      if( val == aValue )
                          {
                          paramOk = ETrue;
                          break;
                          }
                      }
                  }
             
             EUNIT_ASSERT_DESC( paramOk, "View Boolean param not ok" );
             }

         void VerifyViewCommandInteger( const TInt& aCmd, const TInt& aValue )
             {
             TBool paramOk = EFalse;
            
             for( TInt j = 0 ; j< iCmds->Count(); j++)
                 {
                 TCmds cmd = iCmds->At(j);
                 if( cmd.iCmd == aCmd )  
                     {
                     _LIT(KDesC, "Checking command id = %d ");
                     EUNIT_PRINT( KDesC(), aCmd );
                     TPhoneCmdParamInteger* value = static_cast<TPhoneCmdParamInteger*>( cmd.iP );
                     
                     if( value->Integer() == aValue )
                         {
                         paramOk = ETrue;
                         break;
                         }
                     }
                 }
                 
             EUNIT_ASSERT_DESC( paramOk, "View Integer param not ok" );
             }

         void VerifyViewCommandCallHeader( const TInt& aCmd, const TPhoneCmdParamCallHeaderData& aValue )
             {
             TBool paramOk = EFalse;
             
             for( TInt j = 0 ; j< iCmds->Count(); j++)
                 {
                 TCmds cmd = iCmds->At(j);
                 if( cmd.iCmd == aCmd )  
                     {
                     _LIT(KDesC, "Checking command id = %d ");
                     EUNIT_PRINT( KDesC(), aCmd );
                     TPhoneCmdParamCallHeaderData* value = static_cast<TPhoneCmdParamCallHeaderData*>( cmd.iP );
                     if( CheckCallheaderValues( *value, aValue) == KErrNone )
                         {
                         paramOk = ETrue;
                         break;
                         }
                     }
                 }
             
             EUNIT_ASSERT_DESC( paramOk, "View Call Header param not ok" );
             }

         TInt CheckCallheaderValues(const TPhoneCmdParamCallHeaderData& aActual, 
                 const TPhoneCmdParamCallHeaderData& aExpected )
             {
             EUNIT_PRINT(_L("Checking CallHeaderData"));
             if( aActual.CallState() != aExpected.CallState())
                 {
                 EUNIT_PRINT(_L("CallState does not match."));
                 return KErrNotFound;
                 }
             
             if(aActual.Line2() != aExpected.Line2() )
                 {
                 EUNIT_PRINT(_L("Line2 does not match."));
                 return KErrNotFound;
                 }
                 
             return KErrNone;
             }

         void SetViewGetCountOfActiveCalls( TInt aValue )
             {
             iActiveCallCount = aValue;
             }


         void SetViewGetCallExistsInConference( TInt aValue )
             {
             iCallExistsInConference = aValue;
             }         
         
         void ResetCommands()
             {
             if(iCmds)
                 {
                 for(TInt i = 0; i<iCmds->Count(); i++)
                     {
                     TCmds t = iCmds->At(i);
                     delete t.iP;
                 
                     }
                 iCmds->Reset();
                 }
             }         
    public: // members 
         TPhoneViewResponseId iViewResponseId;
         CArrayFixFlat<TCmds>* iCmds;
         TInt                  iCallId;
         TInt                  iActiveCallCount;
         TBool                 iCallExistsInConference;
         TBool                 iHoldFlag; 
    };
 
