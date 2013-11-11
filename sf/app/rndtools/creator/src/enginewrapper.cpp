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


#include <hbprogressdialog.h>
#include <hbmessagebox.h>
#include <hblabel.h>

#include <QString>
#include <QDate>

#include <e32std.h>
#include <e32base.h>

#include "creator.hrh" // for command ids
#include "notifications.h"
#include "enginewrapper.h"
#include "engine.h"
#include "mainview.h"



// ---------------------------------------------------------------------------

EngineWrapper::EngineWrapper()
: iEngine(0), iProgressDialog(0)
{
}

// ---------------------------------------------------------------------------

EngineWrapper::~EngineWrapper()
{
    if (iEngine != 0) {
        delete iEngine;
        iEngine = NULL;
    }
    if (iProgressDialog != 0) {
        delete iProgressDialog;
        iProgressDialog = NULL;
    }
}

// ---------------------------------------------------------------------------

bool EngineWrapper::init()
{
    TRAPD(err, iEngine = CCreatorEngine::NewL(this));
    if(err != KErrNone) {
        return false;
    }
    else {
        return true;
    }
}

// ---------------------------------------------------------------------------

QList<MemoryDetails> EngineWrapper::GetMemoryDetailsList()
	{
	RPointerArray<TMemoryDetails> tMemDetList = iEngine->GetMemoryDetailsList();
	QList<MemoryDetails> memDetList;
	MemoryDetails memDet;
		for(int i=0; i<tMemDetList.Count(); i++)
			{
			TMemoryDetails* temp = tMemDetList.operator [](i);
			QString free = QString::fromUtf16( temp->iFree.Ptr(), temp->iFree.Length() );
			QString size = QString::fromUtf16( temp->iSize.Ptr(), temp->iSize.Length() );
			QChar drive(temp->iDriveLetter.GetUpperCase());
			memDet.mFree = free; //QString((QChar*)tMemDetList[i]->iFree.Ptr(), tMemDetList[i]->iFree.Length());
			memDet.mSize = size; //QString((QChar*) tMemDetList[i]->iSize.Ptr(), tMemDetList[i]->iSize.Length());
			memDet.mDriveLetter = QString(drive);
			
			//memDetList[i].mDriveLetter = QString::fromUtf8( (char *) &tMemDetList[i]->iDriveLetter, (int) sizeof( char ) );
			memDetList.append( memDet );
			}
	return memDetList;
	}

MemoryDetails EngineWrapper::GetMemoryDetails()
{
	
    TMemoryDetails tMemoryDetails = iEngine->GetMemoryDetails();
    MemoryDetails memoryDetails;

    // Convert TMemoryDetails to MemoryDetails 
	memoryDetails.mRamFree  = QString::fromUtf16( tMemoryDetails.iRamFree.Ptr(), tMemoryDetails.iRamFree.Length() );
	memoryDetails.mRamSize  = QString::fromUtf16( tMemoryDetails.iRamSize.Ptr(), tMemoryDetails.iRamSize.Length() );

	return memoryDetails;
}


// ---------------------------------------------------------------------------

void EngineWrapper::ShowErrorMessage(const TDesC& aErrorMessage)
{
    QString errorMessage((QChar*)aErrorMessage.Ptr(),aErrorMessage.Length());
    Notifications::error(errorMessage);
 
}

// ---------------------------------------------------------------------------

void EngineWrapper::ShowNote(const TDesC& aNoteMessage, TInt /*aResourceId*/)
{
    QString note((QChar*)aNoteMessage.Ptr(),aNoteMessage.Length());
    Notifications::showGlobalNote(note, HbMessageBox::MessageTypeInformation, 1000);
}

// ---------------------------------------------------------------------------

void EngineWrapper::ShowProgressBar(const TDesC& aPrompt, int aMax)
{
	QString text((QChar*)aPrompt.Ptr(), aPrompt.Length());
	CloseProgressbar();
    iProgressDialog = Notifications::showProgressBar(text, aMax);
	connect(iProgressDialog, SIGNAL(cancelled()), this, SLOT(ProgressDialogCancelled()));
}

// ---------------------------------------------------------------------------

void EngineWrapper::IncrementProgressbarValue()
{
    if(iProgressDialog)
        iProgressDialog->setProgressValue(iProgressDialog->progressValue() + 1);
}

