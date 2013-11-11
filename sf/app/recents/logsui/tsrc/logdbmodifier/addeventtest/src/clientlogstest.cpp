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

#include "clientlogstest.h"
#include <f32file.h>
#include <s32file.h>
#include <utf.h>
#include <bacline.h>
#include <EscapeUtils.h> 


#ifdef EKA2 //RD_APPS_TO_EXES 
#include <eikstart.h>
#endif
_LIT(KtestlogclientsFileName,"C:\\System\\testlogclients.dat");


#include <e32std.h>
#include <eikstart.h>

_LIT(KClientlogstestPanic,"Clientlogstest creation");

LOCAL_C void MainL(); 

// main function called by E32
GLDEF_C TInt E32Main() 
	{
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(error,MainL());
	__ASSERT_ALWAYS(error == KErrNone,User::Panic(KClientlogstestPanic,error));
	delete cleanup; // destroy clean-up stack
	User::Heap().Reset();
	return 0; // and return
	}
      
LOCAL_C void MainL() 
	{RDebug::Print(_L("CClientlogstest MainL"));
	RThread().SetPriority(EPriorityAbsoluteForeground);	
	// install an active scheduler
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	//CleanupStack::PushL(scheduler);
	//CleanupStack::Pop(scheduler);

	CClientlogstest* main = 0;
	TRAPD (err, main = CClientlogstest::NewL());
    if (err == KErrNone) 
    	{RDebug::Print(_L("CClientlogstest CActiveScheduler::Start()"));
    	CActiveScheduler::Start();
    	}
  RDebug::Print(_L("CClientlogstestafter  CActiveScheduler::Start()"));
  delete main;
  delete scheduler;
  
	}
      
CClientlogstest::~CClientlogstest()
    {
    	RDebug::Print(_L("CClientlogstest deleted"));
    	RDebug::Print(_L("CClientlogstest Cancel()"));
    	CActive::Cancel();
    	RDebug::Print(_L("CClientlogstest delete iLogEvent"));
    	delete iLogEvent;
    	iLogEvent = NULL;
    	RDebug::Print(_L("CClientlogstest delete iLogClient"));
    	delete iLogClient;
    	iLogClient = NULL;
    	RDebug::Print(_L("CClientlogstest iLogEvents.ResetAndDestroy()"));
    	iLogEvents.ResetAndDestroy();
    	RDebug::Print(_L("CClientlogstest iFsSession.Close()"));
    	iFsSession.Close();
    	RDebug::Print(_L("CClientlogstest delete iDescription"));
    	delete iDescription;
    	iDescription = NULL;
    	RDebug::Print(_L("CClientlogstest delete iRemoteParty"));
			delete iRemoteParty;
			iRemoteParty = NULL;
			RDebug::Print(_L("CClientlogstest delete iDirection"));
			delete iDirection;
			iDirection = NULL;
			RDebug::Print(_L("CClientlogstest delete iCallStatus"));
			delete iCallStatus;
			iCallStatus = NULL;
			RDebug::Print(_L("CClientlogstest delete iSubject"));
			delete iSubject;
			iSubject = NULL;
			RDebug::Print(_L("CClientlogstest delete iNumber"));
			delete iNumber;
			iNumber = NULL;
			RDebug::Print(_L("CClientlogstest delete iData"));
			delete iData;
			iData = NULL;
			delete ikeyword;
			ikeyword = NULL;
			delete iArgs;
			iArgs = NULL;
			RDebug::Print(_L("CClientlogstest deleted <-"));
			//CActiveScheduler::Install (NULL);
			//CActiveScheduler::Stop();
    }    
// From CActive
void CClientlogstest::RunL ()
	{
    RDebug::Print(_L("CClientlogstest::RunL->"));
    if (iStatus.Int() == KErrNone)
    	{RDebug::Print(_L("CClientlogstest::RunL iStatus.Int() == KErrNone"));
    	if (iLogEvents.Count() > 0)
    		{RDebug::Print(_L("CClientlogstest::RunL if (iLogEvents.Count() > 0)"));
    		delete iLogEvent;
    		iLogEvent = NULL;
    		RDebug::Print(_L("CClientlogstest::RunL iStatus.Int() == KErrNone"));
    		User::LeaveIfError(AddEvent());
    		}
    	else
    		{
    		RDebug::Print(_L("CClientlogstest::RunL <-"));
    		CActiveScheduler::Stop();
    		}
    	}
    else
    	{	
    	if (iStatus.Int() == KErrNotFound)
    	    {
    	    //if the event type is not registered with the Log Engine
    	    RDebug::Print(_L("CClientlogstest::RunL event type is not registered with the Log Engine"));
    	    }
    	
    	RDebug::Print(_L("CClientlogstest::RunL iStatus.Int() != KErrNone"));
    	CActiveScheduler::Stop();
			}
	}

