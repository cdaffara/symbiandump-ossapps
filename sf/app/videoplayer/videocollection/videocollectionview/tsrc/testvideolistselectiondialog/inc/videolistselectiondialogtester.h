/**
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
* Description:   VideoListSelectionDialogTester class
* 
*/

#ifndef __VIDEOLISTSELECTIONDIALOGTESTER_H__
#define __VIDEOLISTSELECTIONDIALOGTESTER_H__

#include <qpointer.h>
#include "videolistselectiondialog.h"

class VideoListSelectionDialogTesterHelper : public QObject
{
    Q_OBJECT
    
signals:
    
    void markAllSignal(int);
    
    void selectionChangedSignal(const QItemSelection&, const QItemSelection&);
    
    void singleItemSelectedSignal(const QModelIndex&);
    
    void modelReadySignal();
    
    void updateCounterSignal();
    
    void primaryActionTriggeredSignal();
    
    void finishedSignal(HbAction*);
    
public:
    
    VideoListSelectionDialogTesterHelper(VideoListSelectionDialog *testable = 0) 
    : mTestable(testable), mSignalsConnected(false) {};
    
    ~VideoListSelectionDialogTesterHelper() {};
    
    bool connectSignals()
    {
        if(!mTestable || mSignalsConnected)
        {
            return false;
        }
        mSignalsConnected = true;

        if(!connect(this, SIGNAL(markAllSignal(int)), mTestable, SLOT(markAllStateChangedSlot(int))))
        {
            return false;
        }
        if(!connect(this, SIGNAL(selectionChangedSignal(const QItemSelection&, const QItemSelection&)), 
                mTestable, SLOT(selectionChangedSlot(const QItemSelection&, const QItemSelection&))))
        {
            return false;
        }
        if(!connect(this, SIGNAL(singleItemSelectedSignal(const QModelIndex&)), 
                mTestable, SLOT(singleItemSelectedSlot(const QModelIndex&))))
        {
            return false;
        }
        if(!connect(this, SIGNAL(modelReadySignal()), mTestable, SLOT(modelReadySlot())))
        {
            return false;
        }
        if(!connect(this, SIGNAL(updateCounterSignal()), mTestable, SLOT(updateCounterSlot())))
        {
            return false;
        }
        if(!connect(this, SIGNAL(primaryActionTriggeredSignal()), 
                mTestable, SLOT(primaryActionTriggeredSlot())))
        {
            return false;
        }
        if(!connect(this, SIGNAL(finishedSignal(HbAction*)), mTestable, SLOT(finishedSlot(HbAction*))))
        {
            return false;
        }
        return true;
    }
    
    void disconnectSignals()
    {
        if(mTestable && mSignalsConnected)
        {
            mSignalsConnected = false;
            disconnect(this, SIGNAL(markAllSignal(int)), mTestable, SLOT(markAllStateChangedSlot(int)));
            disconnect(this, SIGNAL(selectionChangedSignal(const QItemSelection&, const QItemSelection&)), 
                            mTestable, SLOT(selectionChangedSlot(const QItemSelection&, const QItemSelection&)));
            disconnect(this, SIGNAL(singleItemSelectedSignal(const QModelIndex&)), 
                            mTestable, SLOT(singleItemSelectedSlot(const QModelIndex&)));
            disconnect(this, SIGNAL(modelReadySignal()), mTestable, SLOT(modelReadySlot()));
            disconnect(this, SIGNAL(updateCounterSignal()), mTestable, SLOT(updateCounterSlot()));
            disconnect(this, SIGNAL(primaryActionTriggeredSignal()), 
                            mTestable, SLOT(primaryActionTriggeredSlot()));
            disconnect(this, SIGNAL(finishedSignal(HbAction*)), mTestable, SLOT(finishedSlot(HbAction*)));
        }
    }
    
    void emitMarkAllStateChanged(int state)
    {
        emit markAllSignal(state);
    }

    void emitSelectionChangedSlot(const QItemSelection &selected, const QItemSelection &deselected)
    {
        emit selectionChangedSignal(selected, deselected);
    }
    
    void emitSingleItemSelectedSlot(const QModelIndex &index)
    {
        emit singleItemSelectedSignal(index);
    }
    
    void emitModelReadySlot()
    {
        emit modelReadySignal();
    }
    
    void emitUpdateCounterSlot()
    {
        emit updateCounterSignal();
    }
    
    void emitPrimaryActionTriggeredSlot()
    {
        emit primaryActionTriggeredSignal();
    }
    
    void emitFinishedSlot(HbAction *action)
    {
        emit finishedSignal(action);
    }
    
    QPointer<VideoListSelectionDialog> mTestable;
    
    /**
     * flag indicating if signals are connected. 
     */
    bool mSignalsConnected;    
};

#endif
