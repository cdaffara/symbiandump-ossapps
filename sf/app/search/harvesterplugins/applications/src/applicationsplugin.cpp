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

#include "applicationsplugin.h"
#include "harvesterserverlogger.h"
#include <common.h>

#include <ccpixindexer.h>
#include <csearchdocument.h>
#include <e32base.h>
//#include <menu2internalcrkeys.h> //for KCRUidMenu
//#include <WidgetPropertyValue.h> // EBundleDisplayName 
#include <centralrepository.h>
#include <opensystemtrace.h> 
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "applicationspluginTraces.h"
#endif
#include<usif/scr/appreginfo.h>
//Hidden applications
//#define KHiddenAppRepositoryUid KCRUidMenu
#define KNumberOfAppInfoToBeRead 1
_LIT( KMimeTypeField, CPIX_MIMETYPE_FIELD );
_LIT( KMimeTypeApplication, APPLICATION_MIMETYPE);

/** Field names */
_LIT(KApplicationFieldShortCaption, "Name");
_LIT(KApplicationFieldCaption, "CaptionName");
_LIT(KApplicationFieldUid, "Uid");
_LIT(KApplicationFieldAbsolutePath, "Path");

// TAppInfo.Name() returns [121345678]. The below constants are used to extract '[' & ']'
const TInt KUidStartIndex = 1;
const TInt KUidEndIndex = 8;

/** The delay between harvesting chunks. */
const TInt KHarvestingDelay = 1000;

// -----------------------------------------------------------------------------
CApplicationsPlugin* CApplicationsPlugin::NewL()
	{
    OstTraceFunctionEntry0( CAPPLICATIONSPLUGIN_NEWL_ENTRY );
    CPIXLOGSTRING("CApplicationsPlugin::NewL()");
	CApplicationsPlugin* instance = CApplicationsPlugin::NewLC();
    CleanupStack::Pop(instance);
    OstTraceFunctionExit0( CAPPLICATIONSPLUGIN_NEWL_EXIT );
    return instance;
	}

// -----------------------------------------------------------------------------
CApplicationsPlugin* CApplicationsPlugin::NewLC()
	{
	CApplicationsPlugin* instance = new (ELeave) CApplicationsPlugin();
    CleanupStack::PushL(instance);
    instance->ConstructL();
    return instance;
	}

// -----------------------------------------------------------------------------
CApplicationsPlugin::CApplicationsPlugin()
	{
	}

// -----------------------------------------------------------------------------
CApplicationsPlugin::~CApplicationsPlugin()
    {
    OstTraceFunctionEntry0( CAPPLICATIONSPLUGIN_CAPPLICATIONSPLUGIN_ENTRY );

    if (iAsynchronizer)
        iAsynchronizer->CancelCallback();
    //iApplicationServerSession.Close();
    iScrView.Close();
    iScrSession.Close();
    //iWidgetRegistry.Close();
    //delete iHiddenApplicationsRepository;
    delete iAsynchronizer;
    delete iNotifier;
    delete iIndexer;
    OstTraceFunctionExit0( CAPPLICATIONSPLUGIN_CAPPLICATIONSPLUGIN_EXIT );
    }

// -----------------------------------------------------------------------------
void CApplicationsPlugin::ConstructL()
    {
    OstTraceFunctionEntry0( CAPPLICATIONSPLUGIN_CONSTRUCTL_ENTRY );
	iIndexState = ETrue;
    iAsynchronizer = CDelayedCallback::NewL(CActive::EPriorityIdle);
    iNotifier = CApaAppListNotifier::NewL(this, CActive::EPriorityHigh);
    //iHiddenApplicationsRepository = CRepository::NewL( KHiddenAppRepositoryUid );
    //User::LeaveIfError( iWidgetRegistry.Connect() );
    OstTraceFunctionExit0( CAPPLICATIONSPLUGIN_CONSTRUCTL_EXIT );
    }

// -----------------------------------------------------------------------------
void CApplicationsPlugin::StartPluginL()
    {
    OstTraceFunctionEntry0( CAPPLICATIONSPLUGIN_STARTPLUGINL_ENTRY );
    TRAPD(error,iScrSession.Connect());
    if (error != KErrNone)
        return;

    User::LeaveIfError(iSearchSession.DefineVolume(
            _L(APPLICATIONS_QBASEAPPCLASS), KNullDesC));

    // Open database
    iIndexer = CCPixIndexer::NewL(iSearchSession);
    iIndexer->OpenDatabaseL(_L(APPLICATIONS_QBASEAPPCLASS));

    // Start harvester for this plugin
    iObserver->AddHarvestingQueue(this, iIndexer->GetBaseAppClass());
    OstTraceFunctionExit0( CAPPLICATIONSPLUGIN_STARTPLUGINL_EXIT );
    }

