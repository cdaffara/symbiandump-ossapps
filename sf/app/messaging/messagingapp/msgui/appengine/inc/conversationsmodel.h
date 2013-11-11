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

#ifndef CONVERSATIONS_MODEL_H
#define CONVERSATIONS_MODEL_H

// INCLUDES
#include <QObject>
#include <QStandardItemModel>
#include <ccsdefs.h>
#include <sqldb.h>
#include <QCache>
#include <HbIcon>

// FORWARD DECLARATIONS
class CCsConversationEntry;
class ConversationMsgStoreHandler;
class UniDataModelLoader;
class UniDataModelPluginInterface;

/**
 * This class implements the model of the model/view framework.
 * The model holds data for the conversations fetched from the
 * server.
 *
 * Data from server is mapped to UI via Model
 *
 */

class ConversationsModel: public QStandardItemModel
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit ConversationsModel(ConversationMsgStoreHandler* msgStoreHandler, QObject* parent = 0);

    /**
     * Destructor
     */
    virtual ~ConversationsModel();

    /**
     * Method for mapping the data in the Model to the UI
     * @param index index for which data needs to be fetched
     * @param role defined for the data
     * @return QVariant as the data
     */
    QVariant data(const QModelIndex& index, int role) const;

    /**
     * Add a new row to the conversations model.
     * Overwrite the row if the message already exists.
     * @param entry, CCsConversationEntry
     * @param dynamicUpdate , true if it is a dynamic update event
     */
    void addRow(const CCsConversationEntry& entry, bool dynamicUpdate = false);

    /**
     * Delete a row from conversations model.
     * @param msgId, Message Id
     */
    void deleteRow(int msgId);
    
    /*
     * Get SQL DB handle
     * @param isOpen, set to true if open, check this before using the handle
     */
    RSqlDatabase& getDBHandle(TBool& isOpen);

    /*
     * Clears the pixmap cache & model
     */
    void clearModel();
    
    void emitConversationViewEmpty();

signals:

    /*
     * Signal emitted to retrieve the pixmap icon
     */
    void retrievePreviewIcon(int msgId, QString& filepath) const;
    
    void conversationViewEmpty();

private slots:

    /*
     * Slot which handles retrievePreviewIcon signal
     */
    void updatePreviewIcon(int msgId, QString& filePath);

private:

    /**
     * Populate an item in model from CCsConversationEntry.
     * @param item, QStandardItem
     * @param entry, CCsConversationEntry
     */
    void populateItem(QStandardItem& item, const CCsConversationEntry& entry);

    /**
     * Populate MMS related data into item.
     * @param item, QStandardItem
     * @param entry, CCsConversationEntry
     */
    void handleMMS(QStandardItem& item, const CCsConversationEntry& entry);
    
    /**
     * Populate MMS Notification related data into item.
     * @param item, QStandardItem
     * @param entry, CCsConversationEntry
     */
    void handleMMSNotification(QStandardItem& item, 
        const CCsConversationEntry& entry);

    /**
     * Populate BT message related data into item.
     * @param item, QStandardItem
     * @param entry, CCsConversationEntry
     */
    void handleBlueToothMessages(QStandardItem& item, 
        const CCsConversationEntry& entry);

    /**
     * Populate Bio message related data into item.
     * @param item, QStandardItem
     * @param entry, CCsConversationEntry
     */
    void handleBioMessages(QStandardItem& item, 
        const CCsConversationEntry& entry);

    /**
     * Populates preview icon into the previewicon cache
     * @param pixmap, QPixmap
     * @param filePath, filepath to be used if icon is not in Db
     * @param msgId, key to pixmap in previewicon cache
     * @param inDb, indicates if icon was available in Db (or) not
     */    
    void setPreviewIcon(QPixmap& pixmap, QString& filePath, int msgId,
        bool inDb);

    /**
     * Get the preview icon item if available in pixmap cache / create
     * and return the preview icon
     * @param msgId, key to pixmap in previewicon cache
     * @param filePath, filepath to be used if icon is not in cache
     */
    HbIcon* getPreviewIconItem(int msgId, QString& filepath) const;
    
    /**
     * Check if a MMS is forwardable or not
     * @param messageId, message id
     * @return true/false
     */
    bool validateMsgForForward(qint32 messageId);
    
    /*
     * Handle Vcard messages
     * @param item, QStandardItem
     * @param msgId
     */
    void handleVCard(QStandardItem& item, int msgId);

    /*
     * Get the scaled size from the original image size
     * @param originalSize, QSize
     * @param scaledSize, QSize
     */
    void getScaledSize(const QSize &originalSize,
            QSize &scaledSize);

private:

    /**
     * Msg Store Handler
     * Not Own
     */
    ConversationMsgStoreHandler* mMsgStoreHandler;

    /**
     * UniDataModelLoader object
     * Own
     */
    UniDataModelLoader* iDataModelPluginLoader;

    /**
     * Mms Data model plugin interface
     * Not Own
     */
    UniDataModelPluginInterface* iMmsDataPlugin;

    /**
     * BioMsg Data model plugin interface
     * Not Own
     */
    UniDataModelPluginInterface* iBioMsgPlugin;
    
    /*
     * SQL DB handle
     */
    RSqlDatabase iSqlDb;

    /*
     * DB open.
     */
    TBool iSqlDbOpen;

    /*
     *  preview-icon cache
     */
    QCache<int, HbIcon> previewIconCache;
};

#endif // CONVERSATIONS_MODEL_H
