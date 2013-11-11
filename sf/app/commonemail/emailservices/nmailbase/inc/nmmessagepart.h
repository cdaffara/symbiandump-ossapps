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

#ifndef NMMESSAGEPART_H_
#define NMMESSAGEPART_H_

#include <QExplicitlySharedDataPointer>
#include "nmbasedef.h"
#include "nmcommon.h"
#include "nmmessageenvelope.h"

class NMBASE_EXPORT NmMessagePart;

class NMBASE_EXPORT NmMessagePartPrivate : public QSharedData
{
public:
	NmMessagePartPrivate();
	virtual ~NmMessagePartPrivate();

	bool isMessage() const;
	bool isTextContent() const;

	NmId mPartId;

	quint32 mSize;
	quint32 mFetchedSize;

	QString mTextContent;
	QString mContentType;
	QString mContentDescription;
	QString mContentDisposition;
	QString mContentId;
	QString mAttachmentName;

	QList<NmMessagePart*> mChildParts;
	QByteArray mBinaryContent;
};

class NMBASE_EXPORT NmMessagePart
{
public:

	NmMessagePart();
	NmMessagePart(const NmId &partId);
	
	
	virtual ~NmMessagePart();

	virtual NmId partId() const;
	virtual void setPartId(const NmId &id);
	

	quint32 size() const;
	void setSize(quint32 size);

	quint32 fetchedSize() const;
	void setFetchedSize(quint32 fetchedSize);

	bool isMessage() const;

	void setTextContent(const QString &content, const QString &contentType);
	const QString &textContent() const;

	void setContentType(const QString &contentType);
	QString contentType() const;

	void setContentDescription(const QString &contentDescription);
	QString contentDescription() const;

	void setContentDisposition(const QString &contentDisposition);
	QString contentDisposition() const;

	void setContentId(const QString &contentId);
	QString contentId();

	void setChildParts(QList<NmMessagePart*> parts);
	QList<NmMessagePart*> &childParts() const;

	void addChildPart(NmMessagePart *part);
	void removeChildPart(const NmId &partId);
	void removeAllChildParts();

	void setBinaryContent(const QByteArray &content,
	                      const QString &contentType);
	const QByteArray &binaryContent() const;
	
	void setAttachmentName(const QString &filePath);
	QString attachmentName() const;
	
	void appendAttachments(QList<NmMessagePart*> &attachments) const;

protected:

	const NmMessagePart *findContentPart(const QString &contentType) const;
	NmMessagePart *findContentPart(const QString &contentType);
	// this cannot be private, must be usable from derived class
    NmMessagePart(const NmMessagePart &part);

    // for CFSMailMessagePart and NmMessage
    NmMessagePart(QExplicitlySharedDataPointer<NmMessagePartPrivate> nmPrivateMessagePart);
    
private:
	// prohibited
	NmMessagePart &operator=(const NmMessagePart &part);

	// Data object shared between Symbian and QT data types.
	QExplicitlySharedDataPointer<NmMessagePartPrivate> d;

	friend class CFSMailMessagePart;
};

#endif /* NMMESSAGEPART_H_ */



