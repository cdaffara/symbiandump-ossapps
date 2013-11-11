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



#include <aknlists.h> 
#include <AknsSkinInstance.h>
#include <data_caging_path_literals.hrh>

#include "engine.h"
#include "enginewrapper.h"
#include "creator_traces.h"
#include "creator_factory.h"
#include "creator_scriptentry.h"



const TInt KReadBufSize = 8192;

// Creator random data file filter
_LIT(KRandomDataFileFilter, "*.creatordataxml");

// ---------------------------------------------------------------------------


CCommandParser* CCommandParser::NewL(CCreatorEngine* aEngine)
    {
    CCommandParser* self = CCommandParser::NewLC(aEngine);
    CleanupStack::Pop(self);
    return self;
    }

CCommandParser* CCommandParser::NewLC(CCreatorEngine* aEngine)
    {
    CCommandParser* self = new (ELeave) CCommandParser;
    CleanupStack::PushL(self);
    self->ConstructL(aEngine);
    return self;
    }

CCommandParser::CCommandParser()
	{
	}

void CCommandParser::ConstructL(CCreatorEngine* aEngine)
	{
    LOGSTRING("Creator: CCommandParser::ConstructL");

    iEngine = aEngine;

    iReadBuf = HBufC8::NewL(KReadBufSize);
	}

CCommandParser::~CCommandParser()
	{
    LOGSTRING("Creator: CCommandParser::~CCommandParser");

    if (iSearchArray){
        delete iSearchArray;
        iSearchArray = NULL;
    }

    if (iReadBuf)
        delete iReadBuf;
	}

// ---------------------------------------------------------------------------

void CCommandParser::QueryDialogClosedL(TBool aPositiveAction, TInt aUserData)
    {
    User::LeaveIfNull(iSearchArray);
    
    if( aUserData == EGetingScript )
        {
        if(aPositiveAction)
            {
            iObserver->ScriptChosenL( ETrue, iSearchArray->MdcaPoint(iSelectedItem) );
            }
        else
            {
            iObserver->ScriptChosenL( EFalse );
            }
        }
    else if( aUserData == EGetingRandomDataFile && iSearchArray->Count() )
        {
        if(aPositiveAction)
            {
            TFileName fileName;
            if (iSelectedItem == (iSearchArray->Count() - 1))
                {
                // "default" (resource file) selected
                fileName.Copy(KNullDesC);
                }
            else
                {
                // xml file selected
                fileName.Copy(iSearchArray->MdcaPoint(iSelectedItem));
                }
            iObserver->RandomDataFileChosenL( ETrue, fileName );
            }
        else
            {
            iObserver->RandomDataFileChosenL( EFalse );
            }
        }
    }

