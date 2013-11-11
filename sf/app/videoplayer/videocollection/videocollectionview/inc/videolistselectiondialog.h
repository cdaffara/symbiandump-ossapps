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
* Description:   Videolist selection dialog 
* 
*/

#ifndef VIDEOLISTSELECTIONDIALOG_H
#define VIDEOLISTSELECTIONDIALOG_H

#include <hbdialog.h>
#include <qset.h>
#include <qitemselectionmodel.h>
#include <mpxitemid.h>

class QGraphicItem;
class QItemSelection;
class HbWidget;
class HbLabel;
class HbCheckBox;
class HbStackedWidget;
class VideoCollectionUiLoader;
class VideoCollectionWrapper;
class VideoProxyModelGeneric;
class VideoListWidget;

class VideoListSelectionDialog: public HbDialog
{     
    /**
     * definition required for emitting / connecting signals 
     */
    Q_OBJECT
    
    /**
     * disable copy-constructor and assignment operator
     */
    Q_DISABLE_COPY(VideoListSelectionDialog) 

    
public: 
    
    enum TSelectionFunction
    {
        EDeleteVideos,
        EAddToCollection,        
        ERemoveFromCollection,
        ECreateCollection,
        ESelectCollection
    };
    
    /**
     * Constructor
     *
     * @param uiLoader used to load UI components from docml
     * @param parent item's parent component
     */
    VideoListSelectionDialog(VideoCollectionUiLoader *uiLoader, QGraphicsItem *parent=0);
    
    /**
     * Destructor
     */
    ~VideoListSelectionDialog();
   
    /**
     * Method setups provided title and videolist content widget.
     * Prepares dialog for showing.
     * If either provided title is empty or widget is NULL, dialog
     * is in invalid state and cannot be shown using exec.
     *
     * @param type selection dialog function typy
     * @param activeItem id of item that the selection concerns
     */
    void setupContent(int type, TMPXItemId activeItem = TMPXItemId::InvalidId());
    
public slots:

    /**
     * Shows modal dialog build from the provided Videolistwidget.
     *
     */
    void exec();
    
    /**
     * Signaled by HbInputDialog when it's closed.
     */
    void newAlbumNameDialogFinished(HbAction *action);    
    
private slots:
    
    /**
     * Slot connected to mCheckBox's stateChanged -signal. 
     * Selects or deselects all items if mForcedCheck -flag is false.
     * 
     * If Forcedcehck is true, it indeicates that we want to change the checked state
     * only, not slect or deselect anything. This is needed for example if after 
     * all items are marked, user manually deselects item's. In that case all is not
     * marked, so checked state needs to be changed. 
     *
     * @param state Qt::Checked everything should be set as selected.
     */
    void markAllStateChangedSlot(int state);
    
    /**
     * disables or enables popup's primary action in case selection changes
     *
     * @param selected item selection list containing selected
     * @param deselected item selection list containing  not selected (not used in this scope)
     */
    void selectionChangedSlot(const QItemSelection &selected, const QItemSelection &deselected); 
    
    /**
     * connected to list's activated -signal. Called when singel item is selected.
     * If dialog's selection type is ESelectCollection, saves mpx id from provided index.
     * and closes dialog by triggering primary action
     *
     * @param index of item selected
     */
    void singleItemSelectedSlot(const QModelIndex &index); 
    
    /**
     * Handles model ready signal from model.
     */
    void modelReadySlot();
    
    /**
     * Handles album list ready signal from model.
     */
    void albumListReadySlot();
    
    /**
     * Changes the counter value. Sets the checkbutton state based on selection count.
     *
     */
    void updateCounterSlot();
    
    /**
     * Primary action triggered signal slot.
     * 
     */
    void primaryActionTriggeredSlot();
    
    /**
     * called when dialog has been finished
     * 
     * @param action causing the finished
     */
    void finishedSlot(HbAction *action);
      
private:
    
    /**
     * Method initializes dialog by getting all ui components and 
     * checking if they are valid
     * 
     * @return bool true if inisializaion ok
     */
    bool initDialog();
    
    /**
     * Method activated dialog based on value in mTypeOfSelection
     */
    void activateSelection();
    
    /**
     * connects all required signals into appropriate slots 
     * for selection mode
     */
    void connectSignals();
    
    /**
     * disconnects all signals 
     */
    void disconnectSignals();
    
    /**
     * Gets selected item's name from appropriate model.
     * 
     * @return QString item's name
     */
    QString getSelectedName();
    
    /**
     * Finalizes the operation.
     */
    void finalize(QString albumName = "");
    
private:
   
    /**
     * docml UI loader, not owned
     */
    VideoCollectionUiLoader *mUiLoader;
    
    /**
     * type for defining selection functionality
     */
    int mTypeOfSelection;
    
    /**
     * Selection 
     */
    QItemSelection mSelection;
    
    /**
     * Selected video items
     */
    QSet<TMPXItemId> mSelectedVideos;
    
    /**
     * selected single items album id
     */
    TMPXItemId mSelectedAlbumId;
    
    /**
     * Text for a new album.
     */
    QString mNewAlbumText;
    
    /**
     * header label
     */
    HbLabel *mHeading;
    
    /**
     * container widget for itemcount and checkbox;
     */
    HbWidget *mCheckboxContainer;
    
    /**
     * counter label from docml
     */
    HbLabel *mItemCount;
    
    /**
     * checkbox label from docml
     */
    HbLabel *mCheckBoxText;
    
    /**
     * mark all checkbox
     */
    HbCheckBox *mCheckBox;
    
    /**
     * videolist container in dialog
     */
    HbStackedWidget *mListContainer;
    
    /**
     * flag indicating that we've changed check-btn state 
     * explicitly and don't wanna handle selection based on that 
     */
    bool mForcedCheck;  
    
    /**
     * Video list model
     */
    VideoProxyModelGeneric *mModel;
    
    /**
     * Boolean to know if the model is ready or not. 
     */
    bool mModelReady;
    
    /**
     * Boolean to know if album list is ready or not.
     */
    bool mAlbumListReady;
    
    /** 
     * List widget to show in selection
     */
    VideoListWidget *mListWidget;
    
    /**
     * Action to accept the selection dialog. 
     */
    HbAction *mPrimaryAction;
    
    /**
     * Action to cancel the selection dialog.
     */
    HbAction *mSecondaryAction;
};

#endif  //VIDEOLISTSELECTIONDIALOG_H
