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

#include <mpxcollectionpath.h>
#include <mglxmedialist.h>
#include <glxcommandfactory.h>
#include <glxcommandhandleraddtocontainer.h>
#include <glxcommandhandlernewmedia.h>

#include <QStringListModel>
#include <hblabel.h>
#include <hbaction.h>

#include <glxalbummodel.h>
#include <glxcollectionpluginalbums.hrh>
#include <glxmodelparm.h>
#include <glxfilterfactory.h>
#include <QEventLoop>

#include <hblistwidget.h>
#include <hbview.h>
#include <hbpushbutton.h>
#include <QGraphicsGridLayout>
#include <hbselectiondialog.h>

#include <hbmessagebox.h>

#include <glxcommandhandlers.hrh>
#include "glxlocalisationstrings.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "glxcommandhandleraddtocontainerTraces.h"
#endif


const TInt KSelectionPopupListHierarchy = 5;

GlxAlbumSelectionPopup::GlxAlbumSelectionPopup() 
    : mSelectionModel( 0 ),
      mEventLoop( 0 ),
      mResult( false )
{
}

GlxAlbumSelectionPopup::~GlxAlbumSelectionPopup()
{
}

QModelIndexList GlxAlbumSelectionPopup::GetSelectionList(GlxAlbumModel *model, bool *ok) 
{
    HbSelectionDialog *dlg = new HbSelectionDialog;
	dlg->setHeadingWidget(new HbLabel(GLX_ALBUM_SELECTION_TITLE));
    dlg->setSelectionMode(HbAbstractItemView::SingleSelection);
    dlg->setModel(model);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->clearActions();
    HbAction *action;
    action= new HbAction(GLX_BUTTON_NEW);
    action->setObjectName( "ch_new_album_button" );
    dlg->addAction(action);
    action= new HbAction(GLX_BUTTON_CANCEL);
    action->setObjectName( "ch_cancel_album_button" );
    dlg->addAction(action);
    dlg->open(this, SLOT(dialogClosed(HbAction*)));
    
    QEventLoop eventLoop;
    mEventLoop = &eventLoop;
  
    eventLoop.exec( );
    mEventLoop = 0 ;
    if ( ok ) {
        *ok = mResult ;
    }

    QModelIndexList selectedIndexes = dlg->selectedModelIndexes();
    return selectedIndexes;
}


void GlxAlbumSelectionPopup::dialogClosed(HbAction *action)
{
    HbSelectionDialog *dlg = (HbSelectionDialog*)(sender());

    if( action == dlg->actions().at(1) ) {
        mResult = false ;
    }
    else {
        mResult = true ;
    }
    if ( mEventLoop && mEventLoop->isRunning( ) ) {
        mEventLoop->exit( 0 );
    }
}

GlxCommandHandlerAddToContainer::GlxCommandHandlerAddToContainer() :
    mNewMediaAdded(false),mAlbumName(QString()),mIsAddtoFavCmd(false)
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERADDTOCONTAINER_GLXCOMMANDHANDLERADDTOCONTAINER_ENTRY );
    mTargetContainers = NULL;
    OstTraceFunctionExit0( GLXCOMMANDHANDLERADDTOCONTAINER_GLXCOMMANDHANDLERADDTOCONTAINER_EXIT );
    }

GlxCommandHandlerAddToContainer::~GlxCommandHandlerAddToContainer()
    {
    OstTraceFunctionEntry0( DUP1_GLXCOMMANDHANDLERADDTOCONTAINER_GLXCOMMANDHANDLERADDTOCONTAINER_ENTRY );
    delete mTargetContainers;
    OstTraceFunctionExit0( DUP1_GLXCOMMANDHANDLERADDTOCONTAINER_GLXCOMMANDHANDLERADDTOCONTAINER_EXIT );
    }

