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
* Description:  CCPixNPPlugin implementation
*
*/

#include <fbs.h>        // CFbsBitmap
#include <bitdev.h>     // CFbsBitmapDevice
#include <bitstd.h>     // CFbsBitGc
#include <coemain.h>    // CCoeEnv

#include <smsclnt.h> // SMS Headers
#include <EIKENV.H>
#include <eikappui.h>
//@TODO: CPbkViewState not available in TB10.1 so removed
//#include <cpbkviewstate.h>
//#include <Pbk2ViewId.hrh>

//@TODO: Not supported in TB10.1
//#include <calenlauncher.h> // Calendar launching
#include "mnproviderfinder.h"
#include "mnmapview.h"

#include "CCPixNPPlugin.h"
#include "CCPixNPPluginEcom.h"
#include "CCPixNPSearcher.h"

#include "indevicecfg.h"
#include "MsvUI.h"

#include <APGCLI.H>
#include <APGTASK.H> //TApaTaskList
#include <W32STD.H>  //RWsSession
//UID of PinbMdl
const TInt KUidApaMessagePinboardValue(0x1000590C);
//UID of NotepadApp
const TInt KUidApaNotePad(0x10005907);
//
const TInt KNotePadIdMaxLength(10);

_LIT( KCPixDocUidFieldName, CPIX_DOCUID_FIELD );
_LIT( KCPixAppClassFieldName, CPIX_APPCLASS_FIELD );
_LIT( KCPixExcerptFieldName, CPIX_EXCERPT_FIELD );
_LIT( KCPixMimeTypeFieldName, CPIX_MIMETYPE_FIELD );
_LIT( KCPixAggregateFieldName, CPIX_DEFAULT_FIELD );
_LIT( KCPixDefaultFieldName, CPIX_DEFAULT_FIELD );
_LIT( KCPixTermTextFieldName, TERM_TEXT_FIELD );
_LIT( KCPixTermDocFreqFieldName, TERM_DOCFREQ_FIELD );

_LIT( KCPixStandardAnalyzerId, 		"" CPIX_ANALYZER_STANDARD );

_LIT( KCPixStandardTokenizerId, 	"" CPIX_TOKENIZER_STANDARD );
_LIT( KCPixWhitespaceTokenizerId,  	"" CPIX_TOKENIZER_WHITESPACE );
_LIT( KCPixLetterTokenizerId, 		"" CPIX_TOKENIZER_LETTER );
_LIT( KCPixKeywordTokenizerId, 		"" CPIX_TOKENIZER_KEYWORD );

_LIT( KCPixStandardFilterId, 		"" CPIX_FILTER_STANDARD );
_LIT( KCPixLowercaseFilterId, 		"" CPIX_FILTER_LOWERCASE );
_LIT( KCPixAccentFilterId, 			"" CPIX_FILTER_ACCENT );
_LIT( KCPixStopFilterId, 			"" CPIX_FILTER_STOP );
_LIT( KCPixStemFilterId, 			"" CPIX_FILTER_STEM );
_LIT( KCPixLengthFilterId, 			"" CPIX_FILTER_LENGTH );

_LIT( KCPixEnglishLanguageId, 		"" CPIX_WLANG_EN );
_LIT( KCPixFinnishLanguageId, 		"" CPIX_WLANG_FI );
_LIT( KCPixHungarianLanguageId, 	"" CPIX_WLANG_HU );
_LIT( KCPixRussianLanguageId, 		"" CPIX_WLANG_RU );

// Version number. This should be updated to current svn revision number
const TInt KVersionNumber = 1339;

CCPixNPPlugin::CCPixNPPlugin()
	{
	}

CCPixNPPlugin::~CCPixNPPlugin()
	{
    iFs.Close();
	iSearchServer.Close();
	
	iMnProviders.ResetAndDestroy();
	iMnProviders.Close();
	delete iMapView;
	
	delete iMsvUI;
	}

