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
 * Description: This class defines Converged message's attachment
 */

#ifndef CONVERGEDMESSAGEATTACHMENT_H_
#define CONVERGEDMESSAGEATTACHMENT_H_

//SYSTEM INCLUDE
#include <QString>
#include <QList>
#include "msgutilsapidefines.h"

/**
 * ConvergedMessageAttachment
 * Definition of Converged message's attachment object
 */
class MSG_UTILS_API_EXPORT ConvergedMessageAttachment
    {
public:
    /**
     * AttachmentType
     * enumerations for converged message's attachement type
     */
    enum AttachmentType
        {
        EInline = 0,
        EAttachment,
        ESmil,
        EUnknown
        };

    /**
     * Constructor
     */
    ConvergedMessageAttachment(const QString& filepath = QString(),
                               const int attachmenttype = EUnknown);

    /**
     * Destructor
     */
    ~ConvergedMessageAttachment();
    
    /**
     * setFilePath
     * @param filepath
     */
    void setFilePath( const QString& filepath );

    /**
     * filePath
     * @return file path of the attachment
     */
     const QString& filePath() const;
     
     /**
      * setAttachmentType
      * @param attachmenttype
      */
     void setAttachmentType( const int attachmenttype );

    /**
     * type
     * @return attachment type
     */
     int attachmentType() const;

	/**
	* Serialize the data memebers into the stream.
	* @param stream data stream to which data is serialized.
	*/
	void serialize(QDataStream &stream) const;

	/**
	* Deserialize the stream to data members.
	* @param stream data stream from which data is deserialized.
	*/
	void deserialize(QDataStream &stream);

	/**
	* Overloaded == operator
	* Compares two message attachments
	* @param other another ConvergedMessageAttachment
	* @return bool true or false if the attachments are same
	*/
	bool operator==(const ConvergedMessageAttachment &other) const;

private:
    /**
     * mPath Path of the attachment
     */
    QString mPath;

    /**
     * mType Attachment type
     */
    int mType;
    };

typedef QList<ConvergedMessageAttachment*> ConvergedMessageAttachmentList;

#endif /* CONVERGEDMESSAGEATTACHMENT_H_ */