// ---------------------------------------------------------------------------
//#if(!defined __SERIES60_30__ && !defined __SERIES60_31__)
void CCommandParser::OpenScriptL() {}
TBool CCommandParser::OpenScriptL(MCommandParserObserver* aObserver)
    {    
    LOGSTRING("Creator: CCommandParser::OpenScriptL");
    
    User::LeaveIfNull( aObserver );
    iObserver = aObserver;
    iSelectedItem = 0;
    
    TBool ret = EFalse;

    // init the search array
    delete iSearchArray;
    iSearchArray = NULL;
    iSearchArray = new(ELeave) CDesCArrayFlat(20);

    // wait dialog
	// TODO
    //CAknGlobalNote* waitDialog = CAknGlobalNote::NewLC();
    //waitDialog->SetSoftkeys(R_AVKON_SOFTKEYS_CANCEL);
    //TInt dialogId = waitDialog->ShowNoteL(EAknGlobalWaitNote, _L("Searching"));


    // search all the .creator files from the phone
    TInt err;
    TRAP(err, FindFiles(_L("*.creatorxml"), _L("\\")));

    TRAP(err, FindFiles(_L("*.creatorxml"), _L("\\system\\apps\\Creator\\")));

    TRAP(err, FindFiles(_L("*.creatorxml"), _L("\\data\\")));
    TRAP(err, FindFilesRecursiveL(_L("*.creatorxml"), _L("\\data\\")));
    
    TRAP(err, FindFiles(_L("*.creatorxml"), _L("\\documents\\")));
    TRAP(err, FindFilesRecursiveL(_L("*.creatorxml"), _L("\\documents\\")));

    TRAP(err, FindFiles(_L("*.creatorxml"), _L("\\creator\\")));
    TRAP(err, FindFilesRecursiveL(_L("*.creatorxml"), _L("\\creator\\")));
    
    TRAP(err, FindFiles(_L("*.creatorxml"), _L("\\scripts\\")));
    TRAP(err, FindFilesRecursiveL(_L("*.creatorxml"), _L("\\scripts\\")));
    
    TRAP(err, FindFiles(_L("*.creatorxml"), _L("\\temp\\")));
    TRAP(err, FindFilesRecursiveL(_L("*.creatorxml"), _L("\\temp\\")));
    
    TRAP(err, FindFiles(_L("*.creatorxml"), _L("\\system\\temp\\")));
    TRAP(err, FindFilesRecursiveL(_L("*.creatorxml"), _L("\\system\\temp\\")));

    TRAP(err, FindFiles(_L("*.creatorxml"), _L("\\private\\20011383\\")));
    TRAP(err, FindFilesRecursiveL(_L("*.creatorxml"), _L("\\private\\20011383\\")));

    TRAP(err, FindFilesRecursiveL(_L("*.creatorxml"), _L("\\private\\1000484b\\")));


    // remove the wait dialog
	// TODO
    //waitDialog->CancelNoteL(dialogId);
    //CleanupStack::PopAndDestroy(); //waitDialog;


    if ( iSearchArray->MdcaCount() > 0)
        {
        // create a file name array based on the iSearchArray
        CDesCArray* fileNameArray = new(ELeave) CDesCArrayFlat(iSearchArray->MdcaCount());
        CleanupStack::PushL(fileNameArray);
        
        for (TInt i=0; i<iSearchArray->MdcaCount(); i++)
            {
            
            TParse filename; // file name parser
            filename.Set(iSearchArray->MdcaPoint(i), NULL, NULL);

            if ( filename.Drive()[0] == 'e' || filename.Drive()[0] == 'E')  // MMC
                {
                TFileName nameMMC;
                nameMMC.Append(filename.Name());
                nameMMC.Append(_L("\t0"));        // MMC icon
                fileNameArray->AppendL(nameMMC);
                }
            else
                {
                fileNameArray->AppendL(filename.Name());
                }
            }

        ret = iEngine->GetEngineWrapper()->PopupListDialog(_L("Select script"), fileNameArray, &iSelectedItem, this, EGetingScript);
		CleanupStack::PopAndDestroy(fileNameArray);
        }
    else  // no scripts found from the search paths
        {
        iEngine->GetEngineWrapper()->ShowNote(_L("No scripts found"));
        delete iSearchArray;
        iSearchArray = NULL;
        }

    
    return ret;
    }
