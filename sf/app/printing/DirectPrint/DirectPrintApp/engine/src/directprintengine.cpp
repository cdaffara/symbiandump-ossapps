/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:
* {Description of the file}
*
*/

#include <pdrport.h>				// CFilePrinterPort
#include <TXTETEXT.H>				// CEditableText
#include <TXTRICH.H>				// CRichText
#include <FLDBLTIN.H>				// CPageNumField
#include <flddef.h>					// KPageNumberFieldUid, KNumPagesFieldUid
#include <stringloader.h>			// StringLoader
#include <xmlengdocument.h>			// RXmlEngDocument
#include <xmlengelement.h>			// TXmlEngElement
#include <xmlengattr.h>				// TXmlEngAttr
#include <xmlengnodelist.h>			// RXmlEngNodeList
#include <bautils.h>				// BaflUtils
#include <eikenv.h>					// CEikonEnv

#include "clog.h"
#include <directprintapp.rsg>
#include "directprintengine.h"
#include "directprintband.h"
#include "directprintengineobserver.h"
#include "directprintmodel.h"
#include "directprintbanduid.h"

//! Default Page spec in Twips
#define KDefaultPageSpecInTwips TPageSpec(TPageSpec::EPortrait,TSize(11906,16838))

CDirectPrintEngine::CDirectPrintEngine(CDirectPrintModel& aModel)
	: iModel(aModel)
	{
	}

CDirectPrintEngine::~CDirectPrintEngine()
	{
	if (iProgressDialog)
		{
		TRAP_IGNORE(iProgressDialog->ProcessFinishedL());
		delete iProgressDialog;
		iProgressDialog = NULL;
		}

	if (iPrintBand)
		{
		delete iPrintBand;
		iPrintBand = NULL;
		}

	iPrintSetup->FreeModelList();
	delete iPrintSetup;
	delete iProgressMessage;
	iFs.Close();
	}

CDirectPrintEngine* CDirectPrintEngine::NewL(CDirectPrintModel& aModel)
	{
	CDirectPrintEngine* self = CDirectPrintEngine::NewLC(aModel);
	CleanupStack::Pop(self);
	return self;
	}

