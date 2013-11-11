/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   videolist selection dialog implementation
* 
*/

// Version : %version: 30.1.5 %

// INCLUDE FILES
#include <qgraphicsitem.h>
#include <hblabel.h>
#include <hbcheckbox.h>
#include <hbaction.h>
#include <hbstackedwidget.h>
#include <hbabstractitemview.h>
#include <hbinputdialog.h>
#include <vcxmyvideosdefs.h>
#include <hbstyleloader.h>

#include "videocollectionuiloader.h"
#include "videolistwidget.h"
#include "videoproxymodelgeneric.h"
#include "videolistselectiondialog.h"
#include "videocollectionwrapper.h"
#include "videocollectionviewutils.h"
#include "videocollectionclient.h"
#include "videocollectiontrace.h"

// Object names.
const char* const SELECTION_DIALOG_OBJECT_NAME_LIST_WIDGET    = "vc:SelectionDialogListWidget";
const char* const SELECTION_DIALOG_OBJECT_NAME_NEW_COLLECTION = "vc:SelectionDialogNewCollection";
const char* const LIST_VIEW_OBJECT_NAME_CREATE_COLLECTION     = "vc::ListViewInputDialogCreateCollection";
const char* const SELECTION_DIALOG_OBJECT_NAME_BUTTON_OK      = "vc:SelectionDialogButtonOk";
const char* const SELECTION_DIALOG_OBJECT_NAME_BUTTON_CANCEL  = "vc:SelectionDialogButtonCancel";

/**
 * global qHash function required fo creating hash values for TMPXItemId -keys
 */
inline uint qHash(TMPXItemId key) 
{ 
    QPair<uint, uint> keyPair(key.iId1, key.iId2); 

    return qHash(keyPair);
}

/**
 * Helper function for creating a new album name query dialog 
 */
HbInputDialog* gCreateNewAlbumNameDialog(const char* objectName)
{
    QString label(hbTrId("txt_videos_title_enter_name"));
    QString text(hbTrId("txt_videos_dialog_entry_new_collection"));
    HbInputDialog *dialog = new HbInputDialog();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setObjectName(objectName);
    dialog->setPromptText(label);
    dialog->setValue(text);
    return dialog;
}

// ---------------------------------------------------------------------------
// VideoListSelectionDialog
// ---------------------------------------------------------------------------
//
VideoListSelectionDialog::VideoListSelectionDialog( VideoCollectionUiLoader *uiLoader,
                                                    QGraphicsItem *parent)  
    : HbDialog( parent )
    , mUiLoader( uiLoader )
    , mTypeOfSelection( -1 )
    , mHeading( 0 )
    , mCheckboxContainer( 0 )
    , mItemCount( 0 )
    , mCheckBoxText( 0 )
    , mCheckBox( 0 )
    , mListContainer( 0 )
    , mForcedCheck( false )
    , mModel( 0 )
    , mModelReady( false )
    , mAlbumListReady( false )
    , mListWidget( 0 )
    , mPrimaryAction( 0 )
    , mSecondaryAction( 0 )
{
	FUNC_LOG;
    
	HbStyleLoader::registerFilePath( ":/style/hbdialog.css" );

    setDismissPolicy(HbDialog::NoDismiss);
    setTimeout(HbDialog::NoTimeout);    

    // create model for list widget
    mModel =
        VideoCollectionWrapper::instance().getGenericModel();
    if (mModel)
    {
        // create list widget
        mListWidget = new VideoListWidget(mUiLoader);
        if(mListWidget->initialize(*mModel) < 0)
        {
            ERROR(-1, "VideoListSelectionDialog::VideoListSelectionDialog() failed to init list widget");
            delete mListWidget;
            mListWidget = 0;
        }
        // disable list related animations to make dialog faster 
        mListWidget->setEnabledAnimations(HbAbstractItemView::None);
        mListWidget->setObjectName(SELECTION_DIALOG_OBJECT_NAME_LIST_WIDGET);
    }
}

