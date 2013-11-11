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


#include <e32base.h> 
#include <ccpixindexer.h>
#include <csearchdocument.h>
#include <QtGlobal>
#include "cemailplugin.h"
#include "harvesterserverlogger.h"
#include "qtemailfetcher.h"

// maximum length that the fully qualified msg Plugin base app class descriptor can be
// e.g. "@c:root msg email"
const TInt KMsgPluginBaseAppClassMaxLen = 64;


// local declarations and functions
namespace {

_LIT(KCPixSearchServerPrivateDirectory, "\\Private\\2001f6f7\\");
_LIT(KPathIndexDbPath, CPIX_INDEVICE_INDEXDB);

_LIT(KPathTrailer, "\\root\\msg\\email");
_LIT(KMsgBaseAppClassGeneric, ":root msg email");
_LIT(KAtSign, "@");
_LIT(KColon, ":");

/**
* MapFileToDrive - gets the TDriveNumber associated with the aBaseAppClass.
* @aBaseAppClass e.g. "@c:root file"
* @aDrive returns the TDriveNumber for the aBaseAppClass
* returns KErrNone on success or a standard error code
*/
TInt MapBaseAppClassToDrive(const TDesC& aBaseAppClass, TDriveNumber& aDrive)
    {
    if (KErrNone != aBaseAppClass.Left(1).Compare(KAtSign))
        {
        return KErrGeneral;
        }

    TPtrC drvChar = aBaseAppClass.Left(2).Right(1);
    TChar d(drvChar[0]);
    TInt drive;
    const TInt ret = RFs::CharToDrive(d, drive);
    if (!ret)
        {
        aDrive = TDriveNumber(drive);
        }

    return ret;
    }

}// anonymous namespace


// ---------------------------------------------------------------------------
// CEmailPlugin::NewL
// ---------------------------------------------------------------------------
//  
CEmailPlugin* CEmailPlugin::NewL()
{
    CPIXLOGSTRING("CEmailPlugin::NewL : Start");
	  CEmailPlugin* instance = CEmailPlugin::NewLC();
    CleanupStack::Pop(instance);
    CPIXLOGSTRING("CEmailPlugin::NewL : End");
    return instance;
}

// ---------------------------------------------------------------------------
// CEmailPlugin::NewLC
// ---------------------------------------------------------------------------
//  
CEmailPlugin* CEmailPlugin::NewLC()
{
    CPIXLOGSTRING("CEmailPlugin::NewLC : Start");
	CEmailPlugin* instance = new (ELeave)CEmailPlugin();
    CleanupStack::PushL(instance);
    instance->ConstructL();
    CPIXLOGSTRING("CEmailPlugin::NewLC : End");
    return instance;
}

// ---------------------------------------------------------------------------
// CEmailPlugin::CEmailPlugin
// ---------------------------------------------------------------------------
//  
CEmailPlugin::CEmailPlugin()
{
	
}

// ---------------------------------------------------------------------------
// CEmailPlugin::~CEmailPlugin
// ---------------------------------------------------------------------------
//  
CEmailPlugin::~CEmailPlugin()
{
    // remove notification paths before destroying iMonitor
    for (TInt i=EDriveA; i<=EDriveZ; i++)
        UnMount(TDriveNumber(i), EFalse);
	  iFs.Close();
	  delete iQEmailFetcher;  
}

// ---------------------------------------------------------------------------
// CEmailPlugin::ConstructL
// ---------------------------------------------------------------------------
//  
void CEmailPlugin::ConstructL()
{
	TInt err = iFs.Connect();
    CPIXLOGSTRING2("CEmailPlugin::ConstructL, iFs.Connect: %i", err);
    User::LeaveIfError(err);
    iCurrentDrive = EDriveC; //Default drive is C drive
    for (TInt i=EDriveA; i<=EDriveZ; i++)
        {
        iIndexer[i] = NULL; //Initialize to NULL
        }
}