CCPixNPPlugin* CCPixNPPlugin::NewLC()
	{
	CCPixNPPlugin* self = new (ELeave)CCPixNPPlugin();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CCPixNPPlugin* CCPixNPPlugin::NewL()
	{
	CCPixNPPlugin* self = CCPixNPPlugin::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CCPixNPPlugin::ConstructL()
	{
	InitInterfaceL();
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError( iSearchServer.Connect() );
	iMsvUI = new (ELeave) CMsvUI();
	}

//
// Exposed functions
//

const TDesC& CCPixNPPlugin::DocUidFieldName() const
	{
	return KCPixDocUidFieldName();  
	}

const TDesC& CCPixNPPlugin::AppClassFieldName() const
	{
	return KCPixAppClassFieldName(); 
	}

const TDesC& CCPixNPPlugin::ExcerptFieldName() const
	{
	return KCPixExcerptFieldName(); 
	}

 const TDesC& CCPixNPPlugin::MimeTypeFieldName() const
	{
	return KCPixMimeTypeFieldName(); 
	}

const TDesC& CCPixNPPlugin::AggregateFieldName() const
	{
	return KCPixAggregateFieldName(); 
	}

const TDesC& CCPixNPPlugin::DefaultFieldName() const
	{
	return KCPixDefaultFieldName(); 
	}

const TDesC& CCPixNPPlugin::TermTextFieldName() const
	{
	return KCPixTermTextFieldName(); 
	}

const TDesC& CCPixNPPlugin::TermDocFreqFieldName() const
	{
	return KCPixTermDocFreqFieldName(); 
	}

const TDesC& CCPixNPPlugin::StandardAnalyzerId() const
	{
	return KCPixStandardAnalyzerId();
	}

const TDesC& CCPixNPPlugin::StandardTokenizerId() const
	{
	return KCPixStandardTokenizerId();
	}

const TDesC& CCPixNPPlugin::WhitespaceTokenizerId() const
	{
	return KCPixWhitespaceTokenizerId();
	}

const TDesC& CCPixNPPlugin::LetterTokenizerId() const
	{
	return KCPixLetterTokenizerId();
	}

const TDesC& CCPixNPPlugin::KeywordTokenizerId() const
	{
	return KCPixKeywordTokenizerId();
	}

const TDesC& CCPixNPPlugin::StandardFilterId() const
	{
	return KCPixStandardFilterId();
	}

const TDesC& CCPixNPPlugin::LowercaseFilterId() const
	{
	return KCPixLowercaseFilterId();
	}

const TDesC& CCPixNPPlugin::AccentFilterId() const
	{
	return KCPixAccentFilterId();
	}

const TDesC& CCPixNPPlugin::StopFilterId() const	
	{
	return KCPixStopFilterId();
	}

const TDesC& CCPixNPPlugin::StemFilterId() const
	{
	return KCPixStemFilterId();
	}

const TDesC& CCPixNPPlugin::LengthFilterId() const
	{
	return KCPixLengthFilterId();
	}

const TDesC& CCPixNPPlugin::EnglishLanguageId() const
	{
	return KCPixEnglishLanguageId(); 
	}

const TDesC& CCPixNPPlugin::FinnishLanguageId() const
	{
	return KCPixFinnishLanguageId(); 
	}

const TDesC& CCPixNPPlugin::HungarianLanguageId() const
	{
	return KCPixHungarianLanguageId(); 
	}

const TDesC& CCPixNPPlugin::RussianLanguageId() const
	{
	return KCPixRussianLanguageId(); 
	}

CCPixNPSearcher* CCPixNPPlugin::CreateSearcherL( const TDesC& aDomainSelector, const TDesC& aDefaultSearchField ) const
	{
	return CCPixNPSearcher::NewL( aDomainSelector, aDefaultSearchField );
	}

CCPixNPSearcher* CCPixNPPlugin::CreateSearcherL( const TDesC& aDomainSelector ) const
	{
	return CCPixNPSearcher::NewL( aDomainSelector );
	}

void CCPixNPPlugin::DefineVolumeL( const TDesC& aQualifiedBaseAppClass, const TDesC& aIndexDbPath )
	{
	User::LeaveIfError(iSearchServer.DefineVolume( aQualifiedBaseAppClass, aIndexDbPath ));
	}

void CCPixNPPlugin::UnDefineVolumeL( const TDesC& aQualifiedBaseAppClass )
	{
	User::LeaveIfError(iSearchServer.UnDefineVolume( aQualifiedBaseAppClass ));
	}


void CCPixNPPlugin::OpenContactL( TInt aContactId )
	{
	//@TODO: Contact can not belaunched by this API in TB10.1
	/*	
	CPbkViewState* pbkViewParam = CPbkViewState::NewLC();
	pbkViewParam->SetFocusedContactId(aContactId);
	 
	// Package and return object in a buffer.
	HBufC8* paramBuf = pbkViewParam->PackLC();
	 
	// Uid for Contacts application
	const TUid appUid = {0x101f4cce};
	// Messaging: 0x100058C5
	 
	// Activate the view
	// 3 is the Contact Group Info View's ID
	CEikonEnv::Static()->EikAppUi()->ActivateViewL(TVwsViewId(appUid, TUid::Uid(EPbk2ContactInfoViewId)), CPbkViewState::Uid(), *paramBuf);
	 
	// Cleanup
	CleanupStack::PopAndDestroy(2); // paramBuf, pbkViewParam	
	*/
	}


void CCPixNPPlugin::OpenMessagingL( TInt aMessagingId )
	{
	iMsvUI->OpenEntryL(TUid::Uid(aMessagingId));
	}

void CCPixNPPlugin::OpenCalendarL( TInt aCalId )
	{
	//@TODO: Not supported in TB10.1
	//CalenLauncher::ViewEntryL(aCalId);
	}

void CCPixNPPlugin::OpenMapsL( TReal aLatitude, TReal aLongitude )
    {
    const TReal KMapRadius = 500; // Radius in meters
    
    if ( !iMapView )
        {
        iMnProviders.ResetAndDestroy();
        MnProviderFinder::FindProvidersL( iMnProviders, CMnProvider::EServiceMapView );

        if ( iMnProviders.Count() > 0 )
            {
            iMapView = CMnMapView::NewL( *iMnProviders[0] );
            iMapView->SetExitObserverL(*this);
            }
        }

    TCoordinate coord(aLatitude, aLongitude);
    
    iMapView->ResetMapArea();
    iMapView->SetMapAreaL( coord, KMapRadius );
    iMapView->ShowMapL();
    }

TInt CCPixNPPlugin::GetVersionNumber() const
    {
    return KVersionNumber;
    }

void CCPixNPPlugin::HandleServerAppExit(TInt /*aReadon*/)
    {
    // Maps application close. We can't use maps handle anymore.
    delete iMapView;
    iMapView = 0;
    }

//Added to launch Notes from Widget
void CCPixNPPlugin::OpenNoteL( const TInt aNoteId)
    {
    RWsSession wssession;
    User::LeaveIfError( wssession.Connect() );
    CleanupClosePushL( wssession );
    TBuf8<KNotePadIdMaxLength> notesId;
    notesId.AppendNum(aNoteId);
    
    //Get the tasklist from the Window server session
    TApaTaskList taskList( wssession );
    //Find the task related to NotePad app
    TApaTask task = taskList.FindApp( TUid::Uid( KUidApaNotePad ) );

    if ( task.Exists() )
        {
        // NotePad app is running
        //Send a message to Notepad app with PinBoard UID as
        //NotePad is handling the message only if it get Pinboard Uid
        User::LeaveIfError( task.SendMessage( TUid::Uid( KUidApaMessagePinboardValue ), 
                notesId ) );
        //Bring the Notepad to Foreground
        task.BringToForeground();
        }
    else
        { // NotePad app not yet running
        RApaLsSession appArcSession;
        CleanupClosePushL( appArcSession );
        User::LeaveIfError( appArcSession.Connect() );      
        TApaAppInfo appInfo;
        const TInt err = appArcSession.GetAppInfo( appInfo, 
                                        TUid::Uid( KUidApaNotePad ) );
        if( err == KErrNone )
            {
            CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
            cmdLine->SetExecutableNameL( appInfo.iFullName );
            cmdLine->SetCommandL( EApaCommandRun );
            //Send the Note Id as the tail
            cmdLine->SetTailEndL( notesId );

            User::LeaveIfError( appArcSession.StartApp( *cmdLine ) );
            CleanupStack::PopAndDestroy( cmdLine );
            }
        CleanupStack::PopAndDestroy( &appArcSession ); 
        } 
    CleanupStack::PopAndDestroy( &wssession );
    }

void CCPixNPPlugin::GetDriveFromMediaIdL(HBufC* aMediaId,TDes& aDrive)
    {
    TUint32 mediaId;
    TBuf<14> bufMedia;
    bufMedia.Copy(*aMediaId);
    TUint mediaNum;
    TLex lex(bufMedia);
    lex.Val(mediaNum);
    TDriveNumber drive = TDriveNumber(KErrNotSupported);//Initialize to not supported first
    TVolumeInfo volumeInfo;
    TChar driveLetter;
    for(TInt i=0;i<=EDriveZ;i++)
       {
       TInt err= iFs.Volume(volumeInfo,i);
        if (err!=KErrNotReady) 
            // Volume() returns KErrNotReady if no volume present.
            // In this case, check next drive number
            {
            if(volumeInfo.iUniqueID == mediaNum)
                {
                drive = TDriveNumber(i);//Is the drive
                User::LeaveIfError(iFs.DriveToChar(drive,driveLetter));
                break;
                }
            }
       }
    aDrive.Append(driveLetter);
    aDrive.Append(_L(":"));
    //Convert to Int and get the drive
    
    }
//  End of File