// ---------------------------------------------------------------------------
// ~VideoListSelectionDialog
// ---------------------------------------------------------------------------
//
VideoListSelectionDialog::~VideoListSelectionDialog() 
{
	FUNC_LOG;
	HbStyleLoader::unregisterFilePath( ":/style/hbdialog.css" );
    delete mListWidget;
    mListWidget = 0;
}

// ---------------------------------------------------------------------------
// setupContent
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::setupContent(int type, TMPXItemId activeItem)
{
	FUNC_LOG;
	INFO_1("VideoListSelectionDialog::setupContent() type: %d", type);
    if(type < EDeleteVideos || type > ESelectCollection)
    {
        return;
    }
   
    mTypeOfSelection = type; 
    if(!initDialog())
    {
        ERROR(-1, "VideoListSelectionDialog::setupContent() dialog init failed.");
        return;
    }
    mSelection.clear();
    mSelectedVideos.clear();
    mSelectedAlbumId = TMPXItemId::InvalidId();
    mNewAlbumText = QString();
    
    // if provided "owner" is album or category    
    if(activeItem != TMPXItemId::InvalidId())
    {
        // if album, set as "selected"
        if(activeItem.iId2 == KVcxMvcMediaTypeAlbum)
        {
            mSelectedAlbumId = activeItem;
        }
        else if(activeItem.iId2 == KVcxMvcMediaTypeVideo)
        {
            mSelectedVideos.insert(activeItem);
        }
    }
    // set (or reset) generic id filter
    bool filterValue = true;
    if(mTypeOfSelection == EAddToCollection || 
       mTypeOfSelection == ESelectCollection ||
       mTypeOfSelection == ECreateCollection)
    {
        filterValue = false;
    }
    mModel->setGenericIdFilter(activeItem, filterValue);    
    activateSelection();
}

// ---------------------------------------------------------------------------
// initDialog
// ---------------------------------------------------------------------------
//
bool VideoListSelectionDialog::initDialog()
{
	FUNC_LOG;
    if(!mModel)
    {
        mModel = VideoCollectionWrapper::instance().getGenericModel();
        if (!mModel)
        {
           return false;
        }
    }  
    if (!mListContainer)
    {
        mListContainer =
                   mUiLoader->findWidget<HbStackedWidget>(DOCML_NAME_LIST_CONTAINER);
    
        if(!mListWidget)
        {
            mListWidget = new VideoListWidget(mUiLoader);
            mListWidget->setObjectName(SELECTION_DIALOG_OBJECT_NAME_LIST_WIDGET);
            if(mListWidget->initialize(*mModel) < 0)
            {
                ERROR(-1, "VideoListSelectionDialog::initDialog() failed to setup VideoListWidget.");
                delete mListWidget;
                mListWidget = 0;
                mListContainer = 0;
                return false;
            }
        }
        if(mListContainer)
        {
            mListContainer->addWidget(mListWidget);
        }
    }

    if(!mHeading)
    {
        mHeading = mUiLoader->findWidget<HbLabel>(DOCML_NAME_DLG_HEADINGLBL);
    }
    if(!mCheckboxContainer)
    {
        mCheckboxContainer = mUiLoader->findWidget<HbWidget>(DOCML_NAME_CHECK_CONTAINER);
    }
    if(!mItemCount)
    {
        mItemCount = mUiLoader->findWidget<HbLabel>(DOCML_NAME_LBL_SELECTION);
        mItemCount->setAlignment(Qt::AlignRight);
    }        
    if(!mCheckBox)
    {
        mCheckBox = mUiLoader->findWidget<HbCheckBox >(DOCML_NAME_MARKALL);
    }

    if(!mCheckBoxText)
    {
        mCheckBoxText = mUiLoader->findWidget<HbLabel >(DOCML_NAME_LBL_MARKALL);
    }

    // HbDialog connects actions to finished signal by default. 
    if(!mPrimaryAction)
    {
        mPrimaryAction = new HbAction(hbTrId("txt_common_button_ok"));
        mPrimaryAction->setObjectName(SELECTION_DIALOG_OBJECT_NAME_BUTTON_OK);
        addAction(mPrimaryAction);
    }
    if(!mSecondaryAction)
    {
        mSecondaryAction = new HbAction(hbTrId("txt_common_button_cancel"));
        mSecondaryAction->setObjectName(SELECTION_DIALOG_OBJECT_NAME_BUTTON_CANCEL);
        addAction(mSecondaryAction);
    }
    
    if(!mListContainer || !mHeading || !mCheckBox || !mCheckBoxText)
    {
        ERROR(-1, "VideoListSelectionDialog::initDialog() !mListContainer || !mHeading || !mCheckBox || !mCheckBoxText");
        return false;
    }
    return true;  
}

