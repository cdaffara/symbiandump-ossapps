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

#include "nmuiheaders.h"

/*!
	\class NmAttachmentList
	\brief Class for handling the attachment list
*/

/*!
    Constructor
*/
NmAttachmentList::NmAttachmentList(NmAttachmentListWidget &listWidget)
: mListWidget(listWidget)
{
    NM_FUNCTION;
}

/*!
    Destructor
*/
NmAttachmentList::~NmAttachmentList()
{ 
    NM_FUNCTION;
}

/*!
    Insert new list item. Returns the index of the attachment in list
*/
int NmAttachmentList::insertAttachment(
    const QString &fullFileName,
    const QString &fileSize,
    const NmId &attachmentPartId)
{
    NM_FUNCTION;
    
    QString displayName = fullNameToDisplayName(fullFileName);
    mFullFileName.append(fullFileName);
    mDisplayFileName.append(displayName);
    mAttachmentPartId.append(attachmentPartId);
    mFileSize.append(fileSize);
    mListWidget.insertAttachment(
        count() - 1,
        displayName, 
        NmUtilities::attachmentSizeString(fileSize.toDouble()));  
    return count() - 1;
}

/*!
    Set attachmentPartId of the list item. Because there can be several attachments with
    same filename, function will search the one which nmid is not set.
*/
void NmAttachmentList::setAttachmentPartId(const QString fullFileName, 
                                           const NmId &attachmentPartId)
{
    NM_FUNCTION;
    
    for (int i=0; i<count(); ++i) {
        if (mFullFileName.at(i) == fullFileName && mAttachmentPartId.at(i).id() == 0) {
            mAttachmentPartId.replace(i, attachmentPartId);
        }
    }
}

/*!
    Set fileSize of the attachment.
*/
void NmAttachmentList::setAttachmentSize(const NmId &attachmentPartId, const QString &size)
{
    NM_FUNCTION;
    
    for (int i=0; i<count(); ++i) {
        if (mAttachmentPartId.at(i) == attachmentPartId) {
            mFileSize.replace(i, size);
            mListWidget.setAttachmentSize(i, NmUtilities::attachmentSizeString(size.toDouble()));
        }
    }
}

/*!
    Removes attachment from list position
*/
void NmAttachmentList::removeAttachment(int arrayIndex)
{
    NM_FUNCTION;
    
    if (arrayIndex < count() && 
        arrayIndex >= 0 ) {
        // Remove UI
        mListWidget.removeAttachment(arrayIndex);
        // Remove from data structure
        mFullFileName.removeAt(arrayIndex);
        mDisplayFileName.removeAt(arrayIndex);
        mAttachmentPartId.removeAt(arrayIndex);
    }
}

/*!
    Return full filename of the list item
*/
QString NmAttachmentList::getFullFileNameByIndex(int arrayIndex)
{
    NM_FUNCTION;
    
	QString result;
	
    if ( arrayIndex >= 0 && arrayIndex < mFullFileName.count() ) {
        result.append(mFullFileName.at(arrayIndex));
    }
    return result;
}

/*!
    Remove attachment which have same fullFileName from list
*/
void NmAttachmentList::removeAttachment(const QString &fullFileName)
{
    NM_FUNCTION;
    
    for (int i=0; i<count(); ++i) {
        if (mFullFileName.at(i) == fullFileName) {
            removeAttachment(i);
        }
    }
}

/*!
    Remove attachment which have same attachmentPartId from list
*/
void NmAttachmentList::removeAttachment(const NmId &attachmentPartId)
{
    NM_FUNCTION;
    
    for (int i=0; i<count(); ++i) {
        if (mAttachmentPartId.at(i) == attachmentPartId) {
            removeAttachment(i);
        }
    }
}

/*!
    Clear attachment list from UI and from data structure
*/
void NmAttachmentList::clearList()
{
    NM_FUNCTION;
    
    for (int i=count()-1; i>=0; --i) {
        // Remove from UI
        mListWidget.removeAttachment(i);
        // Remove from data structure
        mFullFileName.removeAt(i);
        mDisplayFileName.removeAt(i);
        mAttachmentPartId.removeAt(i);
    }
}

/*!
    Return attachment list widget
*/
NmAttachmentListWidget& NmAttachmentList::listWidget()
{
    NM_FUNCTION;
    
    return mListWidget;
}

/*!
    Return attachment count
*/
int NmAttachmentList::count()
{
    NM_FUNCTION;
    
    return mFullFileName.count();
}

/*!
    Return attachment part id
*/
NmId NmAttachmentList::nmIdByIndex(int listIndex)
{
    NM_FUNCTION;
    
    return mAttachmentPartId.at(listIndex);
}

/*!
    Return array index of attachment
*/
int NmAttachmentList::indexByNmId(const NmId &id)
{
    NM_FUNCTION;
    
    for (int i=0; i<count(); ++i) {
        if (mAttachmentPartId.at(i) == id) {
            return i;
        }
    }
    return -1;
}

/*!
    Remove file path from full filename
*/
QString NmAttachmentList::fullNameToDisplayName(const QString &fullName)
{
    NM_FUNCTION;
    
    return fullName.section('\\', -1);
}
