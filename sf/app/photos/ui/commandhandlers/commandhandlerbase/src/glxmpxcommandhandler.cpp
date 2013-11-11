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
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectionpath.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxmessageprogressdefs.h>

#include <glxfilterfactory.h>
#include <glxcollectionpluginall.hrh>
#include <mglxmedialist.h>
#include <glxmpxcommandhandler.h>
#include <glxfetchcontextremover.h>
#include <glxmedialistiterator.h>
#include <glxattributecontext.h>
#include <glxattributeretriever.h>
#include <glxuistd.h>

#include <textresolver.h>
#include <hbmessagebox.h>
#include <hbnotificationdialog.h>
#include <e32debug.h>
#include <hbprogressdialog.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "glxmpxcommandhandlerTraces.h"
#endif

#include <hbaction.h>

GlxMpxCommandHandler::GlxMpxCommandHandler() :
    iProgressComplete(EFalse),iIsCmdActive(EFalse)
    {
    OstTraceFunctionEntry0( GLXMPXCOMMANDHANDLER_GLXMPXCOMMANDHANDLER_ENTRY );
    iMediaList = NULL;
    OstTraceFunctionExit0( GLXMPXCOMMANDHANDLER_GLXMPXCOMMANDHANDLER_EXIT );
    }

GlxMpxCommandHandler::~GlxMpxCommandHandler()
    {
    OstTraceFunctionEntry0( DUP1_GLXMPXCOMMANDHANDLER_GLXMPXCOMMANDHANDLER_ENTRY );
    //close the existing instance of Media List
    if (iMediaList)
        {
        iMediaList->Close();
        iMediaList = NULL;
        }
    OstTraceFunctionExit0( DUP1_GLXMPXCOMMANDHANDLER_GLXMPXCOMMANDHANDLER_EXIT );
    }

void GlxMpxCommandHandler::executeCommand(int commandId, int collectionId,QList<QModelIndex>  indexList)
    {
    OstTraceFunctionEntry0( GLXMPXCOMMANDHANDLER_EXECUTECOMMAND_ENTRY );
    Q_UNUSED(indexList);
    int aHierarchyId = 0;
    TGlxFilterItemType aFilterType = EGlxFilterImage;
    if (!iIsCmdActive)
         {
        if (collectionId != KGlxAlbumsMediaId)
            {
            OstTrace0( TRACE_NORMAL, DUP2_GLXMPXCOMMANDHANDLER_EXECUTECOMMAND, "GlxMpxCommandHandler::executeCommand::CreateMediaListL" );
            CreateMediaListL(collectionId, aHierarchyId, aFilterType);
            }
        else
            {
            OstTrace0( TRACE_NORMAL, DUP3_GLXMPXCOMMANDHANDLER_EXECUTECOMMAND, "GlxMpxCommandHandler::executeCommand::CreateMediaListAlbumItemL" );
            //for creating Medial List for Albums Media path Items
            CreateMediaListAlbumItemL(collectionId, aHierarchyId, aFilterType);
            }

        TBool consume = ETrue;
        iProgressComplete = EFalse;
        mProgressDialog = NULL;
        //Execute Command 
        DoExecuteCommandL(commandId, *iMediaList, consume);
        mCommandId = commandId;
        ConfirmationNoteL(*iMediaList);
        }
    OstTraceFunctionExit0( GLXMPXCOMMANDHANDLER_EXECUTECOMMAND_EXIT );
    }

void GlxMpxCommandHandler::executeMpxCommand(bool execute)
    {
    if(iMediaList)
        {
        if (execute && !iMediaList->IsCommandActive())
            {
            TBool consume = ETrue;
            CMPXCommand* command = CreateCommandL(mCommandId, *iMediaList,
                    consume);
            if (command)
                {
                command->SetTObjectValueL<TAny*> (
                        KMPXCommandGeneralSessionId,
                        static_cast<TAny*> (this));
                iMediaList->AddMediaListObserverL(this);
                iMediaList->CommandL(*command);

                if (iMediaList->SelectionCount() > 1)
                    {
                    ProgressNoteL(mCommandId);
                    }
                }
            }
        else //command cancelled,so unmark all items
            {
            MGlxMediaList::UnmarkAllL(*iMediaList);
            }
        }
    }