// ---------------------------------------------------------------------------
// CEmailPlugin::StartPluginL
// ---------------------------------------------------------------------------
//  
void CEmailPlugin::StartPluginL()
	{
    CPIXLOGSTRING("CEmailPlugin::StartPluginL : Start");
	CPIXLOGSTRING2("currentDrive used is : %d", iCurrentDrive );
	MountL(TDriveNumber(iCurrentDrive)); //Mount current drive
	//create instance of QEmailFetcher
	QT_TRYCATCH_LEAVING(iQEmailFetcher = QEmailFetcher::newInstance(*this));
	// Define this base application class, use default location
	CPIXLOGSTRING("CEmailPlugin::StartPluginL : End");
	}

// ---------------------------------------------------------------------------
// CEmailPlugin::StartHarvestingL
// ---------------------------------------------------------------------------
//  
void CEmailPlugin::StartHarvestingL(const TDesC& aQualifiedBaseAppClass)
	{
	CPIXLOGSTRING("START CEmailPlugin::StartHarvestingL");
	// Map base app class to a drive number
	TDriveNumber drive ( EDriveA );//Initialize to silence compiler warnings.
	if (KErrNone != MapBaseAppClassToDrive(aQualifiedBaseAppClass, drive))
	   User::Leave(KErrGeneral);

    // Leave if no indexer for this drive
	if (!iIndexer[drive])
	   User::Leave(KErrGeneral);
	
	// Reset the database
	iIndexer[drive]->ResetL();
	iQEmailFetcher->StartHarvesting();
	
#ifdef __PERFORMANCE_DATA
    iStartTime.UniversalTime();
#endif
	}

// ---------------------------------------------------------------------------
// CEmailPlugin::HandleDocumentL
// ---------------------------------------------------------------------------
//
void CEmailPlugin::HandleDocumentL(const CSearchDocument* aSearchDocument, 
	                               TCPixActionType aActionType)
  {
  CPIXLOGSTRING("START CEmailPlugin::HandleDocumentL");
  // Index an empty item if removal action
  if (aActionType == ECPixRemoveAction)
      {
      if (GetIndexer())
          {
          TRAPD(err, GetIndexer()->DeleteL(aSearchDocument->Id()));
          if (err == KErrNone)
              {
              CPIXLOGSTRING("CEmailPlugin::HandleDocumentL: Document deleted.");
              }
          else
              {
              CPIXLOGSTRING2("CEmailPlugin::HandleDocumentL: Error %d in deleting the document.", err);              
              }
          }
      return;
      }
  //Add or update action do accordingly
  if (GetIndexer())
      {
      if (aActionType == ECPixAddAction)
          {
          TRAPD(err, GetIndexer()->AddL(*aSearchDocument));
          if (err == KErrNone)
              {
              CPIXLOGSTRING("CEmailPlugin::HandleDocumentL(): Added.");
              }
          else
              {
              CPIXLOGSTRING2("CEmailPlugin::HandleDocumentL(): Error %d in adding.", err);
              }
          }
      else if (aActionType == ECPixUpdateAction)
          {
          TRAPD(err, GetIndexer()->UpdateL(*aSearchDocument));
          if (err == KErrNone)
              {
              CPIXLOGSTRING("CEmailPlugin::HandleDocumentL(): Updated.");
              }
          else
              {
              CPIXLOGSTRING2("CEmailPlugin::HandleDocumentL(): Error %d in updating.", err);
              }
          }
      }
  else
      {
      CPIXLOGSTRING("END CEmailPlugin::HandleDocumentL(): No indexer present.");
      }
  }
  