// ---------------------------------------------------------------------------
// activateSelection
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::activateSelection()
{
	FUNC_LOG;
    // "add to collection", "remove from collection" and "create collection" -selections
	// needs additional functionality for primary key
    if(mTypeOfSelection == EAddToCollection ||
       mTypeOfSelection == ERemoveFromCollection ||
       mTypeOfSelection == ECreateCollection)
    {
        mPrimaryAction->disconnect(SIGNAL(triggered()));
        connect(mPrimaryAction, SIGNAL(triggered()), this, SLOT(primaryActionTriggeredSlot()));
    }
    QString headingTxt("");
    QString primaryTxt("");
    // create texts
    switch(mTypeOfSelection)
    {
        case EDeleteVideos:
            primaryTxt = hbTrId("txt_common_button_delete");
        break;
        case EAddToCollection:
            primaryTxt = hbTrId("txt_common_button_add");
        break;
        case ECreateCollection:
            primaryTxt = hbTrId("txt_common_button_ok");
        break;
        case ERemoveFromCollection:
            primaryTxt = hbTrId("txt_common_button_remove");
        break;
        case ESelectCollection:
            primaryTxt = hbTrId("txt_videos_button_new");
        break;
        default:
        break;
    }

    mPrimaryAction->setText(primaryTxt);
    
    mModelReady = false;
    mAlbumListReady = false;

    if(mTypeOfSelection == ESelectCollection)
    {
        headingTxt = hbTrId("txt_videos_title_select_collection");
        mCheckboxContainer->setVisible(false);
        mListWidget->setSelectionMode(-1);   
        mListWidget->activate(VideoCollectionCommon::ELevelCategory);
        // New collection button is always enabled for collection selection.
        mPrimaryAction->setDisabled(false);
    }
    else
    {
        headingTxt = hbTrId("txt_videos_title_select_videos");
        mListWidget->setSelectionMode(HbAbstractItemView::MultiSelection);
        mListWidget->activate(VideoCollectionCommon::ELevelVideos);
        mCheckboxContainer->setVisible(true);
        mItemCount->setPlainText(tr("0/%1").arg(mModel->rowCount())); 
        mCheckBox->setChecked(false);
        
        // Add button will be enabled when videos are selected from the list.
        mPrimaryAction->setDisabled(mTypeOfSelection != ECreateCollection);
    }
    mHeading->setPlainText(headingTxt);         
    // sort to make sure dialog has correctly filtered content
    // at the same order as in view
    VideoCollectionViewUtils::sortModel(mModel, false, mListWidget->getLevel());        
}

// ---------------------------------------------------------------------------
// exec
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::exec()
{
	FUNC_LOG;
    // clear checkbox
    mCheckBox->setChecked(false);
    
    mListWidget->clearSelection(); 
    
    // scroll list back to top
    mListWidget->scrollTo(mModel->index(0, 0));
    
    connectSignals();

    if(mTypeOfSelection == ECreateCollection)
    {
        // note this does not leak memory as the dialog will destroy itself upon close.
        HbInputDialog *dialog = gCreateNewAlbumNameDialog(LIST_VIEW_OBJECT_NAME_CREATE_COLLECTION);
        dialog->open(this, SLOT(newAlbumNameDialogFinished(HbAction *)));
    }
    else if(mModel->rowCount())
    {
        // show dialog
        HbDialog::open();
    }
    else if((mModelReady && mTypeOfSelection != ESelectCollection) || 
            (mAlbumListReady && mTypeOfSelection == ESelectCollection))
    {
        INFO("VideoListSelectionDialog::exec(): nothing to show, finishing.")
        // no items, finish right away
        finishedSlot(mPrimaryAction);
    }
}

