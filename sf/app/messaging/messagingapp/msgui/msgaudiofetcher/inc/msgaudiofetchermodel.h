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
 *     The header file for messaging's audio fetcher model
 *     
 */

#ifndef MSGAUDIOFETCHERMODEL_H
#define MSGAUDIOFETCHERMODEL_H

// SYSTEM INCLUDES
#include <QStandardItemModel>
#include <QStringList>
#include <QDir>

// FORWARD DECLARATIONS
class MsgAudioSelectionEngine;

/**
 * @class MsgAudioFetcherModel
 * @brief This class is messaging's audio fetcher model
 */
class MsgAudioFetcherModel : public QStandardItemModel
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    explicit MsgAudioFetcherModel(QObject *parent);

    /**
     * Destructor
     */
    virtual ~MsgAudioFetcherModel();

    /**
     * Clears model
     */
    void clearAll();
    
    /**
     * adds a row into the model
     */
    void addRow(QString filepath);

public: //from QAbstractItemModel
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private:
    /**
     * binary search (ascendant) for the correct index to insert.
     * @param low the start of search
     * @param high the end of search.
     * @return the correct index
     */
    int insertIndex(int low, int high, QString variant);

    /**
     * Add audio files into the model
     * @param toneDir, Dir from which to get audio files to add to model
     */
    void addToneFiles(QDir& toneDir);
    
    /**
     * Finds and adds audio files into the model
     */
    void getRomToneFiles();

    /**
     * Initialize the model
     */
    void init();
    
    /**
     * checks if the given file is DRM protected
     * @param filepath, path of audio file
     */
    bool isDRM(QString filepath);

private slots:
    /**
     * Handle MDS session open
     */
    void mdeSessionOpened();

    /**
     * Handle MDS session open error
     */
    void mdeSessionError(int error);
    
    /**
     * Handle MDS query complete
     */
    void queryComplete(const QStringList &nameList,
            const QStringList &uriList);

    /**
     * Handle MDS query error
     */
    void queryError(int error);

    /**
     * Handle object observer callback
     */
    void onObjectChanged();

private:
    /**
     * audio selection engine
     */
    MsgAudioSelectionEngine* mSelectionEngine;
#ifdef AUDIO_FETCHER_UNIT_TEST
    /**
     * Unit Testing
     */
    friend class TestMsgAudioFetcherModel;
#endif
    
};

#endif /* MsgAudioFetcherModel_H_ */