CDirectPrintEngine* CDirectPrintEngine::NewLC(CDirectPrintModel& aModel)
	{
	CDirectPrintEngine* self = new(ELeave) CDirectPrintEngine(aModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CDirectPrintEngine::ConstructL()
	{
	LOG("CDirectPrintEngine::ConstructL BEGIN");
	iFs.Connect();

	iPrintSetup = CPrintSetup::NewL();
	iPrintSetup->AddPrinterDriverDirL(KDefaultPrinterDriverPath);
	iModelList = iPrintSetup->ModelNameListL(iFs);

#ifdef _DEBUG
	LOG1("ModelCount=[%d]", ModelCount());
	for (TInt i=0; i < ModelCount(); i++)
		{
		TPrinterModelEntry entry = (*iModelList)[i];
		RDebug::Print(_L("[%d][%S][%d] "), i, &(entry.iModelName), entry.iRequiresPrinterPort);
		LOG1("ModelName=[%S]", &(entry.iModelName));
		}
#endif
	// read progress message
	iProgressMessage = StringLoader::LoadL( R_DIRECTPRINT_MSG_PRINTING_PROGRESS );

	LOG("CDirectPrintEngine::ConstructL END");
	}

void CDirectPrintEngine::SetObserver(MDirectPrintEngineObserver* aObserver)
	{
	iObserver = aObserver;
	}

/**
  Override of base class virtual.
  Notifies that a print or print preview operation is about to begin.
 */
void CDirectPrintEngine::NotifyPrintStarted(TPrintParameters /*aPrintParams*/)
	{
	iPageCounter = 0;
	TRAPD(err, StartPrintProgressNoteL());
	iLastError = err;
	}

/**
  Override of base class virtual.\n
  Notifies that a band is about to be printed used to display print progress information,
  including the current page number.\n
  It is called immediately before each band is printed.\n
 */
void CDirectPrintEngine::NotifyBandPrinted(TInt /*aPercent*/, TInt aCurrentPageNum, TInt /*aCurrentCopyNum*/)
	{
	if (iPageCounter==0)
		{
		// start print
		}
	if (iPageCounter<aCurrentPageNum)
		{
		// next page
		iPageCounter = aCurrentPageNum;
		TRAPD(err, UpdatePrintProgressNoteL(iPageCounter));
		iLastError = err;
		}
	}

/**
  Override of base class virtual.\n
  Notifies that the print or print preview operation has completed.\n
  used to display information about how the operation completed,
  for example any errors that occurred.\n
 */
void CDirectPrintEngine::NotifyPrintEnded(TInt aErr)
	{
	CActiveScheduler::Stop();
	TInt err = KErrNone;

	if (aErr == KErrNone)
		{
		if (iObserver)
			{
			TRAP(err, iObserver->HandlePrintEventL(MDirectPrintEngineObserver::EDirectPrintEngineEventFinishCreatePrintData, 0, KNullDesC));
			}
		}
	else
		{
		switch (aErr)
			{
		case KErrCancel:
			if (iObserver)
				{
				TRAP(err, iObserver->HandlePrintEventL(MDirectPrintEngineObserver::EDirectPrintEngineEventCancelCreatePrintData, aErr, KNullDesC));
				}
			break;
		default:
			if (iObserver)
				{
				TRAP(err, iObserver->HandlePrintEventL(MDirectPrintEngineObserver::EDirectPrintEngineEventErrorCreatePrintData, aErr, KNullDesC));
				}
			break;
			}
		}

	TRAP(err, EndPrintProgressNoteL());
	}

/**
  Override of base class virtual.\n
  Returns the number of pages in the current document.\n
  @return - TInt (Number of pages).\n
 */
TInt CDirectPrintEngine::UpdateFieldNumPages()const
	{
	return iMaxPageNum;
	}


void CDirectPrintEngine::PrintL()
	{
	
	// set the total page
	iMaxPageNum = 1;
	iPrintSetup->Header()->SetNumPagesInfo(*this);

	TInt modelNum = iModelList->UidToNum(ModelUid(iModelIndex));
	iPrintSetup->CreatePrinterDeviceL(modelNum);

	// create a port if necessary
	CPrinterPort* port=NULL;
	if ((*iModelList)[modelNum].iRequiresPrinterPort)
		{
#ifdef __WINSCW__
		port = CFilePrinterPort::NewL(_L("c:\\fpr.pcl"));
#else
//		port = CFilePrinterPort::NewL(_L("e:\\fpr.pcl"));
		port = CFilePrinterPort::NewL(_L("c:\\fpr.pcl"));
#endif
		}

	iPrintSetup->PrinterDevice()->SelectPageSpecInTwips(KDefaultPageSpecInTwips);
	iPrintSetup->iPageMarginsInTwips.iHeaderOffset = 720;
	iPrintSetup->iPageMarginsInTwips.iFooterOffset = 0;
	iPrintSetup->iPageMarginsInTwips.iMargins.iLeft = 1440; // 1 inch
	iPrintSetup->iPageMarginsInTwips.iMargins.iRight = 1440;
	iPrintSetup->iPageMarginsInTwips.iMargins.iTop = 1440;
	iPrintSetup->iPageMarginsInTwips.iMargins.iBottom = 1440;

	if (iObserver)
		{
		iObserver->HandlePrintEventL(MDirectPrintEngineObserver::EDirectPrintEngineEventStartCreatePrintData, 0, KNullDesC);
		}
	

	TRAPD(err, DoPrintL(port));
	if (err == KErrNone)
		{
		CActiveScheduler::Start(); // stopped by killing it - subtle...
		}
	else
		{
		if (iObserver)
			{
			iObserver->HandlePrintEventL(MDirectPrintEngineObserver::EDirectPrintEngineEventErrorCreatePrintData, err, KNullDesC);
			}
		}

//	if (iPrintBand)
//		{
//		delete iPrintBand;
//		iPrintBand = NULL;
//		}
	}

void CDirectPrintEngine::DoPrintL(CPrinterPort* aPort)
	{
	// create a header
	iPrintSetup->Header()->CreateTextL();
	TBuf<40> buf(_L("This is the header on page  out of "));
	buf.Append(CEditableText::EParagraphDelimiter);
	iPrintSetup->Header()->Text()->InsertL(0,buf);
	CTextField* pNumberfield=iPrintSetup->Header()->Text()->NewTextFieldL(KPageNumberFieldUid);
	iPrintSetup->Header()->Text()->InsertFieldL(27,pNumberfield,KPageNumberFieldUid);  
	CNumPagesField* numPagesField=(CNumPagesField*)iPrintSetup->Header()->Text()->NewTextFieldL(KNumPagesFieldUid);
	iPrintSetup->Header()->Text()->InsertFieldL(35,numPagesField,KNumPagesFieldUid);  
	iPrintSetup->Header()->Text()->UpdateFieldL(27);
	iPrintSetup->Header()->Text()->UpdateFieldL(35);
	iPrintSetup->Header()->SetFirstPageToPrintTo(1); // no header on first 3 pages

	iPrintSetup->Footer()->CreateTextL();
	TBuf<40> buf2(_L("This is the footer on page "));
	buf2.Append(CEditableText::EParagraphDelimiter);
	iPrintSetup->Footer()->Text()->InsertL(0,buf2);
	CTextField* pNumberfield2=iPrintSetup->Footer()->Text()->NewTextFieldL(KPageNumberFieldUid);
	iPrintSetup->Footer()->Text()->InsertFieldL(27,pNumberfield2,KPageNumberFieldUid);	
	iPrintSetup->Footer()->Text()->UpdateFieldL(27);

	// start print to file
	iPrintSetup->iNumOfFirstPage = 1;
	TPrintParameters params;
	params.iFirstPage = 1; // don't print the first page at all, ie print page no's 3-6
	params.iLastPage = 1;
	params.iNumCopies = 1;
	TPageSpec spec;
	spec.iPortraitPageSize = KA4PaperSizeInTwips;
	spec.iOrientation = TPageSpec::EPortrait;
	iPrintSetup->PrinterDevice()->SelectPageSpecInTwips(spec);

	//===========================
	// start printing
	//===========================
	iPrintSetup->StartPrintL(params, *(PageRegionPrinter()), aPort, this);

	}


TInt CDirectPrintEngine::ModelCount()
	{
	return iModelList->ModelCount();
	}

void CDirectPrintEngine::ModelName(TInt aIndex, TDes& aDes)
	{
	TPtrC name((*iModelList)[aIndex].iModelName);
	if (name.Length() <= aDes.MaxLength())
		{
		aDes.Copy(name);
		}
	}

TUid CDirectPrintEngine::ModelUid(TInt aIndex)
	{
	return (*iModelList)[aIndex].iUid;
	}

void CDirectPrintEngine::SetUseModel(TInt aIndex)
	{
	iModelIndex = aIndex;
	}

void CDirectPrintEngine::StartPrintProgressNoteL()
	{
	// Delete possible previous CAknProgressDialog.
	delete iProgressDialog;
	iProgressDialog = NULL;

	// Create new CAknProgressDialog.
	iProgressDialog = new ( ELeave ) CAknProgressDialog( reinterpret_cast
														 <CEikDialog**> 
														 ( &iProgressDialog ) );
	
	iProgressDialog->SetCallback( this );
	iProgressDialog->PrepareLC( R_DIRECTPRINT_PRINT_PROGRESS_NOTE );
	iProgressInfo = iProgressDialog->GetProgressInfoL();
	iProgressInfo->SetFinalValue( iMaxPageNum );

	TBuf<32> msg;
	msg.AppendFormat(*iProgressMessage, iPageCounter, iMaxPageNum);
	iProgressDialog->SetTextL(msg);
	iProgressDialog->RunLD();
	}

void CDirectPrintEngine::EndPrintProgressNoteL()
	{
	if (iProgressDialog)
		{
		iProgressDialog->ProcessFinishedL();
		delete iProgressDialog;
		iProgressDialog = NULL;
		}
	}

void CDirectPrintEngine::UpdatePrintProgressNoteL(TInt aPage)
	{
	if (iProgressInfo)
		{
		TBuf<32> msg;
		msg.AppendFormat(*iProgressMessage, aPage, iMaxPageNum);
		iProgressDialog->SetTextL(msg);
		iProgressInfo->SetAndDraw(aPage);
		}
	}

void CDirectPrintEngine::DialogDismissedL( TInt /*aButtonId*/ )
	{
	if (iPrintSetup)
		{
		iPrintSetup->EndPrint();
		}
	}

void CDirectPrintEngine::StartPrintPreviewL()
	{
	// Clear previous object
	EndPrintPreview();

	// set the total page
	iMaxPageNum = 1;
	iPrintSetup->Header()->SetNumPagesInfo(*this);

	TInt modelNum = iModelList->UidToNum(ModelUid(iModelIndex));
	iPrintSetup->CreatePrinterDeviceL(modelNum);

	// create a port if necessary
	//CPrinterPort* port=NULL;
	//if ((*iModelList)[modelNum].iRequiresPrinterPort)
	//	{
	//	port = CFilePrinterPort::NewL(_L("e:\\fpr.pcl"));
	//	}

	iPrintSetup->PrinterDevice()->SelectPageSpecInTwips(KDefaultPageSpecInTwips);
	iPrintSetup->iPageMarginsInTwips.iHeaderOffset = 720;
	iPrintSetup->iPageMarginsInTwips.iFooterOffset = 0;
	iPrintSetup->iPageMarginsInTwips.iMargins.iLeft = 1440; // 1 inch
	iPrintSetup->iPageMarginsInTwips.iMargins.iRight = 1440;
	iPrintSetup->iPageMarginsInTwips.iMargins.iTop = 1440;
	iPrintSetup->iPageMarginsInTwips.iMargins.iBottom = 1440;
	}

void CDirectPrintEngine::EndPrintPreview()
	{
	}

void CDirectPrintEngine::SetParamL(RXmlEngDocument* aParam)
	{
	LOG("CDirectPrintEngine::SetParamL BEGIN");

	iParam = aParam;
	if ( iPrintBand )
		{
		delete iPrintBand;
		iPrintBand = NULL;
		}

	_LIT8(KElementRecogMode, "RecognizeMode");
	_LIT8(KElementRecogModeAttr, "mode");
	_LIT8(KElementRecogModeValueFile, "PrintRecognizeModeFile");
	_LIT8(KElementRecogModeValuePlugin, "PrintRecognizeModePlugin");
	_LIT8(KElementFile, "PrintFile");
	_LIT8(KElementFileAttr, "file");
	_LIT8(KElementPluginUid, "PluginUid");
	_LIT8(KElementPluginUidAttr, "uid");

	TXmlEngElement element = iParam->DocumentElement().FirstChild().AsElement();
	HBufC* filename = NULL;
	TUint uidValue = 0;

	enum {
		EParamModeFile	 = 0x0001,
		EParamModePlugin = 0x0002,
		EParamFileExist   = 0x0008,
		EParamPluginExist = 0x0010
	};
	TUint paramFlag = 0;

	while (element.NotNull())
		{
		if (element.Name().CompareF(KElementRecogMode()) == 0)
			{
			TXmlEngAttr attr = element.AttributeNodeL(KElementRecogModeAttr,KNullDesC8);
			if(attr.Value().CompareF(KElementRecogModeValueFile) == 0)
				{
				paramFlag |= EParamModeFile;
				LOG("Param:Mode=File");
				}
			else if(attr.Value().CompareF(KElementRecogModeValuePlugin) == 0)
				{
				paramFlag |= EParamModePlugin;
				LOG("Param:Mode=Plugin");
				}
			else
				{
				LOG("[Error] Param:Mode=Not set");
				break;
				}
			}
		else if (element.Name().CompareF(KElementFile()) == 0)
			{
			TXmlEngAttr attr = element.AttributeNodeL(KElementFileAttr,KNullDesC8);
			filename = HBufC::NewLC(attr.Value().Length());
			TPtr ptr(filename->Des());
			ptr.Copy(attr.Value());
			if (ptr.Length() > 0)
				{
				LOG1("Param:FileName UID=[%S]", &ptr);
				if (BaflUtils::FileExists(CEikonEnv::Static()->FsSession(), ptr))
					{
					paramFlag |= EParamFileExist;
					}
				}
			if (!(paramFlag & EParamFileExist))
				{
				LOG("[Error] File not exist");
				}
			}
		else if (element.Name().CompareF(KElementPluginUid()) == 0)
			{
			TXmlEngAttr attr = element.AttributeNodeL(KElementPluginUidAttr,KNullDesC8);
			//HBufC8* bufUid = HBufC8::NewLC(attr.Value().Length());
			//TPtr8 ptr(bufUid->Des());
			HBufC* bufUid = HBufC::NewLC(attr.Value().Length());
			TPtr ptr(bufUid->Des());
			ptr.Copy(attr.Value());
			//_LIT8(KHeadOfHexString, "0x");
			_LIT(KHeadOfHexString, "0x");
			const TInt KUidStingLength = 8;
			if (ptr.Left(KHeadOfHexString().Length()).CompareF(KHeadOfHexString()) == 0)
				{
				ptr.Delete(0, KHeadOfHexString().Length());
				}
			if (ptr.Length() == KUidStingLength)
				{
				//TBuf<8> bufUid;
				//bufUid.Copy(ptr);
				//TLex lex(bufUid);
				TLex lex(ptr);
				if (lex.Val(uidValue,EHex) == KErrNone)
					{
					LOG1("Param:Plugin UID=[0x%X]", uidValue);
					paramFlag |= EParamPluginExist;
					}
				else
					{
					LOG("[Error] Convert from HEX string to int");
					}
				}
			if (!(paramFlag & EParamPluginExist))
				{
				LOG("[Error] File not exist");
		  		}
			CleanupStack::PopAndDestroy(bufUid); //bufUid
			}

		if (paramFlag == (EParamFileExist|EParamModeFile)
		 || paramFlag == (EParamPluginExist|EParamModePlugin))
			{
			break;
			}
		element = element.NextSibling().AsElement();
		}

	if (paramFlag == (EParamFileExist|EParamModeFile)
	 && filename)
		{
		TPtrC ptr(filename->Des());
		SearchPluginFromFilenameL(ptr);
		}
	if (paramFlag == (EParamPluginExist|EParamModePlugin)
	 && uidValue)
		{
		TUid uid = TUid::Uid(uidValue);
		LoadPluginFromUidL(uid);
		}

	if (filename)
		{
		CleanupStack::PopAndDestroy(filename); //filename
		}

	if ( !iPrintBand )
		{
		// error xml parameter
		User::Leave(KErrArgument);
		}

	LOG("CDirectPrintEngine::SetParamL END");
	}

void CDirectPrintEngine::SearchPluginFromFilenameL(const TDesC& aFileName)
	{
	LOG("CDirectPrintEngine::SearchPluginFromFilenameL START");
	RImplInfoPtrArray infoArray;
	TUid interface = TUid::Uid(KDirectPrintBandInterfaceUid);
	REComSession::ListImplementationsL( interface, infoArray );
	CleanupClosePushL( infoArray );
	LOG1("CDirectPrintEngine::SearchPluginFromFilenameL infoArray.Count(): %d", infoArray.Count());

	TParsePtrC parse(aFileName);

	TPtrC8 dataType;
	TPtrC16 dispName;
	TPtrC8 opaqueData;
	HBufC* dataTypeBuf = NULL;
	TBool bFound = EFalse;
	TInt findLen;
	const TChar KDataTypeSeparator = '|';

	for( TInt i = 0; i < infoArray.Count(); i++ )
		{
		LOG1("[InfoArray] i=%d", i);
		CImplementationInformation* info = infoArray[i];
		dataType.Set( info->DataType() );
		dispName.Set( info->DisplayName() );
		opaqueData.Set( info->OpaqueData() );

		dataTypeBuf = HBufC::NewLC( dataType.Length() );
		TPtr dataTypePtr(dataTypeBuf->Des());
		dataTypePtr.Copy( dataType );

		LOG1("[DataType]=[%S]", &dataTypePtr);
		FOREVER
			{
			findLen = dataTypePtr.Locate( KDataTypeSeparator );
			if (findLen < 0)
				{
				// the separator is not found
				if (dataTypePtr.Length() > 0)
					{
					findLen = dataTypePtr.Length();
					}
				else
					{
					break;
					}
				}
//			LOG1("[Compare]=[%S]", &(dataTypePtr.Left( findLen )));
//			LOG1("[Ext]=[%S]", &(parse.Ext()));
//			LOG1("[Comp]=[%S]", &(dataTypePtr.Left( findLen )));
			if ( parse.Ext().CompareF( dataTypePtr.Left( findLen ) ) == 0 )
				{
				bFound = ETrue;
				break;
				}
			dataTypePtr.Delete( 0, findLen+1 );
			}

		CleanupStack::PopAndDestroy(dataTypeBuf); // dataTypeBuf
		
		if ( bFound )
			{
			LOG("Match!!!");
			LoadPluginFromUidL(info->ImplementationUid());
			break;
			}
		}

	infoArray.ResetAndDestroy();
	CleanupStack::PopAndDestroy(&infoArray); // infoArray

	LOG("CDirectPrintEngine::SearchPluginFromFilenameL End");
	}

void CDirectPrintEngine::LoadPluginFromUidL(const TUid aUid)
	{
	LOG1("CDirectPrintEngine::LoadPluginFromUidL(0x%X) Start", aUid.iUid);

	CDirectPrintBand* band = NULL;
	band = CDirectPrintBand::NewL(aUid);
	TCleanupItem clItem( CleanupBand, band );
	CleanupStack::PushL( clItem );
	band->InitPrintBandL( iPrintSetup,
						  iParam,
						  KNullDesC );
	CleanupStack::Pop(); // band
	iPrintBand = band;

	LOG("CDirectPrintEngine::LoadPluginFromUidL End");
	}

void CDirectPrintEngine::CleanupBand( TAny* aData )
	{
	LOG("[CDirectPrintEngine::CleanupBand]\t Begin");

	CDirectPrintBand* band = (CDirectPrintBand*)aData;
	delete band;

	LOG("[CDirectPrintEngine::CleanupBand]\t End");
	}



// End of file