// ---------------------------------------------------------------------------

void EngineWrapper::CloseProgressbar()
{
    if(iProgressDialog){
        disconnect(iProgressDialog, SIGNAL(cancelled()), this, SLOT(ProgressDialogCancelled()));
        delete iProgressDialog;
        iProgressDialog = NULL;
    }
}

// ---------------------------------------------------------------------------

TBool EngineWrapper::EntriesQueryDialog(TInt* aNumberOfEntries, const TDesC& aPrompt, TBool aAcceptsZero, MUIObserver* observer, int userData)
    {
	QString text((QChar*)aPrompt.Ptr(), aPrompt.Length());
	TBool success(EFalse);
    try{
        CreatorInputDialog::launch(text, aNumberOfEntries, aAcceptsZero ? true : false, observer, userData);
        success = ETrue;
    }
    catch (std::exception& e)
        {
        Notifications::error( QString("exception: ")+e.what() );
        }
	return success;
    }
	
// ---------------------------------------------------------------------------	

TBool EngineWrapper::TimeQueryDialog(TTime* aTime, const TDesC& aPrompt, MUIObserver* observer, int userData)
    {
    QString text((QChar*)aPrompt.Ptr(), aPrompt.Length());
    TBool success(EFalse);
    try{
        CreatorDateTimeDialog::launch(text, aTime, observer, userData); 
        success = ETrue;
    }
    catch (std::exception& e)
        {
        Notifications::error( QString("exception: ")+e.what() );
        }
    return success;
    }

TBool EngineWrapper::YesNoQueryDialog(const TDesC& aPrompt, MUIObserver* observer, int userData)
{
	QString text((QChar*)aPrompt.Ptr(), aPrompt.Length());
	TBool success(EFalse);
    try{
        CreatorYesNoDialog::launch(text, "", observer, userData);	
        success = ETrue;
    }
    catch (std::exception& e)
        {
        Notifications::error( QString("exception: ")+e.what() );
        }
    return success;
}
 
// ---------------------------------------------------------------------------	

bool EngineWrapper::ExecuteOptionsMenuCommand(int commandId)
{
	TInt err = KErrNone;
	if (commandId == ECmdCreateFromFile) {
		TRAP(err, iEngine->RunScriptL());
	}
	else if (commandId == ECmdSelectRandomDataFile) {
		TBool ret = EFalse;
		TRAP(err, ret = iEngine->GetRandomDataL());
		if ( err != KErrNone || ret == EFalse ) {
			Notifications::error("Error in getting random data.");
			return false;
		}		
	}
    else {
		TRAP(err, iEngine->ExecuteOptionsMenuCommandL(commandId));
	}
	// error handling
    if(err != KErrNone) {
        return false;
    }
    else {
        return true;
    }
}

// ---------------------------------------------------------------------------	

TBool EngineWrapper::PopupListDialog(const TDesC& aPrompt, const CDesCArray* aFileNameArray, TInt* aIndex, MUIObserver* aObserver, TInt aUserData) 
{
	QString text((QChar*)aPrompt.Ptr(), aPrompt.Length());
	QStringList itemList;

	for (int i = 0; i < aFileNameArray->Count(); i++) {
		itemList.append(QString::fromUtf16(
				  aFileNameArray->MdcaPoint(i).Ptr(),
				  aFileNameArray->MdcaPoint(i).Length()));
	}
	TBool success(EFalse);
    try{
        CreatorSelectionDialog::launch(text, itemList, aIndex, aObserver, aUserData);
	    success = ETrue;
    }
    catch (std::exception& e)
        {
        Notifications::error( QString("exception: ")+e.what() );
        }
    return success;
}

// ---------------------------------------------------------------------------	

TBool EngineWrapper::DirectoryQueryDialog(const TDesC& aPrompt, TDes& aDirectory, MUIObserver* aObserver, TInt aUserData)
{
	QString text((QChar*)aPrompt.Ptr(), aPrompt.Length());
	TBool success(EFalse);
    try{
        CreatorInputDialog::launch(text, aDirectory, aObserver, aUserData);
        success = ETrue;
    }
    catch (std::exception& e)
        {
        Notifications::error( QString("exception: ")+e.what() );
        }
    return success;
}