// -----------------------------------------------------------------------------
void CApplicationsPlugin::StartHarvestingL(const TDesC& /* aQualifiedBaseAppClass */)
    {
    OstTraceFunctionEntry0( CAPPLICATIONSPLUGIN_STARTHARVESTINGL_ENTRY );

    // Harvest items on each call   
    iScrView.OpenViewL(iScrSession);
    iIndexer->ResetL();
    iHarvestState = EHarvesterStartHarvest;
    //No need to check IsStatred() since this is the first start. 
#ifdef __PERFORMANCE_DATA
    iStartTime.UniversalTime();
#endif
    iAsynchronizer->Start(0, this, KHarvestingDelay);
    OstTraceFunctionExit0( CAPPLICATIONSPLUGIN_STARTHARVESTINGL_EXIT );
    }

// -----------------------------------------------------------------------------
//This need not be a member function.
void AddApplicationInfoL(CSearchDocument* aDocument,
                         Usif::TAppRegInfo& aAppInfo)
    {
    OstTraceFunctionEntry0( _ADDAPPLICATIONINFOL_ENTRY );

    TBuf<KMaxFileName> docidString = aAppInfo.Uid().Name(); //This returns stuff in the form "[UID]". So remove the brackets.
    docidString = docidString.Mid(KUidStartIndex, KUidEndIndex);
    
    //We index the exe name (without extension), only if the title is not present.
    if (aAppInfo.ShortCaption().Compare(KNullDesC))
        {
        aDocument->AddFieldL(KApplicationFieldShortCaption,
                aAppInfo.ShortCaption(), CDocumentField::EStoreYes
                        | CDocumentField::EIndexTokenized
                        | CDocumentField::EIndexFreeText);
        
        if (aAppInfo.Caption().Compare(KNullDesC))
                {
                aDocument->AddFieldL(KApplicationFieldCaption,
                        aAppInfo.Caption(), CDocumentField::EStoreYes
                                | CDocumentField::EIndexTokenized
                                | CDocumentField::EIndexFreeText);
                }

        aDocument->AddHLDisplayFieldL(aAppInfo.ShortCaption());
        }
    else if (aAppInfo.Caption().Compare(KNullDesC))
        {
        aDocument->AddFieldL(KApplicationFieldShortCaption,
                aAppInfo.Caption(), CDocumentField::EStoreYes
                        | CDocumentField::EIndexTokenized
                        | CDocumentField::EIndexFreeText);

        aDocument->AddFieldL(KApplicationFieldCaption, aAppInfo.Caption(),
                CDocumentField::EStoreYes | CDocumentField::EIndexTokenized);
        }
    else
        {
        //Find the *last* location of '\' and remove the .exe to get just the filename.
        TInt location = aAppInfo.FullName().LocateReverse('\\');
        if (location > 0)
            {
            TInt lengthOfNameWithoutExtention = aAppInfo.FullName().Length()
                    - location - 1; //-1 to increment one past '\'.
            TPtrC appName = aAppInfo.FullName().Right(
                    lengthOfNameWithoutExtention);
            aDocument->AddFieldL(KApplicationFieldShortCaption, appName.Left(
                    appName.Length() - 4 /*remove ".exe"*/),
                    CDocumentField::EStoreYes
                            | CDocumentField::EIndexTokenized);

            aDocument->AddFieldL(KApplicationFieldCaption, KNullDesC,
                    CDocumentField::EStoreYes
                            | CDocumentField::EIndexTokenized);

            aDocument->AddHLDisplayFieldL(
                    appName.Left(appName.Length() - 4 /*remove ".exe"*/));
            }
        }

    if (aAppInfo.FullName().Compare(KNullDesC))
        {
        aDocument->AddFieldL(KApplicationFieldAbsolutePath,
                aAppInfo.FullName(), CDocumentField::EStoreYes
                        | CDocumentField::EIndexTokenized
                        | CDocumentField::EAggregateNo);

        }
    //For applications, no content to go into exceprt field, for more info, check the appclass-hierarchy.txt
    //aDocument->AddExcerptL( aAppInfo.iCaption );
    aDocument->AddExcerptL(KNullDesC);

    OstTraceExt2( TRACE_NORMAL, _ADDAPPLICATIONINFOL, "::AddApplicationInfoL;UID=%S;PATH=%S", &docidString, &aAppInfo.FullName() );
	OstTraceExt2( TRACE_NORMAL, DUP1__ADDAPPLICATIONINFOL, "::AddApplicationInfoL;Excerpt=%S;Caption=%S", &aAppInfo.Caption(), &aAppInfo.ShortCaption() );
    OstTraceFunctionExit0( _ADDAPPLICATIONINFOL_EXIT );
    }

