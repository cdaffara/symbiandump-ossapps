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
* Description: Class for handling the attachment list
*
*/

#ifndef NMATTACHMENTLIST_H
#define NMATTACHMENTLIST_H

#include <QObject>
#include <QStringList>

class NmAttachmentListWidget;
class NmId;

class NmAttachmentList : public QObject
{
    Q_OBJECT
public:
    NmAttachmentList(NmAttachmentListWidget &listWidget);
    ~NmAttachmentList();
    int insertAttachment(const QString &fullFileName,
                         const QString &fileSize,
                         const NmId &attachmentPartId);
    void setAttachmentPartId(const QString fullFileName, const NmId &attachmentPartId);
    void setAttachmentSize(const NmId &attachmentPartId, const QString &size);
    void removeAttachment(int arrayIndex);
    void removeAttachment(const QString &fullFileName);
    void removeAttachment(const NmId &attachmentPartId);
    NmAttachmentListWidget &listWidget();
    void clearList();
    int count();
    NmId nmIdByIndex(int listIndex);
    int indexByNmId(const NmId &id);
    QString getFullFileNameByIndex(int arrayIndex);

private:
    QString fullNameToDisplayName(const QString &fullName);
    
signals:
    void attachmentListLayoutChanged();
    
private:
    NmAttachmentListWidget &mListWidget; // Not owned
    QStringList mFullFileName;
    QStringList mDisplayFileName;
    QStringList mFileSize;
    QList<NmId> mAttachmentPartId;
};

#endif // NMATTACHMENTLIST_H