// From CActive
void CClientlogstest::DoCancel ()
	{
		RDebug::Print(_L("CClientlogstest::DoCancel"));

	}



CClientlogstest* CClientlogstest::NewL()
    {RDebug::Print(_L("CClientlogstest NewL"));
    CClientlogstest* self = new (ELeave) CClientlogstest();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CClientlogstest::CClientlogstest()
: CActive(EPriorityStandard)
{
}

void CClientlogstest::ConstructL()
	{RDebug::Print(_L("CClientlogstest ConstructL->"));
		CActiveScheduler::Add (this);
		//TRequestStatus *status=&iStatus;
  	User::LeaveIfError(iFsSession.Connect());  
		iLogClient = CLogClient::NewL( iFsSession );	
		iLogEvent = CLogEvent::NewL();
		TBuf<100> fileName;
  	fileName.Append( KtestlogclientsFileName );
  	CCommandLineArguments* args = CCommandLineArguments::NewLC();
  	//ikeyword = HBufC::NewL(0);
  	_LIT(Kkeywordlogs,"logevent");
  	TBufC<16> buf1(Kkeywordlogs);
  	
  	ikeyword = buf1.AllocL();
		if (args->Count() > 1)
			{
			delete ikeyword;
			ikeyword = NULL;
			ikeyword = args->Arg(1).AllocL();
			RDebug::Print(*ikeyword);
			}
		CleanupStack::PopAndDestroy(args);
  	ReadParametersL( fileName );
  	User::LeaveIfError(AddEvent()); 
   RDebug::Print(_L("CClientlogstest ConstructL<-"));
  }

TInt CClientlogstest::AddEvent()
    {RDebug::Print(_L("CClientlogstest::AddEvent()->"));
    TInt errorCode ( KErrNone );
    iLogEvent = iLogEvents[0];
    iLogEvents.Remove(0);
    iLogEvents.Compress();           
    if ( !IsActive( ) )
        {
        RDebug::Print(_L("CClientlogstest::AddEvent() CLogClient::AddEvent()"));	
        iLogClient->AddEvent( *iLogEvent, iStatus );
        errorCode = KErrNone;
        SetActive( );        
        }
    else
        {
        RDebug::Print(_L("CClientlogstest::AddEvent() Event NOT added")); 	
        errorCode = KErrInUse;
        }
    RDebug::Print(_L("CClientlogstest::AddEvent()<-"));    
    return errorCode;
    }


void CClientlogstest::ReadParametersL( const TDes& fileName )
    {RDebug::Print(_L("CClientlogstest ReadParametersL ->"));
    iArgs = new ( ELeave ) CArgs();
    RFileReadStream readStream;
    TBuf8<256> asciiLine;
    TBuf16<256> unicodeLine;
    TInt findPos;
    TInt leaveCode( KErrNone );
		iCompleteError = KErrNone;
		iId = 0;
		iEventType.Null();
		iRemoteParty = NULL;
		iDirection = NULL;
		iTime = 0;
		iDurationType = 0;
		iDuration = 0;
		iCallStatus = NULL;
		iSubject = NULL;
		iNumber = NULL;
		iContact = 0;
		iDescription = NULL;
		iData = NULL;
		iFlags = 0;
  	if ( KErrNone != readStream.Open( iFsSession, fileName, EFileRead ) )
     	{RDebug::Print(_L("CClientlogstest ReadParametersL KErrNone != readStream.Open "));
     	CleanupStack::PopAndDestroy();
     	return;
     	}
   	
  	CleanupClosePushL( readStream );
  	while ( KErrEof != leaveCode )
      	{RDebug::Print(_L("CClientlogstest ReadParametersL while ")); 
        TRAP( leaveCode, readStream.ReadL( asciiLine, TChar(10)) )
        if ( KErrNone == leaveCode )
            {
            // Delete leading spaces and tabs
            asciiLine.TrimLeft();
            // Delete trailing (CR)+LF if any 
            findPos = asciiLine.Locate( TChar(10) );
            if ( KErrNotFound  != findPos )
                {
                if ( KErrNotFound != asciiLine.Locate( TChar(13) ) )
                    {
                    // Dos style text file(CR+LF)
                    asciiLine.Delete( 
                        findPos - 1, asciiLine.Length() - (findPos + 1) );
                    }
                else
                    {
                    // Unix style text file(LF)
                    asciiLine.Delete( 
                        findPos , asciiLine.Length() - findPos ); 
                    }
                }            
        
            // Skip empty lines and lines that start with '//'
            if ( asciiLine.Length() == 0 || 
                (findPos = asciiLine.Find(_L8("//"))) == 0 )
                {
                continue;
                }
        
            if ( KErrNotFound != findPos )
                {
                 asciiLine.Delete( findPos, asciiLine.Length() - findPos ); 
                 asciiLine.TrimRight();
                }
        
            // Unicode format is used internally
            TInt ret = 
                CnvUtfConverter::ConvertToUnicodeFromUtf8( 
                    unicodeLine, asciiLine );
        
            if ( KErrNone != ret )
                {
                //Unicode conversion failed. Skip this line and go on
                continue;
                }

            if ( KErrNone != iArgs->ParseLineL( EParseFunction, unicodeLine ) )
                {
                continue;
                }
            // Long lines maintained for clarity
            if ( iArgs->StrArg(1) == _L("") )
                {
            //    if ( iArgs->StrArg(2) == _L("logevent") )
            			if (iArgs->StrArg(2) == *ikeyword )
                    {
                    iId = iArgs->NumArg(3);
                    iEventType.iUid = iArgs->NumArg(4);
                    iRemoteParty = iArgs->StrArg(5).AllocL();
                    
                    iDirection = iArgs->StrArg(6).AllocL();
                    RDebug::RawPrint(*iDirection);
                    iTime = iArgs->NumArg(7);
                    iDurationType = iArgs->NumArg(8);
                    iDuration = iArgs->NumArg(9);
                    iCallStatus = iArgs->StrArg(10).AllocL();
                    iSubject = iArgs->StrArg(11).AllocL();
                    iNumber = iArgs->StrArg(12).AllocL();
                    iContact = iArgs->NumArg(13);
                    iDescription = iArgs->StrArg(14).AllocL();
                    RDebug::Print(_L("CClientlogstest data:"));
                    RDebug::RawPrint(iArgs->StrArg(15));
                    iData = HBufC8::NewL( iArgs->StrArg(15).Length() * 2 );
                    TPtr8 ptrData( iData->Des() );
                    ptrData.Copy( iArgs->StrArg(15) );
                    _LIT8( KLogClientFieldDelim, "\\t" );
                    int delimPos = ptrData.Find( KLogClientFieldDelim );
                    while ( delimPos >= 0 ){
                        ptrData.Replace( delimPos, KLogClientFieldDelim().Length(), _L8("\t") );
                        int newDelimPos = ptrData.Mid( delimPos + 1 ).Find( KLogClientFieldDelim );
                        delimPos = ( newDelimPos >= 0 ) ? delimPos + newDelimPos + 1 : -1;
                    }
                    iFlags = iArgs->NumArg(16); 
                    
                    delete iLogEvent;
                    iLogEvent = CLogEvent::NewL();
                    RDebug::Print(_L("CClientlogstest ReadParametersL while iLogEvent->SetId(iId)"));
                    iLogEvent->SetId(iId);
                    //iLogEvent->SetEventType(iEventType);
                    iLogEvent->SetEventType( KLogCallEventTypeUid );
                    iLogEvent->SetRemoteParty(*iRemoteParty);
                    iLogEvent->SetDirection(*iDirection);
                    iLogEvent->SetTime(iTime);
                    iLogEvent->SetDurationType(iDurationType);
                    iLogEvent->SetDuration(iDuration);
                    iLogEvent->SetStatus(*iCallStatus);
                    iLogEvent->SetSubject(*iSubject);
                    iLogEvent->SetNumber(*iNumber);
                    iLogEvent->SetContact(iContact);
                    iLogEvent->SetDescription(*iDescription);
                    iLogEvent->SetFlags(iFlags);
                    RDebug::Print(_L("CClientlogstest ReadParametersL while After set logevent parameters"));
                    iDataPtr.Set( iData->Des() );
                    iLogEvent->SetDataL(iDataPtr);
                    //own the objects
                    RDebug::Print(_L("CClientlogstest ReadParametersL while append LogEvent obj to array"));
                    iLogEvents.AppendL(iLogEvent);
                    iLogEvent = NULL;
                    DeleteReadDataMembers();
                    
										RDebug::Print(_L("CClientlogstest ReadParametersL while After deleting all member parameters"));
                    }
                }
            else
                {
                // not a valid line
                }
            }
        }//while

    CleanupStack::PopAndDestroy();//readStream
    RDebug::Print(_L("CClientlogstest ReadParametersL <-"));
    }

void CClientlogstest::DeleteReadDataMembers()
	{
	RDebug::RawPrint(*iDescription);
  delete iDescription;
  iDescription = NULL;
  RDebug::RawPrint(*iRemoteParty);
	delete iRemoteParty;
	iRemoteParty = NULL;
	RDebug::RawPrint(*iDirection);
	delete iDirection;
	iDirection = NULL;
	RDebug::RawPrint(*iCallStatus);
	delete iCallStatus;
	iCallStatus = NULL;
	RDebug::RawPrint(*iSubject);
	delete iSubject;
	iSubject = NULL;
	RDebug::RawPrint(*iNumber);
	delete iNumber;
	iNumber = NULL;
	//RDebug::RawPrint(*iData);
	delete iData;
	iData = NULL;
	}

// End of file	

