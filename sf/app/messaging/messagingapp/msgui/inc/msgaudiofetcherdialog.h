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
 *     The header file for messaging's audio fetcher view.
 *
 */

#ifndef MSGAUDIOFETCHERDIALOG_H
#define MSGAUDIOFETCHERDIALOG_H

#ifdef AUDIO_FETCHER_UNIT_TEST
 #define MSGAUDIOFETCHER_DLL_EXPORT
#else
 #ifdef  BUILD_MSGAUDIOFETCHER_DLL
  #define MSGAUDIOFETCHER_DLL_EXPORT Q_DECL_EXPORT
 #else
  #define MSGAUDIOFETCHER_DLL_EXPORT Q_DECL_IMPORT
 #endif
#endif

// SYSTEM INCLUDES
#include <QObject>
#include <hblistwidget.h>
#include <HbDialog>

// FORWARD DECLARATIONS
//class MsgAudioFetcherWidget;
class HbAction;
class HbListView;
class MsgAudioFetcherModel;
class MsgAudioPreview;

/**
 * @class MsgAudioFetcherView
 * @brief This class is messaging's audio fetcher view
 */
class MSGAUDIOFETCHER_DLL_EXPORT MsgAudioFetcherDialog : public HbDialog
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    explicit MsgAudioFetcherDialog(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~MsgAudioFetcherDialog();  
        
private:
    /**
     * Creates a widget and sets it to the view
     */
    void initMainWidget();

    /**
     * Initializes dialog actions
     */
    void initActions();   
    
    /**
     * Returns the selected audio path
     */
    QString getCurrentItemPath();

signals:

    /**
     * Emits when audio selected from dialog
     */
    void audioSelected(QString& filePath);
    
private slots:

    /**
     * On View Ready signal do some construction
     */
    void doDelayedConstruction();
    
    /**
     * Handles toolbar's leftaction (select)
     */
    void onSelectAction();

    /**
     * Handles toolbar's rightaction (cancel)
     */
    void onCancelAction();

    /**
     * On close of dialog this resets the item
     */
    void reset();
    
    /**
     * This gets called after list item gets 
     * activated
     */
    void on_list_activated(const QModelIndex &index);

private:
    
    /**
     * List for displaying the audio list
     */
    HbListView* mListView;
    
    /**
     * Model for the List View
     */
    MsgAudioFetcherModel* mFetcherModel;
    
    /**
     * Audio preview instance
     */
    MsgAudioPreview* mAudioPreview;
    
    /**
     * Toolbar's left SELECT action
     */
    HbAction *mLeftAction;
    
    /**
     * last selected item
     */
    QModelIndex mSeletedItem;
    
    /**
     * This to enable the left action in dialog
     */
    bool mSelected;
#ifdef AUDIO_FETCHER_UNIT_TEST
    /**
     * Unit Testing
     */
    friend class TestMsgAudioFetcherDialog;
#endif
};

#endif /* MSGAUDIOFETCHERVIEW_H */