/*
#else
// ---------------------------------------------------------------------------
void CCommandParser::OpenScriptL()
    {
    LOGSTRING("Creator: CCommandParser::OpenScriptL");

    // init the search array
    iSearchArray = new(ELeave) CDesCArrayFlat(500);

    // wait dialog
    CAknGlobalNote* waitDialog = CAknGlobalNote::NewLC();
    waitDialog->SetSoftkeys(R_AVKON_SOFTKEYS_CANCEL);
    TInt dialogId = waitDialog->ShowNoteL(EAknGlobalWaitNote, _L("Searching"));


    // search all the .creator files from the phone
    TInt err;
    TRAP(err, FindFiles(_L("*.creator"), _L("\\")));

    TRAP(err, FindFiles(_L("*.creator"), _L("\\system\\apps\\Creator\\")));

    TRAP(err, FindFiles(_L("*.creator"), _L("\\data\\")));
    TRAP(err, FindFilesRecursiveL(_L("*.creator"), _L("\\data\\")));
    
    TRAP(err, FindFiles(_L("*.creator"), _L("\\documents\\")));
    TRAP(err, FindFilesRecursiveL(_L("*.creator"), _L("\\documents\\")));

    TRAP(err, FindFiles(_L("*.creator"), _L("\\creator\\")));
    TRAP(err, FindFilesRecursiveL(_L("*.creator"), _L("\\creator\\")));
    
    TRAP(err, FindFiles(_L("*.creator"), _L("\\scripts\\")));
    TRAP(err, FindFilesRecursiveL(_L("*.creator"), _L("\\scripts\\")));
    
    TRAP(err, FindFiles(_L("*.creator"), _L("\\temp\\")));
    TRAP(err, FindFilesRecursiveL(_L("*.creator"), _L("\\temp\\")));
    
    TRAP(err, FindFiles(_L("*.creator"), _L("\\system\\temp\\")));
    TRAP(err, FindFilesRecursiveL(_L("*.creator"), _L("\\system\\temp\\")));

    TRAP(err, FindFiles(_L("*.creator"), _L("\\private\\101FA751\\")));
    TRAP(err, FindFilesRecursiveL(_L("*.creator"), _L("\\private\\101FA751\\")));

    TRAP(err, FindFilesRecursiveL(_L("*.creator"), _L("\\private\\1000484b\\")));


    // remove the wait dialog
    waitDialog->CancelNoteL(dialogId);
    CleanupStack::PopAndDestroy(); //waitDialog;


    if ( iSearchArray->MdcaCount() > 0)
        {
        // create a file name array based on the iSearchArray
        CDesCArray* fileNameArray = new(ELeave) CDesCArrayFlat(iSearchArray->MdcaCount());
        CleanupStack::PushL(fileNameArray);
        
        for (TInt i=0; i<iSearchArray->MdcaCount(); i++)
            {
            
            TParse filename; // file name parser
            filename.Set(iSearchArray->MdcaPoint(i), NULL, NULL);

            if ( filename.Drive()[0] == 'e' || filename.Drive()[0] == 'E')  // MMC
                {
                TFileName nameMMC;
                nameMMC.Append(filename.Name());
                nameMMC.Append(_L("\t0"));        // MMC icon
                fileNameArray->AppendL(nameMMC);
                }
            else
                {
                fileNameArray->AppendL(filename.Name());
                }
            }
        CleanupStack::Pop(fileNameArray);


        // create a popup list
        CAknSinglePopupMenuStyleListBox* listBox = new(ELeave) CAknSinglePopupMenuStyleListBox;
        CleanupStack::PushL(listBox);
        CAknPopupList* popupList = CAknPopupList::NewL( listBox, R_AVKON_SOFTKEYS_SELECT_CANCEL, AknPopupLayouts::EMenuWindow );
        CleanupStack::PushL(popupList);
        popupList->SetTitleL(_L("Select script"));
        listBox->ConstructL( popupList, EAknListBoxSelectionList|EAknListBoxLoopScrolling );
        listBox->CreateScrollBarFrameL( ETrue );
        listBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
        listBox->Model()->SetItemTextArray( fileNameArray );
        listBox->Model()->SetOwnershipType( ELbmOwnsItemArray );  // !!!
        listBox->HandleItemAdditionL();


        // define MMC icon 
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        _LIT( KFLDMemoryCardUiBitmapFile, "z:aknmemorycardui.mbm" );
        CAknIconArray* iconArray = new( ELeave ) CAknIconArray( 1 );
        listBox->ItemDrawer()->ColumnData()->SetIconArray( iconArray );

        TParse* fp = new(ELeave) TParse();
        fp->Set(KFLDMemoryCardUiBitmapFile, &KDC_APP_BITMAP_DIR, NULL);
        TFileName resourceFileName( fp->FullName() );
        delete fp;

        CGulIcon* icon = AknsUtils::CreateGulIconL( skin, KAknsIIDQgnIndiMmcAdd,
            resourceFileName,
            EMbmAknmemorycarduiQgn_indi_mmc_add,
            EMbmAknmemorycarduiQgn_indi_mmc_add_mask );
        CleanupStack::PushL( icon );
        iconArray->AppendL( icon );
        CleanupStack::Pop( icon );


        // execute dialog
        TBool result = popupList->ExecuteLD();
        CleanupStack::Pop();  // popupList

        if ( result ) 
            {
            // open the file for reading
            RFile file;
            User::LeaveIfError(file.Open(CEikonEnv::Static()->FsSession(), iSearchArray->MdcaPoint(listBox->CurrentItemIndex()), EFileRead));
            while (ReadLineFromFileL(file) == KErrNone)
                {
                iReadBuf->Des().Trim();  // delete leading and trailing white space

                if (iReadBuf->Length() < 5)  // command too short, ignoring...
                    {
                    //CEikonEnv::Static()->InfoMsg(_L("Corrupted line"));
                    }
        
                // check it's a valid line: first char is A-Z and the line ends to ';'
                else if (iReadBuf->Des()[0] >= 'A' && iReadBuf->Des()[0] <= 'Z' && iReadBuf->Des()[iReadBuf->Length()-1] == ';')
                    {
                    // this line is valid -> parse the command
                    ParseCommandFromDescriptorL();
                    }
                else
                    {
                    //CEikonEnv::Static()->InfoMsg(_L("Not a valid line"));
                    }

                }

            file.Close();

            // show a note if there were no valid commands in the script
            if (iEngine->CommandArrayCount() == 0)
                {
                CAknInformationNote* note = new(ELeave) CAknInformationNote;
                note->ExecuteLD(_L("No valid commands in this script"));
                }

            }

        CleanupStack::PopAndDestroy();  //listBox
        }
    
    else  // no scripts found from the search paths
        {
        CAknInformationNote* note = new (ELeave) CAknInformationNote;
        note->ExecuteLD(_L("No scripts found"));
        }


    delete iSearchArray;
    iSearchArray = NULL;

    }
#endif
*/