// -----------------------------------------------------------------------------
TBool CApplicationsPlugin::IsAppHiddenL(TUid aUid)
    {
    //Application should not have 'hidden' capability.
    TBool ret( EFalse );    
    OstTrace1( TRACE_NORMAL, CAPPLICATIONSPLUGIN_ISAPPHIDDENL, "CApplicationsPlugin::IsAppHiddenL;UID=%d", aUid );
    CPIXLOGSTRING2("CApplicationsPlugin::IsAppHidden(): UID = %d", aUid );
    
    Usif::RRegistrationInfoForApplication appRegInfo;
    appRegInfo.OpenL(iScrSession, aUid);
    Usif::TApplicationCharacteristics appCharacteristics;
    appRegInfo.GetAppCharacteristicsL(appCharacteristics);
    ret = appCharacteristics.iAppIsHidden;
    appRegInfo.Close();
    
    OstTrace1( TRACE_NORMAL, DUP1_CAPPLICATIONSPLUGIN_ISAPPHIDDENL, "CApplicationsPlugin::IsAppHiddenL;Return Value=%d", &ret );

    CPIXLOGSTRING2("CApplicationsPlugin::IsAppHidden(): %d", &ret);
    return ret;
    }

// -----------------------------------------------------------------------------
void CApplicationsPlugin::CreateApplicationsIndexItemL(RPointerArray<
        Usif::TAppRegInfo>& aAppInfo, TCPixActionType /*aActionType*/)
    {
    //If application has 'hidden' capability, don't index.
    for (TInt i = 0; i < aAppInfo.Count(); i++)
        {
        if (!IsAppHiddenL(aAppInfo[i]->Uid()))
            {
            OstTrace0( TRACE_NORMAL, DUP2_CAPPLICATIONSPLUGIN_CREATEAPPLICATIONSINDEXITEML, "CApplicationsPlugin::Indexing Application" );
            
            TBuf<KMaxFileName> docidString;
            docidString.Append(aAppInfo[i]->Uid().Name()); //This returns descriptor in the form "[UID]". So remove the brackets.
            docidString = docidString.Mid(KUidStartIndex, KUidEndIndex);

            CSearchDocument* document = CSearchDocument::NewLC(docidString,
                                                _L(APPLICATIONS_APPCLASS));
            //The UID field should not be aggregated for now as we dont want it to be searchable by default.
            //By default, all tokenized fields are aggregated and therefore searchable.
            //If we dont tokenize, then the field will not be searchable at all.
            //As a middle path, we tokenize this field, but explicitly chose NOT to aggregate it.
            //That way, if a client is interested in the UID field, he can choose to query it explicitly.
            document->AddFieldL(KMimeTypeField, KMimeTypeApplication,
                    CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized);
            document->AddFieldL(KApplicationFieldUid, docidString,
                    CDocumentField::EStoreYes | CDocumentField::EIndexTokenized
                            | CDocumentField::EAggregateNo);
            
            TInt excerptLength = docidString.Length();
            HBufC* excerpt = HBufC::NewL(excerptLength);
            TPtr excerptPtr = excerpt->Des();
            CleanupStack::PushL(excerpt);
            document->AddExcerptL(excerptPtr);
            CleanupStack::PopAndDestroy(excerpt);
            
            /*if( iWidgetRegistry.IsWidget( aAppInfo.iUid  ) ) //Widget support
             AddWidgetInfoL( document, aAppInfo.iUid );
             else*/
            AddApplicationInfoL(document, *aAppInfo[i]);
            TRAPD( error, iIndexer->AddL( *document ) );
            if (KErrNone == error)
                {
                OstTrace0( TRACE_NORMAL, CAPPLICATIONSPLUGIN_CREATEAPPLICATIONSINDEXITEML, "CApplicationsPlugin::CreateApplicationsIndexItemL : No Error" );
                CPIXLOGSTRING("CApplicationsPlugin::CreateApplicationsIndexItemL(): No Error" );
                }
            else
                {
                OstTrace1( TRACE_NORMAL, DUP1_CAPPLICATIONSPLUGIN_CREATEAPPLICATIONSINDEXITEML, "CApplicationsPlugin::CreateApplicationsIndexItemL;Error=%d", error );
                CPIXLOGSTRING2("CApplicationsPlugin::CreateApplicationsIndexItemL(): Error = %d", error );
                }
            CleanupStack::PopAndDestroy(document);
            }        
        }
    }