// ---------------------------------------------------------------------------
// CEmailPlugin::MountL
// ---------------------------------------------------------------------------
//
void CEmailPlugin::MountL(TDriveNumber aMedia,TBool aForceReharvesting)
    {
    CPIXLOGSTRING("START CEmailPlugin::MountL");
    // Check if already exists
    if (iIndexer[aMedia])
        return;
    
    // Form the baseappclass for this media
    TBuf<KMsgPluginBaseAppClassMaxLen> baseAppClass;
    FormBaseAppClass(aMedia, baseAppClass);

    // Define this volume
    HBufC* path = DatabasePathLC(aMedia);
    User::LeaveIfError(iSearchSession.DefineVolume(baseAppClass, *path));
    CleanupStack::PopAndDestroy(path);
    
    // construct and open the database
    TRAPD(err,iIndexer[aMedia] = CCPixIndexer::NewL(iSearchSession));
    CPIXLOGSTRING2("CCPixIndexer::NewL returned : %d", err );
    TRAP(err,iIndexer[aMedia]->OpenDatabaseL(baseAppClass));

    // Add to harvesting queue
    iObserver->AddHarvestingQueue(this, baseAppClass,aForceReharvesting);  
    CPIXLOGSTRING("END CEmailPlugin::MountL");
    }

// ---------------------------------------------------------------------------
// CEmailPlugin::UnMount
// ---------------------------------------------------------------------------
//
void CEmailPlugin::UnMount(TDriveNumber aMedia, TBool aUndefineAsWell)
    {
    CPIXLOGSTRING("START CEmailPlugin::UnMount");
    // Check if already exists
    if (!iIndexer[aMedia])
        return;
    
    // Form the baseappclass for this media
    TBuf<KMsgPluginBaseAppClassMaxLen> baseAppClass;
    FormBaseAppClass(aMedia, baseAppClass);             
        
    // Remove from harvesting queue
    iObserver->RemoveHarvestingQueue(this, baseAppClass);   
    
    // Delete the index object
    if (iIndexer[aMedia])
        {
        delete iIndexer[aMedia];
        iIndexer[aMedia] = NULL;
        }
    
    // if the aActionType is EFFMmcDismount, then the
    // parameter aFilename is the baseAppClass of the Index database
    // to be dropped.
    if (aUndefineAsWell)
        iSearchSession.UnDefineVolume(baseAppClass);
    }