TBool CCommandParser::GetRandomDataFilenameL(MCommandParserObserver *aObserver)
    {    
    LOGSTRING("Creator: CCommandParser::GetRandomDataFilenameL");
    TBool ret = EFalse;

    User::LeaveIfNull( aObserver );
    iObserver = aObserver;
    iSelectedItem = 0;

    // init the search array
    if (iSearchArray)
    	{
    	delete iSearchArray;
    	iSearchArray = NULL;
    	}
    iSearchArray = new(ELeave) CDesCArrayFlat(500);

	/* TODO:
    // wait dialog
    CAknGlobalNote* waitDialog = CAknGlobalNote::NewLC();
    waitDialog->SetSoftkeys(R_AVKON_SOFTKEYS_CANCEL);
    TInt dialogId = waitDialog->ShowNoteL(EAknGlobalWaitNote, _L("Searching"));
	*/


    // search all the creator random data files from the phone
    TInt err;
    TRAP(err, FindFiles(KRandomDataFileFilter, _L("\\")));

    TRAP(err, FindFiles(KRandomDataFileFilter, _L("\\system\\apps\\Creator\\")));

    TRAP(err, FindFiles(KRandomDataFileFilter, _L("\\data\\")));
    TRAP(err, FindFilesRecursiveL(KRandomDataFileFilter, _L("\\data\\")));
    
    TRAP(err, FindFiles(KRandomDataFileFilter, _L("\\documents\\")));
    TRAP(err, FindFilesRecursiveL(KRandomDataFileFilter, _L("\\documents\\")));

    TRAP(err, FindFiles(KRandomDataFileFilter, _L("\\creator\\")));
    TRAP(err, FindFilesRecursiveL(KRandomDataFileFilter, _L("\\creator\\")));
    
    TRAP(err, FindFiles(KRandomDataFileFilter, _L("\\scripts\\")));
    TRAP(err, FindFilesRecursiveL(KRandomDataFileFilter, _L("\\scripts\\")));
    
    TRAP(err, FindFiles(KRandomDataFileFilter, _L("\\temp\\")));
    TRAP(err, FindFilesRecursiveL(KRandomDataFileFilter, _L("\\temp\\")));
    
    TRAP(err, FindFiles(KRandomDataFileFilter, _L("\\system\\temp\\")));
    TRAP(err, FindFilesRecursiveL(KRandomDataFileFilter, _L("\\system\\temp\\")));

    TRAP(err, FindFiles(KRandomDataFileFilter, _L("\\private\\20011383\\")));
    TRAP(err, FindFilesRecursiveL(KRandomDataFileFilter, _L("\\private\\20011383\\")));

    TRAP(err, FindFilesRecursiveL(KRandomDataFileFilter, _L("\\private\\1000484b\\")));


    // remove the wait dialog
	// TODO
    //waitDialog->CancelNoteL(dialogId);
    //CleanupStack::PopAndDestroy(); //waitDialog;


    if (iSearchArray->MdcaCount() > 0)
        {
        // create a file name array based on the iSearchArray
        CDesCArray* fileNameArray = new(ELeave) CDesCArrayFlat(iSearchArray->MdcaCount() + 1);
        CleanupStack::PushL(fileNameArray);

        for (TInt i=0; i<iSearchArray->MdcaCount(); i++)
            {
            
            TParse filename; // file name parser
            filename.Set(iSearchArray->MdcaPoint(i), NULL, NULL);

            if ( filename.Drive()[0] == 'e' || filename.Drive()[0] == 'E')  // MMC
                {
                TFileName nameMMC;
                nameMMC.Append(filename.Name());
                nameMMC.Append(_L("\t0"));        // MMC icon
                fileNameArray->AppendL(nameMMC);
                }
            else
                {
                fileNameArray->AppendL(filename.Name());
                }
            }

        // add "default" (resource file) to list
        fileNameArray->AppendL(_L("Default"));

		ret = iEngine->GetEngineWrapper()->PopupListDialog(_L("Select random data file"), fileNameArray, &iSelectedItem, this, EGetingRandomDataFile);
        CleanupStack::PopAndDestroy(fileNameArray);
        }
    else  // no random data files found from the search paths
        {
        iEngine->GetEngineWrapper()->ShowNote(_L("No random data files found"));
        delete iSearchArray;
        iSearchArray = NULL;
        }

    return ret;
    }