// ---------------------------------------------------------------------------
// finishedSlot
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::finishedSlot(HbAction *action)
{
	FUNC_LOG;
    disconnectSignals();
    if(action == mSecondaryAction)
    {
        INFO("VideoListSelectionDialog::exec(): secondary action triggered.")
        return;
    }
    
    if(mTypeOfSelection == ESelectCollection)
    {
        mTypeOfSelection = EAddToCollection;
        if(mSelectedAlbumId == TMPXItemId::InvalidId())
        {
            close(); // closes selection dialog if it's open.
            
            // note this does not leak memory as the dialog will destroy itself upon close.
            HbInputDialog *dialog = gCreateNewAlbumNameDialog(SELECTION_DIALOG_OBJECT_NAME_NEW_COLLECTION);
            dialog->open(this, SLOT(newAlbumNameDialogFinished(HbAction *)));
       }
       else
       {
           finalize();
       }
    }
    else
    {
        finalize();
    }
}

// ---------------------------------------------------------------------------
// newAlbumNameDialogFinished
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::newAlbumNameDialogFinished(HbAction *action)
{
    FUNC_LOG;
    Q_UNUSED(action);
    
    HbInputDialog *dialog = static_cast<HbInputDialog*>(sender());
    
    QVariant variant = dialog->value();
    
    if(dialog->actions().first() == action && variant.isValid())
    {
        QString text = mModel->resolveAlbumName(variant.toString());
        if(!text.isEmpty())
        {
            if(mSelectedVideos.count() == 0 && mModel->rowCount())
            {
                mNewAlbumText = text;
                // show video selection dialog
                HbDialog::open();
            }
            else
            {
                mSelectedAlbumId = mModel->addNewAlbum(text);
                finalize(text);
            }
        }
        else
        {
            // new album name empty, effectively cancel, so disconnect signals.
            disconnectSignals();
        }
    }
    else
    {
        // new album dialog cancelled, disconnect signals.
        disconnectSignals();
    }
}

// ---------------------------------------------------------------------------
// finalize
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::finalize(QString albumName)
{
    // Must be checked again if type was ESelectCollection
    if(mTypeOfSelection == EAddToCollection ||
       mTypeOfSelection == ECreateCollection)
    {
        if(mTypeOfSelection == ECreateCollection)
        {
            mSelectedAlbumId = mModel->addNewAlbum(mNewAlbumText);
            albumName = mNewAlbumText;
        }
        
        if(mSelectedAlbumId != TMPXItemId::InvalidId() && mSelectedVideos.count())
        {
            if(mModel->addItemsInAlbum(mSelectedAlbumId, mSelectedVideos.toList()) == 0)
            {
                if(!albumName.length())
                {
                    albumName = getSelectedName();
                }
                QList<QVariant> dataList;
                dataList.append(QVariant(mSelectedVideos.count()));
                dataList.append(QVariant(albumName));
                QVariant data = dataList;
                VideoCollectionViewUtils::instance().showStatusMsgSlot(
                        VideoCollectionCommon::statusVideosAddedToCollection,
                        data);
            }
        }
    }
    else if(mTypeOfSelection == ERemoveFromCollection)
    {
        if(mSelectedAlbumId != TMPXItemId::InvalidId() && mSelectedVideos.count())
        {
            if(mModel->removeItemsFromAlbum(mSelectedAlbumId, mSelectedVideos.toList()) == 0)
            {
                QVariant data = getSelectedName();
                VideoCollectionViewUtils::instance().showStatusMsgSlot(
                        VideoCollectionCommon::statusVideosRemovedFromCollection,
                        data);
            }
        }
    }
    // user is deleting videos
    else if(mTypeOfSelection == EDeleteVideos)
    {
        QVariant data = mSelection.indexes().count();
        VideoCollectionViewUtils::instance().showStatusMsgSlot(
                VideoCollectionCommon::statusDeleteInProgress,
                data);
        // delete items            
        mModel->deleteItems(mSelection.indexes());
    } 
}

// ---------------------------------------------------------------------------
// markAllStateChanged
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::markAllStateChangedSlot(int state)
{ 
	FUNC_LOG;
    if(mForcedCheck)
    {
        return;
    }
    if(state == Qt::Checked)
    {
        mListWidget->selectAll();
    }
    else
    {
        mListWidget->clearSelection();
    }   
}

