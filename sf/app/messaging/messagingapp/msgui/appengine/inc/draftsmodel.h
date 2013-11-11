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
 * Description: model for drafts view.
 *
 */

#ifndef DRAFTS_MODEL_H
#define DRAFTS_MODEL_H

// INCLUDES
#include <QObject>
#include <QStandardItemModel>
#include <ccsdefs.h>
#include "convergedmessage.h"

// FORWARD DECLARATIONS
class TMsvEntry;

/**
 * This class implements the model of the model/view framework.
 * The model holds data for the drafts view fetched from the
 * server.
 *
 * Data from server is mapped to UI via Model
 *
 */

class DraftsModel : public QStandardItemModel
{
    Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit DraftsModel(QObject* parent = 0);

public:

    /**
     * Destructor
     */
    virtual ~DraftsModel();

    /**
     * Method for mapping the data in the Model to the UI
     * @param index index for which data needs to be fetched
     * @param role defined for the data
     * @return QVariant as the data
     */
    QVariant data(const QModelIndex & index, int role) const;

    /**
     * Add a new row to thedrafts model.
     * Overwrite the row if the message already exists.
     * @param entry, TMsvEntry
     * 
     */    
    void addRow(const TMsvEntry& entry);

    /**
     * Delete a row from drafts model.
     * @param msgId, message Id.
     */        
    void deleteRow(int msgId);  
    
    /**
     * sets model flag to true after initail fetch completion.
     */
    void setReady();
    
    /*
     * Returns true if initial fetch is completed.
     */
    bool isReady();
    
private:
   
    /**
     * Populate an item in model from TMsvEntry.
     * @param item, QStandardItem
     * @param entry, TMsvEntry
     * 
     */    
    void populateItem(QStandardItem& item,
            const TMsvEntry& entry);       
   
private:
   
    /*
     * Flag set after initail data fetch completion.
     * for dynamic event handling.
     */
    bool mReady;
         
};

#endif // DRAFTS_MODEL_H