// ---------------------------------------------------------------------------
// CreateMediaListL()
// Creates a collection path
// Create a filter as requested filtertype
// Creates the medialist
// ---------------------------------------------------------------------------
void GlxMpxCommandHandler::CreateMediaListL(int aCollectionId,
        int aHierarchyId, TGlxFilterItemType aFilterType)
    {
    OstTraceFunctionEntry0( GLXMPXCOMMANDHANDLER_CREATEMEDIALISTL_ENTRY );
    Q_UNUSED(aHierarchyId);
    Q_UNUSED(aFilterType);

    //close the existing instance of Media List
    if (iMediaList)
        {
        iMediaList->Close();
        iMediaList = NULL;
        }

    // Create path to the list of images and videos
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL(path);
    path->AppendL(aCollectionId);
    // Create filter to filter out either images or videos
    CMPXFilter* filter = TGlxFilterFactory::CreateItemTypeFilterL(
            EGlxFilterImage); //todo take actual filter type
    CleanupStack::PushL(filter);

    // Create the media list
    iMediaList = MGlxMediaList::InstanceL(*path, TGlxHierarchyId(0), filter); //todo take actual hierarchy 
    CleanupStack::PopAndDestroy(filter);

    CleanupStack::PopAndDestroy(path);
    OstTraceFunctionExit0( GLXMPXCOMMANDHANDLER_CREATEMEDIALISTL_EXIT );
    }
    

// ---------------------------------------------------------------------------
// CreateMediaListAlbumItemL()
// Creates the media list for the album Item 
// ---------------------------------------------------------------------------
void GlxMpxCommandHandler::CreateMediaListAlbumItemL(int aCollectionId,
        int aHierarchyId, TGlxFilterItemType aFilterType)
    {
    OstTraceFunctionEntry0( GLXMPXCOMMANDHANDLER_CREATEMEDIALISTALBUMITEML_ENTRY );
    Q_UNUSED(aCollectionId);
    Q_UNUSED(aHierarchyId);
    //retrieve the instance of Media list for ALBUMS LIST which will be saved in iMediaList
    CreateMediaListL(KGlxCollectionPluginAlbumsImplementationUid, 0,
            EGlxFilterAlbum);
    //retrieve the path of the focussed item of the MediaList
    CMPXCollectionPath* path = iMediaList->PathLC(
            NGlxListDefs::EPathFocusOrSelection);
    //close the existing instance of Media List
    iMediaList->Close();
    iMediaList = NULL;
    //create new media list with the derived path
    CMPXFilter* filter =
            TGlxFilterFactory::CreateItemTypeFilterL(aFilterType);
    CleanupStack::PushL(filter);
    iMediaList = MGlxMediaList::InstanceL(*path, TGlxHierarchyId(0), filter); //todo take actual hierarchy
    CleanupStack::PopAndDestroy(filter);

    CleanupStack::PopAndDestroy(path);
    OstTraceFunctionExit0( GLXMPXCOMMANDHANDLER_CREATEMEDIALISTALBUMITEML_EXIT );
    }