TInt CCommandParser::FindFilesRecursiveL(const TDesC& aFileName, const TDesC& aPath)
    {
    return CreatorFileUtils::FindFilesRecursiveL(iSearchArray, aFileName, aPath);
    }
TInt CreatorFileUtils::FindFilesRecursiveL(CDesCArrayFlat* aFileArray, const TDesC& aFileName, const TDesC& aPath)
	{
    TInt err = KErrNone;
    CDirScan* scan = CDirScan::NewLC(CEikonEnv::Static()->FsSession());
    scan->SetScanDataL(aPath, KEntryAttDir, ESortNone);
    CDir* dirEntries = NULL;

    for(;;)
        {
        TRAP(err, scan->NextL(dirEntries));
        if (!dirEntries  || (err!=KErrNone))
            break;

        for (TInt i=0; i<dirEntries->Count(); i++) 
            {
            TFileName path(scan->FullPath());
            path.Append((*dirEntries)[i].iName);
            path.Append(_L("\\"));
            FindFiles(aFileArray, aFileName, path);
            }
        delete(dirEntries);
        }

    CleanupStack::PopAndDestroy(scan);
    return err;
    }

TInt CreatorFileUtils::FindFiles(CDesCArrayFlat* aFileArray, const TDesC& aFileName, const TDesC& aPath)
    {
    TFindFile fileFinder(CEikonEnv::Static()->FsSession());
    CDir* fileList; 
    TInt err = fileFinder.FindWildByDir(aFileName, aPath, fileList);

    while (err == KErrNone)
        {
        for (TInt i=0; i<fileList->Count(); i++)
            {
            TParse fullentry;
            fullentry.Set((*fileList)[i].iName, &fileFinder.File(), NULL);
            
            TRAP(err, aFileArray->AppendL(fullentry.FullName())); 
            }

        delete fileList;
        err = fileFinder.FindWild(fileList);
        }
    return err;
    }
