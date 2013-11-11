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
*
*/

#include <aknviewappui.h>
#include <akntoolbar.h>
#include <barsread.h>
#include <DirectPrintApp.rsg>
#include <stringloader.h>			// StringLoader

#include "clog.h"
#include "directprintapp.hrh"
#include "directprintsearchresultview.h"

CDirectPrintSearchResultView::CDirectPrintSearchResultView()
	{
	iListBox = NULL;
	}

CDirectPrintSearchResultView::~CDirectPrintSearchResultView()
	{
	AppUi()->RemoveFromViewStack(*this, iListBox);

    if (iPeriodic)
        {
        iPeriodic->Cancel();
        delete iPeriodic;
        iPeriodic = NULL;
        }

	if (iProgressDialog)
		{
		TRAP_IGNORE(iProgressDialog->ProcessFinishedL());
		delete iProgressDialog;
		iProgressDialog = NULL;
		}
	if (iProgressMessage)
		{
		delete iProgressMessage;
		iProgressMessage = NULL;
		}

	delete iListBox;
	delete iItemArray;
	}

CDirectPrintSearchResultView* CDirectPrintSearchResultView::NewL()
	{
	CDirectPrintSearchResultView* self = CDirectPrintSearchResultView::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CDirectPrintSearchResultView* CDirectPrintSearchResultView::NewLC()
	{
	CDirectPrintSearchResultView* self = new(ELeave) CDirectPrintSearchResultView();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CDirectPrintSearchResultView::ConstructL()
	{
	LOG("CDirectPrintSearchResultView::ConstructL BEGIN");
	BaseConstructL(R_DIRECTPRINT_SEARCHRESULT_VIEW);

	iViewId = TUid::Uid(EDirectPrintSearchResultViewId);

	iItemArray = new(ELeave) CDesCArrayFlat(3);
	LOG("CDirectPrintSearchResultView::ConstructL END");
	}

void CDirectPrintSearchResultView::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EAknSoftkeyCancel:
			{
			AppUi()->HandleCommandL(EDirectPrintCmdIdSearchResultViewCancel);
			}
			break;
		case EDirectPrintCmdIdSearchResultViewSearch:
			{
			AppUi()->HandleCommandL(aCommand);
			}
			break;
		default:
			{
			AppUi()->HandleCommandL(aCommand);
			}
			break;
		}
	}

void CDirectPrintSearchResultView::HandleListIndexL(TInt /*aIndex*/)
	{
	AppUi()->HandleCommandL(EDirectPrintCmdIdSearchResultViewSelect);
	}

void CDirectPrintSearchResultView::OfferToolbarEventL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EDirectPrintCmdIdPrinterListViewEdit:
			{
			AppUi()->HandleCommandL(aCommand);
			}
			break;
		default:
			break;
		}
	}

void CDirectPrintSearchResultView::DoActivateL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
	{
	LOG("CDirectPrintSearchResultView::DoActivateL BEGIN");
	CAknToolbar* toolbar = AppUi()->CurrentFixedToolbar();
	toolbar->SetToolbarObserver(this);

	// Create list box
	if (!iListBox)
		{
		iListBox = CDirectPrintListBox::NewL(ClientRect(), iItemArray);
		iListBox->SetMopParent(this);
		iListBox->SetListObserver(this);
		}
	AppUi()->AddToStackL(*this, iListBox);
	iListBox->MakeVisible(ETrue);

	// set the title pane
	SetActiveTitleL(R_DIRECTPRINT_PRINTERSEARCH_TITLE);
	
	StartSearchL();
	LOG("CDirectPrintSearchResultView::DoActivateL END");
	}

void CDirectPrintSearchResultView::DoDeactivate()
	{
	LOG("CDirectPrintSearchResultView::DoDeactivate BEGIN");
	iListBox->MakeVisible(EFalse);
	AppUi()->RemoveFromViewStack(*this, iListBox);
	LOG("CDirectPrintSearchResultView::DoDeactivate END");
	}

void CDirectPrintSearchResultView::StartSearchProgressNoteL()
	{
	// Delete possible previous CAknProgressDialog.
	if (iProgressDialog)
		{
		delete iProgressDialog;
		iProgressDialog = NULL;
		}

	if (!iProgressMessage)
		{
		// read progress message
		iProgressMessage = StringLoader::LoadL( R_DIRECTPRINT_MSG_SEARCHING_PROGRESS );
		}

	// Create new CAknProgressDialog.
	iProgressDialog = new ( ELeave ) CAknProgressDialog( reinterpret_cast
														 <CEikDialog**> 
														 ( &iProgressDialog ) );
	iProgressDialog->SetCallback( this );
	iProgressDialog->PrepareLC( R_DIRECTPRINT_SEARCH_PROGRESS_NOTE );
	//iProgressInfo = iProgressDialog->GetProgressInfoL();
	//iProgressInfo->SetFinalValue( 1 );

	iProgressDialog->SetTextL(*iProgressMessage);
	iProgressDialog->RunLD();
	}

void CDirectPrintSearchResultView::EndProgressNoteL()
	{
	if (iProgressDialog)
		{
		iProgressDialog->ProcessFinishedL();
		if (iProgressDialog)
			{
			delete iProgressDialog;
			iProgressDialog = NULL;
			}
		}
	}

void CDirectPrintSearchResultView::StartSearchL()
	{
	const TInt KSearchTimeoutTime(10000000);  // 10 sec

    if (iPeriodic)
        {
        iPeriodic->Cancel();
        }
    else
        {
        iPeriodic = CPeriodic::NewL(CActive::EPriorityIdle);
        }
    iPeriodic->Start(KSearchTimeoutTime, KSearchTimeoutTime, TCallBack(HandleTimeout, this));
	StartSearchProgressNoteL();
	}

TInt CDirectPrintSearchResultView::HandleTimeout(TAny* aAny)
    {
    // Setting size button back to up state
    
    CDirectPrintSearchResultView* self = static_cast<CDirectPrintSearchResultView*>( aAny );
    // Cancel timer
    TRAPD( err, self->HandleTimeoutL());
    return err;
    }

void CDirectPrintSearchResultView::HandleTimeoutL()
    {
	iTimeoutCount++;
	if (iTimeoutCount == 1)
		{
		// for test
		TBuf<50> buf;
		buf.Copy(_L("\tCanon MP640_000XXX\t"));
		iItemArray->AppendL(buf);
		iListBox->DrawNow();
		}
	else if (iTimeoutCount > 1)
		{
		EndProgressNoteL();
		if (iPeriodic)
			{
			iPeriodic->Cancel();
			delete iPeriodic;
			iPeriodic = NULL;
			}
		iTimeoutCount = 0;
		}
	}

void CDirectPrintSearchResultView::DialogDismissedL( TInt aButtonId )
	{
	//TInt button = aButtonId;
	}


// End of file