CMPXCommand* GlxCommandHandlerAddToContainer::CreateCommandL(TInt aCommandId,
        MGlxMediaList& aMediaList, TBool& /*aConsume*/) const
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERADDTOCONTAINER_CREATECOMMANDL_ENTRY );
    CMPXCommand* command = NULL;
    mAlbumName.clear();
    mIsAddtoFavCmd = false;
    if(aCommandId == EGlxCmdAddToFav)
        {
           mIsAddtoFavCmd = TRUE;
		   mAlbumName = GLX_ALBUM_FAV;
           CMPXCollectionPath* targetCollection = CMPXCollectionPath::NewL();
           CleanupStack::PushL(targetCollection);
           // The target collection has to be appeneded with the albums plugin id
           targetCollection->AppendL(KGlxCollectionPluginAlbumsImplementationUid);
           // The target collection has also to be appeneded with the the relation id.
           // appending another level into the albums to get favourites and 1 is the relation id of albums
           targetCollection->AppendL( TMPXItemId(KGlxCollectionFavoritesId) );
           targetCollection->Set( 0 );
    
           CMPXCollectionPath* sourceItems = aMediaList.PathLC( NGlxListDefs::EPathFocusOrSelection );
      
           command = TGlxCommandFactory::AddToContainerCommandLC(
               *sourceItems, *targetCollection);
           CleanupStack::Pop(command); 
           
           CleanupStack::PopAndDestroy(sourceItems);
           CleanupStack::PopAndDestroy(targetCollection);
        }
    else
        {

            CMPXCollectionPath* sourceItems = aMediaList.PathLC(
            NGlxListDefs::EPathFocusOrSelection);
            bool ok = false;
        
            //create target medialist
            CMPXCollectionPath* path = CMPXCollectionPath::NewL();
            CleanupStack::PushL(path);
            path->AppendL(KGlxCollectionPluginAlbumsImplementationUid);
        
            CMPXFilter* filter =
            TGlxFilterFactory::CreateCameraAlbumExclusionFilterL();
            CleanupStack::PushL(filter);
    
            // Create the media list
            MGlxMediaList* targetMediaList = MGlxMediaList::InstanceL(*path,
                    TGlxHierarchyId(KSelectionPopupListHierarchy), filter); //todo take actual hierarchy 
            CleanupStack::PopAndDestroy(filter);
            CleanupStack::PopAndDestroy(path);
    
            //create target model
            GlxModelParm modelParm(KGlxCollectionPluginAlbumsImplementationUid,
                    KSelectionPopupListHierarchy, EGlxFilterExcludeCamera);
            GlxAlbumModel *albumMediaModel = new GlxAlbumModel(modelParm);
            albumMediaModel->setData(QModelIndex(), (int) GlxContextSelectionList,
                    GlxContextRole);
    
            GlxAlbumSelectionPopup popupWidget;
            QModelIndexList modelList = popupWidget.GetSelectionList(albumMediaModel, &ok);
            if(ok && modelList.count() == 0)
            	{
                createNewMedia();
                ok = false;        
            	}
            else if(!ok)
                {
				// Reset the flag so that next addmedia cmd can  be executed
                iIsCmdActive = false;
                }
            OstTraceExt2( TRACE_NORMAL, GLXCOMMANDHANDLERADDTOCONTAINER_CREATECOMMANDL, "GlxCommandHandlerAddToContainer::CreateCommandL;ok=%d;newMedia=%d", ok, mNewMediaAdded );
    
            if (ok || mNewMediaAdded)
                {
                int count = modelList.count();
    
                for (int i = 0; i < count; i++)
                    {
                    albumMediaModel->setData(modelList[i], modelList[i].row(),
                            GlxSelectedIndexRole);
                    }
    
                if (!mNewMediaAdded)
                    {
                    CMPXCollectionPath* targetContainers = targetMediaList->PathLC(
                            NGlxListDefs::EPathFocusOrSelection);
                    CleanupStack::Pop(targetContainers);
                    delete mTargetContainers;
                    mTargetContainers = NULL;
                    mTargetContainers = targetContainers;

					const TGlxMedia& item = targetMediaList->Item(targetMediaList->SelectedItemIndex(0));
					const TDesC& title = item.Title();
					mAlbumName = QString::fromUtf16(title.Ptr(),title.Length());
                    }
    
                command = TGlxCommandFactory::AddToContainerCommandLC(*sourceItems,
                        *mTargetContainers);
                CleanupStack::Pop(command);
                mNewMediaAdded = false;
                }
            else
                {
				// Reset the flag so that next addmedia cmd can  be executed
                iIsCmdActive = false;
                MGlxMediaList::UnmarkAllL(aMediaList);
                }
    
            MGlxMediaList::UnmarkAllL(*targetMediaList);
            targetMediaList->Close();
            CleanupStack::PopAndDestroy(sourceItems);
            delete albumMediaModel;
        }
    OstTraceFunctionExit0( GLXCOMMANDHANDLERADDTOCONTAINER_CREATECOMMANDL_EXIT );
    return command;
    }
  
void GlxCommandHandlerAddToContainer::createNewMedia() const
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERADDTOCONTAINER_CREATENEWMEDIA_ENTRY );
    GlxCommandHandlerNewMedia* commandHandlerNewMedia =
            new GlxCommandHandlerNewMedia();
    TGlxMediaId newMediaId;
    QString newTitle;
    TInt error = commandHandlerNewMedia->ExecuteLD(newMediaId,newTitle);

    while (error == KErrAlreadyExists)
        {
        QString stringToDisplay = hbTrId(GLX_NAME_ALREADY_EXIST).arg(newTitle);
        HbMessageBox::warning(stringToDisplay);
        error = KErrNone;
        error = commandHandlerNewMedia->ExecuteLD(newMediaId,newTitle);
        }

    if (error == KErrNone)
        {
        CMPXCollectionPath* path = CMPXCollectionPath::NewL();
        CleanupStack::PushL(path);
        path->AppendL(KGlxCollectionPluginAlbumsImplementationUid);
        path->AppendL(newMediaId.Value());
        CleanupStack::Pop(path);
        delete mTargetContainers;
        mTargetContainers = NULL;
        mTargetContainers = path;
        mAlbumName = newTitle;
        mNewMediaAdded = true;
        }
    OstTraceFunctionExit0( GLXCOMMANDHANDLERADDTOCONTAINER_CREATENEWMEDIA_EXIT );
    }

QString GlxCommandHandlerAddToContainer::CompletionTextL() const
    {
    if(!mAlbumName.isNull() && mIsAddtoFavCmd == false){
        return (hbTrId(GLX_IMAGES_ADDED).arg(mAlbumName)); 
        }
	return 	QString();
    }

QString GlxCommandHandlerAddToContainer::ProgressTextL() const
    {
    return GLX_ADDING_IMAGES;
    }