// ---------------------------------------------------------------------------

void EngineWrapper::ProgressDialogCancelled()
{
	TRAPD(err, iEngine->ProgressDialogCancelledL());
	// error handling
    if(err != KErrNone) {
        Notifications::error("Error in operation cancel.");
    }
    // disconnect & delete iProgressDialog to free memory
    disconnect(iProgressDialog, SIGNAL(cancelled()), this, SLOT(ProgressDialogCancelled()));
    iProgressDialog->deleteLater();
    iProgressDialog = NULL;
}

// ---------------------------------------------------------------------------

TBool EngineWrapper::ListQueryDialog(const TDesC& aPrompt, TListQueryId aId, TInt* aIndex, MUIObserver* aObserver, TInt aUserData)
{
	bool ret = false;
	QString text((QChar*)aPrompt.Ptr(), aPrompt.Length());
	QStringList itemList;
	
	switch (aId) {
		case R_CONTACT_CREATION_TYPE_QUERY: {
			itemList << "Default fields" << "Define...";
			break;
		}
		case R_GROUP_CREATION_TYPE_QUERY: {
			itemList << "Random" << "Define...";
			break;
		}
		case R_MESSAGE_TYPE_QUERY: {
			itemList << "SMS" << "MMS" << "AMS" << "Email" << "Smart Message" << "IR Message" << "BT Message";
			break;
		}
		case R_FOLDER_TYPE_QUERY: {
			itemList << "Inbox" << "Drafts" << "Outbox" << "Sent";
			break;
		}
		case R_UNREAD_QUERY: {
			itemList << "Read" << "New";
			break;
		}
		case R_ATTACHMENT_MULTI_SELECTION_QUERY: {
			Notifications::error("Not supported resource id."); 
			break;
		}
		case R_AMS_ATTACHMENT_SINGLE_SELECTION_QUERY: {
			itemList << "AMR 20kB";
			break;
		}
		case R_ATTACHMENT_SINGLE_SELECTION_QUERY: {
			itemList << "JPEG 25kB" << "JPEG 300kB" << "JPEG 500kB" << "PNG 15kB" << "GIF 2kB" << "RNG 1kB" 
			<< "MIDI 10kB" << "WAVE 20kB" << "AMR 20kB" << "Excel 15kB" << "Word 20kB" << "PowerPoint 40kB" 
			<< "Text 10kB" << "Text 70kB" << "3GPP 70kB" << "MP3 250kB" << "AAC 100kB" << "RM 95kB";
			break;
		}
		default: {
			Notifications::error("Error in resource id.");
			return ret;
		}
	}
	TBool success(EFalse);
    try{
        CreatorSelectionDialog::launch(text, itemList, aIndex, aObserver, aUserData);
        success = ETrue;
    }
    catch (std::exception& e)
        {
        Notifications::error( QString("exception: ")+e.what() );
        }
    return success;
}

TBool EngineWrapper::ListQueryDialog(const TDesC& aPrompt, TListQueryId aId, CArrayFixFlat<TInt>* aSelectedItems, MUIObserver* aObserver, TInt aUserData)
{
    TBool success(EFalse);
	QStringList itemList;
	QString text((QChar*)aPrompt.Ptr(), aPrompt.Length());
	if (aId == R_ATTACHMENT_MULTI_SELECTION_QUERY) {
       itemList << "None" << "JPEG 25kB" << "JPEG 300kB" << "JPEG 500kB" << "PNG 15kB" << "GIF 2kB" << "RNG 1kB" 
           << "MIDI 10kB" << "WAVE 20kB" << "AMR 20kB" << "Excel 15kB" << "Word 20kB" << "PowerPoint 40kB" 
           << "Text 10kB" << "Text 70kB" << "3GPP 70kB" << "MP3 250kB" << "AAC 100kB" << "RM 95kB";
	}
	else{
        Notifications::error("Error in resource id.");
        return EFalse;
    }
	
    try{
        CreatorSelectionDialog::launch(text, itemList, aSelectedItems, aObserver, aUserData);
        success = ETrue;
    }
    catch (std::exception& e)
        {
        Notifications::error( QString("exception: ")+e.what() );
        }
    return success;
}

void EngineWrapper::CloseCreatorApp()
{
	MainView::closeApp();
}