// ---------------------------------------------------------------------------
// CEmailPlugin::FormBaseAppClass
// ---------------------------------------------------------------------------
//  
TInt CEmailPlugin::FormBaseAppClass(TDriveNumber aMedia, TDes& aBaseAppClass)
    {
    TChar chr;
    const TInt ret = RFs::DriveToChar(aMedia, chr);
    if (KErrNone == ret)
        {
        aBaseAppClass.Copy(KAtSign);
        aBaseAppClass.Append(chr);
        aBaseAppClass.LowerCase();
        aBaseAppClass.Append(KMsgBaseAppClassGeneric);
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CEmailPlugin::DatabasePathLC
// ---------------------------------------------------------------------------
//
HBufC* CEmailPlugin::DatabasePathLC(TDriveNumber aMedia)
    {
    CPIXLOGSTRING("START CEmailPlugin::DatabasePathLC");
    // Allocate extra space for root path e.g. "C:\\Private\\2001f6f7\\"
    const TInt KRootPathMaxLength = 30;
    HBufC* indexDbPath = HBufC::NewLC(KRootPathMaxLength + KPathIndexDbPath().Length() + KPathTrailer().Length());
    TPtr indexDbPathPtr = indexDbPath->Des();

#if 1 // Data caging implementation
    iFs.CreatePrivatePath(aMedia);

    TChar chr;
    RFs::DriveToChar(aMedia, chr);
    indexDbPathPtr.Append(chr);
    indexDbPathPtr.Append(KColon);

    TFileName pathWithoutDrive;
    iFs.PrivatePath(pathWithoutDrive);
    indexDbPathPtr.Append(KCPixSearchServerPrivateDirectory);
#else // here is the way to calculate the path if data caging is not being used.
    TFileName rootPath;
    PathInfo::GetRootPath(rootPath, aMedia);
    indexDbPathPtr.Append(rootPath);
#endif 

    indexDbPathPtr.Append(KPathIndexDbPath);
    indexDbPathPtr.Append(KPathTrailer);

    return indexDbPath;
    }

// ---------------------------------------------------------------------------
// CEmailPlugin::GetIndexer
// ---------------------------------------------------------------------------
//
CCPixIndexer* CEmailPlugin::GetIndexer()
    {
    return iIndexer[iCurrentDrive];
    }

// ---------------------------------------------------------------------------
// CEmailPlugin::HarvestingCompleted callback from email fetcher
// ---------------------------------------------------------------------------
//
void CEmailPlugin::HarvestingCompleted()
    {
    HarvestingCompleted(KErrNone);
    }

// ---------------------------------------------------------------------------
// CEmailPlugin::HarvestingCompleted
// ---------------------------------------------------------------------------
//  
void CEmailPlugin::HarvestingCompleted(TInt aError)
{
	// Notifies the indexing manager of completed harvesting, called by CMessageHarvester
    Flush( *GetIndexer() );
    TBuf<KMsgPluginBaseAppClassMaxLen> baseAppClass;
    FormBaseAppClass(TDriveNumber(iCurrentDrive), baseAppClass);
#ifdef __PERFORMANCE_DATA
    TRAP_IGNORE( UpdatePerformaceDataL() );
#endif
    iObserver->HarvestingCompleted(this, baseAppClass, aError);
}

void CEmailPlugin::PausePluginL()
    {
    
    }

void CEmailPlugin::ResumePluginL()
    {
    
    }

// ---------------------------------------------------------------------------
// CEmailPlugin::UpdatePerformaceDataL
// ---------------------------------------------------------------------------
//
#ifdef __PERFORMANCE_DATA
void CEmailPlugin::UpdatePerformaceDataL()
    {
    TTime now;
   
    
    iCompleteTime.UniversalTime();
    TTimeIntervalMicroSeconds timeDiff = iCompleteTime.MicroSecondsFrom(iStartTime);
    
    RFs fileSession;
    RFile perfFile;
    User::LeaveIfError( fileSession.Connect () );
    
    
    /* Open file if it exists, otherwise create it and write content in it */
    
        if(perfFile.Open(fileSession, _L("c:\\data\\MessagePerf.txt"), EFileWrite))
                   User::LeaveIfError(perfFile.Create (fileSession, _L("c:\\data\\MessagePerf.txt"), EFileWrite));
    
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
    ptr.Append( _L("Time taken for Harvesting Message is : "));
    ptr.AppendNum(timeDiff.Int64()/1000) ;
    ptr.Append(_L(" MilliSeonds \n"));
    TInt myInt = 0;
    perfFile.Seek(ESeekEnd,myInt);
    perfFile.Write (ptr);
    perfFile.Close ();
    fileSession.Close ();
    delete heap;
    }

// ---------------------------------------------------------------------------
// CEmailPlugin::UpdatePerformaceDataL
// ---------------------------------------------------------------------------
//
void CEmailPlugin::UpdatePerformaceDataL(TMsvSessionEvent action) {
    
        iCompleteTime.UniversalTime();
        TTimeIntervalMicroSeconds timeDiff = iCompleteTime.MicroSecondsFrom(iStartTime);
        
        RFs fileSession;
        RFile perfFile;
        User::LeaveIfError( fileSession.Connect () );
        
        
        /* Open file if it exists, otherwise create it and write content in it */
        
            if(perfFile.Open(fileSession, _L("c:\\data\\MessagePerf.txt"), EFileWrite))
                       User::LeaveIfError(perfFile.Create (fileSession, _L("c:\\data\\MessagePerf.txt"), EFileWrite));
        
        HBufC8 *heap = HBufC8::NewL(100);
        TPtr8 ptr = heap->Des();

        switch (action) {
            case EMsvEntriesDeleted: ptr.Append( _L("del "));break;
            case EMsvEntriesChanged: ptr.Append( _L("upd "));break;
            case EMsvEntriesMoved: ptr.Append( _L("mov "));break;
        }
        ptr.AppendNum(timeDiff.Int64()/1000) ;
        ptr.Append(_L("\n"));
        TInt myInt = 0;
        perfFile.Seek(ESeekEnd,myInt);
        perfFile.Write (ptr);
        perfFile.Close ();
        fileSession.Close ();
        delete heap;
    }

#endif