// -----------------------------------------------------------------------------
void CApplicationsPlugin::DelayedCallbackL( TInt /*aCode*/ )
    {
    if( !iIndexState )
        return;
    
    RPointerArray<Usif::TAppRegInfo> appInfo;
    //const TInt error = iApplicationServerSession.GetNextApp(appInfo);
    iScrView.GetNextAppInfoL(KNumberOfAppInfoToBeRead, appInfo);
    if (appInfo.Count() > 0)
        {
        CreateApplicationsIndexItemL( appInfo, ECPixAddAction );
		iAsynchronizer->Start( 0, this, KHarvestingDelay );
        }
		else
        {
        OstTrace0( TRACE_NORMAL, CAPPLICATIONSPLUGIN_DELAYEDCALLBACKL, "CApplicationsPlugin::DelayedCallbackL:Flushing" );
        
        Flush(*iIndexer);
#ifdef __PERFORMANCE_DATA
    UpdatePerformaceDataL();
#endif
        iHarvestState = EHarvesterIdleState;
        iObserver->HarvestingCompleted( this, iIndexer->GetBaseAppClass(), KErrNone );
        }
    appInfo.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
void CApplicationsPlugin::DelayedError( TInt aCode )
    {
    Flush(*iIndexer);
    iHarvestState = EHarvesterIdleState;
    iObserver->HarvestingCompleted(this, iIndexer->GetBaseAppClass(), aCode);
    }
   
// -----------------------------------------------------------------------------
void CApplicationsPlugin::HandleAppListEvent( TInt aEvent )
    {
    OstTraceFunctionEntry0( CAPPLICATIONSPLUGIN_HANDLEAPPLISTEVENT_ENTRY );
    OstTrace1( TRACE_NORMAL, CAPPLICATIONSPLUGIN_HANDLEAPPLISTEVENT, "CApplicationsPlugin::HandleAppListEvent;Event=%d", aEvent );
    CPIXLOGSTRING2("CApplicationsPlugin::HandleAppListEvent: Start with Event = %d", aEvent );
    if( aEvent == EAppListChanged )
        {
        if( iAsynchronizer->CallbackPending() )
            {
            iAsynchronizer->CancelCallback(); //first cancel any ongoing harvesting.
            }
        TRAP_IGNORE( StartHarvestingL( KNullDesC ) ); //simply reharvest
        }
    CPIXLOGSTRING("CApplicationsPlugin::HandleAppListEvent: Exit" );
    OstTraceFunctionExit0( CAPPLICATIONSPLUGIN_HANDLEAPPLISTEVENT_EXIT );
    }

void CApplicationsPlugin::PausePluginL()
    {
    OstTraceFunctionEntry0( CAPPLICATIONSPLUGIN_PAUSEPLUGINL_ENTRY );
    iIndexState = EFalse;
    OstTraceFunctionExit0( CAPPLICATIONSPLUGIN_PAUSEPLUGINL_EXIT );
    }

void CApplicationsPlugin::ResumePluginL()
    {
    OstTraceFunctionEntry0( CAPPLICATIONSPLUGIN_RESUMEPLUGINL_ENTRY );
    iIndexState = ETrue;
    if(iHarvestState == EHarvesterStartHarvest)
        {
        if(iAsynchronizer->CallbackPending())
            iAsynchronizer->CancelCallback();
        iAsynchronizer->Start( 0, this, KHarvestingDelay );
        }
    OstTraceFunctionExit0( CAPPLICATIONSPLUGIN_RESUMEPLUGINL_EXIT );
    }

#ifdef __PERFORMANCE_DATA
void CApplicationsPlugin::UpdatePerformaceDataL()
    {
    TTime now;
   
    
    iCompleteTime.UniversalTime();
    TTimeIntervalMicroSeconds timeDiff = iCompleteTime.MicroSecondsFrom(iStartTime);
    
    RFs fileSession;
    RFile perfFile;
    User::LeaveIfError( fileSession.Connect () );
    
    
    /* Open file if it exists, otherwise create it and write content in it */
    
        if(perfFile.Open(fileSession, _L("c:\\data\\ApplicationsPerf.txt"), EFileWrite))
                   User::LeaveIfError(perfFile.Create (fileSession, _L("c:\\data\\ApplicationsPerf.txt"), EFileWrite));
    
    HBufC8 *heap = HBufC8::NewL(100);
    TPtr8 ptr = heap->Des();
    now.HomeTime();
    TBuf<50> timeString;             
                
    _LIT(KOwnTimeFormat,"%:0%H%:1%T%:2%S");
    now.FormatL(timeString,KOwnTimeFormat);
    ptr.AppendNum(now.DateTime().Day());
    ptr.Append(_L("/"));
    ptr.AppendNum(now.DateTime().Month());
    ptr.Append(_L("/"));
    ptr.AppendNum(now.DateTime().Year());
    ptr.Append(_L(":"));
    ptr.Append(timeString);
    ptr.Append( _L(": Ani: Time took for Harvesting Applications is : "));
    ptr.AppendNum(timeDiff.Int64()/1000) ;
    ptr.Append(_L(" MilliSeonds \n"));
    TInt myInt = 0;
    perfFile.Seek(ESeekEnd,myInt);
    perfFile.Write (ptr);
    perfFile.Close ();
    fileSession.Close ();
    delete heap;
    }
#endif

// End of file