// -----------------------------------------------------------------------------
// HandleItemAddedL
// -----------------------------------------------------------------------------
//  
void GlxMpxCommandHandler::HandleItemAddedL(TInt /*aStartIndex*/,
        TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
    {
    OstTraceFunctionEntry0( GLXMPXCOMMANDHANDLER_HANDLEITEMADDEDL_ENTRY );
    // Do nothing
    OstTraceFunctionExit0( GLXMPXCOMMANDHANDLER_HANDLEITEMADDEDL_EXIT );
    }

// -----------------------------------------------------------------------------
// HandleMediaL
// -----------------------------------------------------------------------------
//	
void GlxMpxCommandHandler::HandleMediaL(TInt /*aListIndex*/, MGlxMediaList* /*aList*/)
    {
    OstTraceFunctionEntry0( GLXMPXCOMMANDHANDLER_HANDLEMEDIAL_ENTRY );
    // Do nothing
    OstTraceFunctionExit0( GLXMPXCOMMANDHANDLER_HANDLEMEDIAL_EXIT );
    }

// -----------------------------------------------------------------------------
// HandleItemRemovedL
// -----------------------------------------------------------------------------
//	
void GlxMpxCommandHandler::HandleItemRemovedL(TInt /*aStartIndex*/,
        TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
    {
    OstTraceFunctionEntry0( GLXMPXCOMMANDHANDLER_HANDLEITEMREMOVEDL_ENTRY );
    // Do nothing
    OstTraceFunctionExit0( GLXMPXCOMMANDHANDLER_HANDLEITEMREMOVEDL_EXIT );
    }
	
// -----------------------------------------------------------------------------
// HandleItemModifiedL
// -----------------------------------------------------------------------------
//	
void GlxMpxCommandHandler::HandleItemModifiedL(const RArray<TInt>& /*aItemIndexes*/, 
        MGlxMediaList* /*aList*/)
	{
	OstTraceFunctionEntry0( GLXMPXCOMMANDHANDLER_HANDLEITEMMODIFIEDL_ENTRY );
	// Do nothing
	OstTraceFunctionExit0( GLXMPXCOMMANDHANDLER_HANDLEITEMMODIFIEDL_EXIT );
	}	

// -----------------------------------------------------------------------------
// HandleAttributesAvailableL
// -----------------------------------------------------------------------------
//	
void GlxMpxCommandHandler::HandleAttributesAvailableL(TInt /*aItemIndex*/, 	
	const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* /*aList*/)
	{
	OstTraceFunctionEntry0( GLXMPXCOMMANDHANDLER_HANDLEATTRIBUTESAVAILABLEL_ENTRY );
	// Do nothing
	OstTraceFunctionExit0( GLXMPXCOMMANDHANDLER_HANDLEATTRIBUTESAVAILABLEL_EXIT );
	}
		
// -----------------------------------------------------------------------------
// HandleFocusChangedL
// -----------------------------------------------------------------------------
//	
void GlxMpxCommandHandler::HandleFocusChangedL(NGlxListDefs::TFocusChangeType /*aType*/, 
        TInt /*aNewIndex*/, TInt /*aOldIndex*/, MGlxMediaList* /*aList*/) 
	{
	OstTraceFunctionEntry0( GLXMPXCOMMANDHANDLER_HANDLEFOCUSCHANGEDL_ENTRY );
	// Do nothing
	OstTraceFunctionExit0( GLXMPXCOMMANDHANDLER_HANDLEFOCUSCHANGEDL_EXIT );
	}
	
// -----------------------------------------------------------------------------
// HandleItemSelectedL
// -----------------------------------------------------------------------------
//	
void GlxMpxCommandHandler::HandleItemSelectedL(TInt /*aIndex*/, 
        TBool /*aSelected*/, MGlxMediaList* /*aList*/)
	{
	OstTraceFunctionEntry0( GLXMPXCOMMANDHANDLER_HANDLEITEMSELECTEDL_ENTRY );
	// Do nothing
	OstTraceFunctionExit0( GLXMPXCOMMANDHANDLER_HANDLEITEMSELECTEDL_EXIT );
	}

// -----------------------------------------------------------------------------
// HandleMessageL
// -----------------------------------------------------------------------------
//	
void GlxMpxCommandHandler::HandleMessageL(const CMPXMessage& aMessage,
        MGlxMediaList*/* aList*/)
    {
    OstTraceFunctionEntry0( GLXMPXCOMMANDHANDLER_HANDLEMESSAGEL_ENTRY );
    if ((aMessage.IsSupported(KMPXMessageGeneralId) && (aMessage.IsSupported(
            KMPXCommandGeneralSessionId))))
        {
        TInt msgId = aMessage.ValueTObjectL<TInt> (KMPXMessageGeneralId);
        TAny* sessionId = aMessage.ValueTObjectL<TAny*> (
                KMPXCommandGeneralSessionId);
        TInt error = KErrNone;
        // Check if this is a progress message and intended for this object
        TryExitL(error);
        }
    OstTraceFunctionExit0( GLXMPXCOMMANDHANDLER_HANDLEMESSAGEL_EXIT );
    }

// -----------------------------------------------------------------------------
// HandleCommandCompleteL
// -----------------------------------------------------------------------------
//	
void GlxMpxCommandHandler::HandleCommandCompleteL(TAny* aSessionId,
        CMPXCommand* aCommandResult, TInt aError, MGlxMediaList* aList)
    {
    OstTraceFunctionEntry0( GLXMPXCOMMANDHANDLER_HANDLECOMMANDCOMPLETEL_ENTRY );
    if(iMediaList)
        {
        MGlxMediaList::UnmarkAllL(*iMediaList);

        DoHandleCommandCompleteL(aSessionId, aCommandResult, aError, aList);
        iProgressComplete = ETrue;
        TryExitL(aError);
        }
    OstTraceFunctionExit0( GLXMPXCOMMANDHANDLER_HANDLECOMMANDCOMPLETEL_EXIT );
    }

// -----------------------------------------------------------------------------
// HandleError
// -----------------------------------------------------------------------------
//  
void GlxMpxCommandHandler::HandleError(TInt /*aError*/)
    {
    OstTraceFunctionEntry0( GLXMPXCOMMANDHANDLER_HANDLEERROR_ENTRY );
    
    OstTraceFunctionExit0( GLXMPXCOMMANDHANDLER_HANDLEERROR_EXIT );
    }
// -----------------------------------------------------------------------------
// Default implementation of advanced command handling does nothing
// -----------------------------------------------------------------------------
//	
void GlxMpxCommandHandler::DoHandleCommandCompleteL(TAny* /*aSessionId*/, 
        CMPXCommand* /*aCommandResult*/, TInt /*aError*/, MGlxMediaList* /*aList*/)
    {
    OstTraceFunctionEntry0( GLXMPXCOMMANDHANDLER_DOHANDLECOMMANDCOMPLETEL_ENTRY );
    
    OstTraceFunctionExit0( GLXMPXCOMMANDHANDLER_DOHANDLECOMMANDCOMPLETEL_EXIT );
    }
 
// -----------------------------------------------------------------------------
// Default implementation of advanced command handling does nothing
// -----------------------------------------------------------------------------
//

void GlxMpxCommandHandler::DoExecuteCommandL(TInt /*aCommandId*/,
        MGlxMediaList& /*aMediaList*/, TBool& /*aConsume*/)
    {
    OstTraceFunctionEntry0( GLXMPXCOMMANDHANDLER_DOEXECUTECOMMANDL_ENTRY );

    OstTraceFunctionExit0( GLXMPXCOMMANDHANDLER_DOEXECUTECOMMANDL_EXIT );
    }

// -----------------------------------------------------------------------------
// TryExitL
// -----------------------------------------------------------------------------
//  
void GlxMpxCommandHandler::TryExitL(TInt aErrorCode)
    {
    OstTraceFunctionEntry0( GLXMPXCOMMANDHANDLER_TRYEXITL_ENTRY );
    if (KErrNone != aErrorCode)
        {
        // @todo error received. Close progress note 
        DismissProgressNoteL();
        if(iMediaList)
            {
            MGlxMediaList::UnmarkAllL(*iMediaList);
            iMediaList->RemoveMediaListObserver(this);
            iMediaList->Close();
            iMediaList = NULL;
            }
        // handle error
        HandleErrorL(aErrorCode);
        }
    else if (iProgressComplete)
        {
        DismissProgressNoteL();
        iMediaList->RemoveMediaListObserver(this);
        iMediaList->Close();
        iMediaList = NULL;
        CompletionNoteL();
        iProgressComplete = EFalse;
        }
    iIsCmdActive = EFalse;
    OstTraceFunctionExit0( GLXMPXCOMMANDHANDLER_TRYEXITL_EXIT );
    }

// -----------------------------------------------------------------------------
// Default implementation shows an error note
// -----------------------------------------------------------------------------
//	
void GlxMpxCommandHandler::HandleErrorL(TInt aErrorCode) const
    {
    OstTraceFunctionEntry0( GLXMPXCOMMANDHANDLER_HANDLEERRORL_ENTRY );
    OstTraceFunctionExit0( GLXMPXCOMMANDHANDLER_HANDLEERRORL_EXIT );
    }

void GlxMpxCommandHandler::CompletionNoteL() const
    {
    QString displayText = CompletionTextL();
    if (!displayText.isNull())
        {
		HbNotificationDialog::launchDialog(displayText);
        }
    }

QString GlxMpxCommandHandler::CompletionTextL() const
    {
    return QString();
    }

QString GlxMpxCommandHandler::ProgressTextL() const
    {
    return QString("Progressing...");
    }

void GlxMpxCommandHandler::ProgressNoteL(TInt /*aCommandId*/)
    {
    mProgressDialog = new HbProgressDialog(HbProgressDialog::WaitDialog);
    mProgressDialog->setText(ProgressTextL());
    connect ( mProgressDialog, SIGNAL (cancelled()), this, SLOT(commandCancelled()));
    mProgressDialog->show();
    }

void GlxMpxCommandHandler::DismissProgressNoteL()
    {
    if(mProgressDialog)
        {
        mProgressDialog->close();
        delete mProgressDialog;
        mProgressDialog = NULL;
        }
    }

void GlxMpxCommandHandler::ConfirmationNoteL(MGlxMediaList& aMediaList)
    {
	TInt selectionCount = aMediaList.SelectionCount();
	iIsCmdActive = ETrue;
    // If media list is not empty, treat focused item as selected
    // At this point can assume that the command was disabled 
    // if static items were not supported	
	if ( selectionCount == 0  )
		{
		selectionCount = 1;
		}
	
    // Show confirmation note
	if ( selectionCount == 1 )
	    {
		ConfirmationNoteSingleL(aMediaList);
	    }
	else
	    {
		ConfirmationNoteMultipleL(aMediaList);
	    }
    }

void GlxMpxCommandHandler::ConfirmationNoteSingleL(MGlxMediaList& aMediaList)
    {
    QString qtText = ConfirmationTextL();
    
    if(!qtText.isEmpty ())
        {
        // create fetch context
        TGlxSelectionIterator iterator;
        iterator.SetRange(1);
        CGlxAttributeContext* attributeContext = new(ELeave) CGlxAttributeContext(&iterator);
        CleanupStack::PushL(attributeContext);
        attributeContext->AddAttributeL(KMPXMediaGeneralTitle);
                        
        // add context to media list
        aMediaList.AddContextL(attributeContext, KGlxFetchContextPriorityLow);
            
        // TGlxContextRemover will remove the context when it goes out of scope
        // Used here to avoid a trap and still have safe cleanup
        TGlxFetchContextRemover contextRemover(attributeContext, aMediaList);
        // put to cleanupstack as cleanupstack is emptied before stack objects
        // are deleted
        CleanupClosePushL( contextRemover);
        // retrieve title attribute
        TInt err = GlxAttributeRetriever::RetrieveL(*attributeContext, aMediaList, EFalse);
        // context off the list
        CleanupStack::PopAndDestroy( &contextRemover );
    
        if ( err == KErrNone )
            {
            // use iterator to get the index of the right item 
            iterator.SetToFirst(&aMediaList);
            const CGlxMedia* media = aMediaList.Item(iterator++).Properties();
        
            // noteText has a place for a title string in it
            const TDesC& itemName = media->ValueText(KMPXMediaGeneralTitle);
            QString qtItemName = QString::fromUtf16(itemName.Ptr(),itemName.Length());
            QString localisedString  = qtText.arg(qtItemName);
            qtText = localisedString;          
            }
            // (else) If error, assume confirmed anyway
        CleanupStack::PopAndDestroy(attributeContext);
        HbMessageBox::question(qtText, this, SLOT(messageDialogClose(HbAction*)),HbMessageBox::Ok | HbMessageBox::Cancel);
        }
    else{
        executeMpxCommand(true);
        }
    }

void GlxMpxCommandHandler::ConfirmationNoteMultipleL(MGlxMediaList& /*aMediaList*/)
    {
    QString qtText = ConfirmationTextL(true);
    if(!qtText.isEmpty ())
        {
        HbMessageBox::question(qtText, this, SLOT(messageDialogClose(HbAction*)),HbMessageBox::Ok |HbMessageBox::Cancel);
        }    
    else{
        executeMpxCommand(true);
        }
    }

void GlxMpxCommandHandler::messageDialogClose(HbAction* action)
    {
    HbMessageBox *dlg = static_cast<HbMessageBox*>(sender());
    if(action == dlg->actions().at(0)) 
        {
        executeMpxCommand(true);
        }
    else
        {
        // Cancellation is done.
        executeMpxCommand(false);
        iIsCmdActive = EFalse;
        }
    }


QString GlxMpxCommandHandler::ConfirmationTextL(bool /*multiSelection */) const
    {
    return QString();
    }

void GlxMpxCommandHandler::commandCancelled()
    {
    disconnect ( mProgressDialog, SIGNAL (cancelled()), this, SLOT(commandCancelled()));
    mProgressDialog = NULL;
    iMediaList->CancelCommand();
    TryExitL(KErrCancel);
    }