// ---------------------------------------------------------------------------
// selectionChangedSlot
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::selectionChangedSlot(const QItemSelection &selected, const QItemSelection &deselected)
{  
	FUNC_LOG;
    if(!mPrimaryAction || mTypeOfSelection == ESelectCollection)
    {
        INFO("VideoListSelectionDialog::selectionChangedSlot(): no primary action or we are selecting collection.")
        return;
    }
   
    if(selected.indexes().count() > 0)
    {
        mSelection.merge(selected, QItemSelectionModel::Select);
    }
    
    if(deselected.indexes().count() >  0)
    {
        mSelection.merge(deselected, QItemSelectionModel::Deselect);
    }
    
    if(mSelection.indexes().count() > 0)
    {
        mPrimaryAction->setDisabled(false);
        
        if(mTypeOfSelection == ECreateCollection)
        {
            mPrimaryAction->setText(hbTrId("txt_common_button_add"));
        }
    }
    else
    {
        mPrimaryAction->setDisabled(mTypeOfSelection != ECreateCollection);
        
        if(mTypeOfSelection == ECreateCollection)
        {
            mPrimaryAction->setText(hbTrId("txt_common_button_ok"));
        }
    }    

    updateCounterSlot();
}

// ---------------------------------------------------------------------------
// singleItemSelectedSlot
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::singleItemSelectedSlot(const QModelIndex &index)
{
	FUNC_LOG;
    if(mTypeOfSelection == ESelectCollection)
    {
       if(index.isValid())
       {
           mSelectedAlbumId = mModel->getMediaIdAtIndex(index);            
           mPrimaryAction->trigger();
       }
    }
}

// ---------------------------------------------------------------------------
// modelReadySlot
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::modelReadySlot()
{
	FUNC_LOG;
    mModelReady = true;
    
	if(mTypeOfSelection != ESelectCollection)
	{
        if(!mModel->rowCount())
        {
            // no items, finish right away
            INFO("VideoListSelectionDialog::modelReadySlot(): nothing to show, finishing.");
            
            if(mTypeOfSelection == EAddToCollection ||
               mTypeOfSelection == ERemoveFromCollection)
            {
                primaryActionTriggeredSlot();
            }
            else
            {
                finishedSlot(mPrimaryAction);
            }
            
            return;
        }
            
        // if dialog is not yet visible, bring it visible. 
        if(!isVisible() && mTypeOfSelection != ECreateCollection)
        {
            // scroll list back to top
            mListWidget->scrollTo(mModel->index(0, 0));
            
            HbDialog::open();
        }
        
        updateCounterSlot();
	}
}

// ---------------------------------------------------------------------------
// albumListReadySlot
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::albumListReadySlot()
{
    mAlbumListReady = true;
    
    if(mTypeOfSelection == ESelectCollection)
    {
        if(!mModel->rowCount())
        {
            finishedSlot(mPrimaryAction);
            return;
        }
            
        // if dialog is not yet visible, bring it visible. 
        if(!isVisible())
        {
            // scroll list back to top
            mListWidget->scrollTo(mModel->index(0, 0));
            
            HbDialog::open();
        }
        
        updateCounterSlot();
    }
}

// ---------------------------------------------------------------------------
// updateCounterSlot
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::updateCounterSlot()
{
	FUNC_LOG;
    if(!mItemCount)
    {
        return;
    }

    int rowCount = mModel->rowCount();       
    int selectionCount = mSelection.indexes().count();
    mItemCount->setPlainText(tr("%1/%2").arg(selectionCount).arg(rowCount));  
    mForcedCheck = true;
    if(selectionCount == rowCount && rowCount > 0)
    {     
        mCheckBox->setChecked(true);
    }
    else
    {
        mCheckBox->setChecked(false);
    }
    mForcedCheck = false;
}

