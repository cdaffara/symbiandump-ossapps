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
 *     Email message text content public header.
 */

#ifndef NMAPITEXTCONTENT_H_
#define NMAPITEXTCONTENT_H_

#include <QString>
#include <QSharedDataPointer>
#include <nmapimessagecontent.h>
#include <nmapidef.h>

namespace EmailClientApi
{
class NmApiTextContentPrivate;

class NMAPI_EXPORT NmApiTextContent : public NmApiMessageContent
{
public:
    NmApiTextContent();
    NmApiTextContent(const NmApiTextContent &textcontent);    
    virtual ~NmApiTextContent();

    NmApiTextContent& operator=(const NmApiTextContent &textcontent);    

    QString content() const;
    void setContent(const QString &content);

private:
    
    QSharedDataPointer<NmApiTextContentPrivate> d;
};
}

#endif /* NMAPITEXTCONTENT_H_ */
