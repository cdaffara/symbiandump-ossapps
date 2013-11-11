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
* Description:   TestVideoFileDetails class definition
* 
*/

#ifndef TESTVIDEOFILEDETAILS_H
#define TESTVIDEOFILEDETAILS_H


// INCLUDES
#include <qobject>
#include <qabstractitemmodel.h>
#include <mpxitemid.h>

class VideoFileDetailsViewPlugin;
class VideoProxyModelGeneric;
class VideoCollectionWrapper;

class TestVideoFileDetails : public QObject
	{
    
    Q_OBJECT

public: 
    
    /**
     * Used in testfunctions to init
     */
    void init();
    
    /**
     * used in testfunctions to cleanup
     */
    void cleanup();
    
    /**
     * common view activation call
     */
    void activateView();
    
    /**
     * creates some metadata to be "shown" in tests 
     */
    QMap<QString, QVariant> createDummyMetadata();
    
    /**
     * runned when checking view activation related things
     */
    void activateViewTestRound();

signals: // test signals to emit

    /**
     * short details test signal
     */
    void shortDetailsReady(TMPXItemId index);
    
    /**
     * full details test signal
     */
    void fullDetailsReady(QVariant& variant);
    
    /**
     * data changed test signal
     */
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    
    /**
     * Row removal test signal
     */
    void rowsRemoved(const QModelIndex &parent, int first, int last);
    
    /**
     * generic test signal
     */
    void testSignal();
    
    /**
     * for testing error signal slots
     */
    void testErrorSignal(int, QVariant&);
    
public slots:
    
    void handleCommand(int);
   
private slots:
    
    /**
     * called at the very beginning of the test
     */
    void initTestCase();
    
    /**
     * called at the very end of the test
     */
    void cleanupTestCase();

    /**
     * verifies createView -method
     */
    void testCreateView();
    
    /**
     * verifies destroyView -method
     */
    void testDestroyView();
    
    /**
     * verifies activate in portrait mode
     */
    void testActivateViewPortrait();
    
    /**
     * verifies activate in landscape
     */
    void testActivateViewLandscape();
    
    /**
     * verifies deactivateView
     */
    void testDeactivateView();
    
    /**
     * verifies orientation changed datas
     */
    void testOrientationChange_data();
    
    /**
     * veries orienation changed method
     */
    void testOrientationChange();
    
    /**
     * verifies back -method
     */
    void testBack();
    
    /**
     * verifies shortdetails ready
     */
    void testShortDetailsReadySlot();
    
    /**
     * verifies fulldetailsready
     */
    void testFullDetailsReadySlot();
    
    /**
     * verifies startPlayback
     */
    void testStartPlaybackSlot();
    
    /**
     * verifies sendvideo -method
     */
    void testSendVideoSlot();
    
    /**
     * verifies delete video
     */
    void testDeleteVideoSlot();
    
    /**
     * verifies rowsRemoved
     */
    void testRowsRemovedSlot();
    
    /**
     * verifies error handling
     */
    void testHandleErrorSlot();
    
    /**
     * verifies thumbnail ready.
     */
    void testThumbnailReadySlot();

private:
    
    /**
     * widget finder used when reading objetc from XML
     */
    template <class T>
    T* findWidget(QString name);
    
    /**
     * Qobject finder used when reading objetc from XML
     */
    template<class T>
    T* findObject(QString name);
    
private:
    
    /**
     * stub model
     */
    VideoProxyModelGeneric* mDummyModel;
    
    /**
     * Testable object
     */
    VideoFileDetailsViewPlugin* mPlugin;
    
    /**
     * indicate client has emitted signle received 
     * in handleCommand
     */
    bool mCommandReceived;
    
    /**
     * id of command recently received
     */
    int mReceivedCommand;
    
public:
    
    VideoCollectionWrapper *mWrapper;
    
    };

#endif  // TESTVIDEOFILEDETAILS_H

// End of File