// ---------------------------------------------------------------------------
// primaryActionTriggeredSlot
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::primaryActionTriggeredSlot()
{
	FUNC_LOG;
    // reconnect primary action
    mPrimaryAction->disconnect(SIGNAL(triggered()));
    connect(mPrimaryAction, SIGNAL(triggered()), this, SLOT(close()));

    // update video items selection here before content changes.
    QModelIndexList indexes = mSelection.indexes();
    int count = indexes.count();
    TMPXItemId id = TMPXItemId::InvalidId();
    for(int i = 0; i < count; ++i)
    {
        id = mModel->getMediaIdAtIndex(indexes.at(i));
        if(id.iId2 == KVcxMvcMediaTypeVideo)
        {
            mSelectedVideos.insert(id);
        }
    }
    
    if(mSelectedAlbumId != TMPXItemId::InvalidId() ||
       mTypeOfSelection == ECreateCollection)
    {
        INFO("VideoListSelectionDialog::primaryActionTriggeredSlot(): closing dialog.")
        mPrimaryAction->trigger();
        return;
    }
    else if(mTypeOfSelection == EAddToCollection)
    {
        INFO("VideoListSelectionDialog::primaryActionTriggeredSlot(): activating collection selection.")
        // videos for collection selected, but collection 
        // not yet selected, activate selection for it
        mTypeOfSelection = ESelectCollection;
        activateSelection();
   }   
}

// ---------------------------------------------------------------------------
// connectSignals
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::connectSignals()
{   
	FUNC_LOG;
    // dialog finished
    connect(this, SIGNAL(finished(HbAction*)), this, SLOT(finishedSlot(HbAction*)));
    
    // selection changes
    connect(mListWidget, SIGNAL(activated(const QModelIndex&)),
            this, SLOT(singleItemSelectedSlot(const QModelIndex&)));
    
    connect(mListWidget->selectionModel(), SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
            this, SLOT(selectionChangedSlot(const QItemSelection&, const QItemSelection &)));

    // model changes signals
    connect(mModel->sourceModel(), SIGNAL(modelReady()), this, SLOT(modelReadySlot()));
    connect(mModel->sourceModel(), SIGNAL(albumListReady()), this, SLOT(albumListReadySlot()));
    connect(mModel->sourceModel(), SIGNAL(modelChanged()), this, SLOT(updateCounterSlot()));
    
    // mark all state changes
    connect(mCheckBox, SIGNAL(stateChanged(int)), this, SLOT(markAllStateChangedSlot(int)));
}
 
// ---------------------------------------------------------------------------
// disconnectSignals
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::disconnectSignals()
{   
	FUNC_LOG;
    disconnect(this, SIGNAL(finished(HbAction*)), this, SLOT(finishedSlot(HbAction*)));
    
    disconnect(mListWidget, SIGNAL(activated(const QModelIndex&)),
                this, SLOT(singleItemSelectedSlot(const QModelIndex&)));
    
    disconnect(mListWidget->selectionModel(), SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
                            this, SLOT(selectionChangedSlot(const QItemSelection&, const QItemSelection &)));

    // model changes signals
    disconnect(mModel->sourceModel(), SIGNAL(modelReady()), this, SLOT(modelReadySlot()));    
    disconnect(mModel->sourceModel(), SIGNAL(albumListReady()), this, SLOT(albumListReadySlot()));
    disconnect(mModel->sourceModel(), SIGNAL(modelChanged()), this, SLOT(updateCounterSlot()));   
    
    // mark all state changes
    disconnect(mCheckBox, SIGNAL(stateChanged(int)), this, SLOT(markAllStateChangedSlot(int)));
}

// ---------------------------------------------------------------------------
// getSelectedName
// ---------------------------------------------------------------------------
//
QString VideoListSelectionDialog::getSelectedName()
{
	FUNC_LOG;
    QString name;
    if(mSelectedAlbumId.iId2 > KVcxMvcMediaTypeVideo)
    {
        VideoProxyModelGeneric *model = 
                VideoCollectionWrapper::instance().getGenericModel();
        if(!model)
        {
            return name;
        }
        QModelIndex index = model->indexOfId(mSelectedAlbumId);
        if(index.isValid())
        {
            name = model->data(index, VideoCollectionCommon::KeyTitle).toString();
        }
    }
    return name;
}

// End of file