// ---------------------------------------------------------------------------

TInt CCommandParser::FindFiles(const TDesC& aFileName, const TDesC& aPath)
    {
    return CreatorFileUtils::FindFiles(iSearchArray, aFileName, aPath);
    }

// ---------------------------------------------------------------------------

TInt CCommandParser::ReadLineFromFileL(RFile& aInputFile)
    {
    LOGSTRING("Creator: CCommandParser::ReadLineFromFile");

	_LIT8(KImcvCRLF, "\r\n");

    TInt err = KErrNone;

    //  Read into the buffer
    HBufC8* tempLine = HBufC8::NewLC(KReadBufSize);
    TPtr8 buffer = tempLine->Des();

    // clear the targer
    iReadBuf->Des().Copy(_L(""));

    //  Get the current file position
    TInt filePos = 0;
    aInputFile.Seek(ESeekCurrent, filePos);

    //  Read the buffer
    err = aInputFile.Read(buffer);

    //end of file?
    TInt s = buffer.Length();
    if  ( s == 0)
        err = KErrEof;

    if  (err == KErrNone)
        {
        //  Copy to the lfcr and then set the file pointer to the point after that
        TInt pos = buffer.Find(KImcvCRLF);
        if      ( pos != -1)
            {
            iReadBuf->Des().Justify(buffer, pos, ELeft, ' ');
            filePos += (pos+2);

            //  Set the file pointer back to after the lfcr
            aInputFile.Seek(ESeekStart, filePos);
            }
        //  Else fill the whole buffer
        else
            {
            iReadBuf->Des().Copy(buffer);
            }
        }

    CleanupStack::PopAndDestroy(); // tempLine
    return err;
    }

// ---------------------------------------------------------------------------

void CCommandParser::StrParserL(HBufC*& aDestinationBuf, TUint aToken)
    {
    // store old position
	iParserOldPosition = iParserPosition;

    // make sure we are not reading outside of buffer
    if (iParserPosition >= iReadBuf->Length() - 1)
        return;
    
    // read until asked token
	do
	    {
	    iParserPosition++;
	    }
	while (iReadBuf->Des()[iParserPosition] != aToken && iParserPosition < iReadBuf->Des().Length()-1);
	
	// calculate length of the destination buffer
	TInt bufferLength = iParserPosition-iParserOldPosition-1;
	
	// reset the destination buffer
	if (aDestinationBuf)
	    {
        delete aDestinationBuf;
        aDestinationBuf = NULL;
	    }

    // allocate memory for the destination buffer
	aDestinationBuf = HBufC::NewL(bufferLength);

	// copy data from the read buffer to the destination buffer
    aDestinationBuf->Des().Copy(iReadBuf->Des().Mid(iParserOldPosition+1, bufferLength));
    }

// ---------------------------------------------------------------------------

void CCommandParser::StrParserL(HBufC8*& aDestinationBuf, TUint aToken)
    {
    // store old position
	iParserOldPosition = iParserPosition;

    // make sure we are not reading outside of buffer
    if (iParserPosition >= iReadBuf->Length() - 1)
        return;
    
    // read until asked token
	do
	    {
	    iParserPosition++;
	    }
	while (iReadBuf->Des()[iParserPosition] != aToken && iParserPosition < iReadBuf->Des().Length()-1);
	
	// calculate length of the destination buffer
	TInt bufferLength = iParserPosition-iParserOldPosition-1;
	
	// reset the destination buffer
	if (aDestinationBuf)
	    {
        delete aDestinationBuf;
        aDestinationBuf = NULL;
	    }

    // allocate memory for the destination buffer
	aDestinationBuf = HBufC8::NewL(bufferLength);

	// copy data from the read buffer to the destination buffer
    aDestinationBuf->Des().Copy(iReadBuf->Des().Mid(iParserOldPosition+1, bufferLength));
    }

//----------------------------------------------------------------------------

void CCommandParser::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/) // from MMsvSessionObserver
    {
    }

//----------------------------------------------------------------------------

