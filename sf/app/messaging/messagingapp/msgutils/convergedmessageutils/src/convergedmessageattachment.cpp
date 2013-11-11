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
 * Description: Implementation of ConvergedMessageAttachment class
 *
 */

//SYSTEM INCLUDES
#include "convergedmessageattachment.h"

//USER INCLUDES

//----------------------------------------------------------------
// ConvergedMessageAttachment::ConvergedMessageAttachment
// @see header
//----------------------------------------------------------------
ConvergedMessageAttachment::ConvergedMessageAttachment(
                               const QString& filepath,
                               const int attachmenttype )
    {
    mPath = filepath;
    mType = attachmenttype;
    }

//----------------------------------------------------------------
// ConvergedMessageAttachment::~ConvergedMessageAttachment
// @see header
//----------------------------------------------------------------
ConvergedMessageAttachment::~ConvergedMessageAttachment()
    {
    }

//----------------------------------------------------------------
// ConvergedMessageAttachment::setFilePath
// @see header
//----------------------------------------------------------------
void ConvergedMessageAttachment::setFilePath( 
		const QString& filepath )
    {
    mPath = filepath;
    }

//----------------------------------------------------------------
// ConvergedMessageAttachment::filePath
// @see header
//----------------------------------------------------------------
const QString& ConvergedMessageAttachment::filePath() const
    {
    return mPath;
    }

//----------------------------------------------------------------
// ConvergedMessageAttachment::attachmentType
// @see header
//----------------------------------------------------------------
void ConvergedMessageAttachment::setAttachmentType( 
		const int attachmenttype )
    {
    mType = attachmenttype;
    }

//----------------------------------------------------------------
// ConvergedMessageAttachment::attachmentType
// @see header
//----------------------------------------------------------------
int ConvergedMessageAttachment::attachmentType() const
    {
    return mType;
    }

//----------------------------------------------------------------
// ConvergedMessageAttachment::serialize
// @see header
//----------------------------------------------------------------
void ConvergedMessageAttachment::serialize(
		QDataStream &stream) const
    {
    //put path and type of attachment into the stream
    stream << mPath;
    QString attType = QString::number(mType);
    stream << attType;
    }

//----------------------------------------------------------------
// ConvergedMessageAttachment::deserialize
// @see header
//----------------------------------------------------------------
void ConvergedMessageAttachment::deserialize(
		QDataStream &stream)
    {
    //get path and type of attachment from the stream
    stream >> mPath;
    QString attType;
    stream >> attType; 
    mType = attType.toInt();
    }

//----------------------------------------------------------------
// ConvergedMessageAttachment::operator==
// @see header
//----------------------------------------------------------------
bool ConvergedMessageAttachment::operator==(
		const ConvergedMessageAttachment &other) const
    {
    bool result = false;
        
    if( mPath == other.filePath() && 
        mType == other.attachmentType() )
        {
        result = true;
        }
    
    return result;
    }

//eof

