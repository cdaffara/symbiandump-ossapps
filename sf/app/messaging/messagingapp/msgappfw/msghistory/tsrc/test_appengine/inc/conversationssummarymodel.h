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

#ifndef CONVERSATIONS_SUMMARY_MODEL_H
#define CONVERSATIONS_SUMMARY_MODEL_H

// INCLUDES
#include <QObject>
#include <QStandardItemModel>
#include <ccsdefs.h>

// FORWARD DECLARATIONS
class CCsClientConversation;

/**
 * This class implements the model of the model/view framework.
 * The model holds data for the conversations list fetched from the
 * server.
 *
 * Data from server is mapped to UI via Model
 *
 */

class ConversationsSummaryModel : public QStandardItemModel
{
    Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit ConversationsSummaryModel(QObject* parent = 0);

public:

    /**
     * Destructor
     */
    virtual ~ConversationsSummaryModel();

    /**
     * Method for mapping the data in the Model to the UI
     * @param index index for which data needs to be fetched
     * @param role defined for the data
     * @return QVariant as the data
     */
    QVariant data(const QModelIndex & index, int role) const;

    /**
     * Add a new row to the conversation summary model.
     * Overwrite the row if the message already exists.
     * @param conversation, CCsClientConversation
     * 
     */    
    void addRow(const CCsClientConversation& conversation);

    /**
     * Delete a row from conversation summary model.
     * @param conversationId, conversation Id
     */        
    void deleteRow(qint64 conversationId);  
    
private:
   
    /**
     * Populate an item in model from CCsClientConversation.
     * @param item, QStandardItem
     * @param conversation, CCsClientConversation
     * 
     */    
    void populateItem(QStandardItem& item,
            const CCsClientConversation& conversation);   
         
};

#endif // CONVERSATIONS_SUMMARY_MODEL_H
