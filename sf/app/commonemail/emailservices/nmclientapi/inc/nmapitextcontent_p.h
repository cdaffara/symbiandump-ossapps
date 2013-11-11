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
*       Email message text content private header.
*/

#ifndef NMAPITEXTCONTENT_P_H_
#define NMAPITEXTCONTENT_P_H_

namespace EmailClientApi
{

class NmApiTextContentPrivate : public QSharedData
{

public:
    NmApiTextContentPrivate();
    NmApiTextContentPrivate(const NmApiTextContentPrivate &textcontentprivate);
    virtual ~NmApiTextContentPrivate();

    QString content() const;
    void setContent(const QString &content);
    
private:
    
    QString mContent;
};
}

#endif /* NMAPITEXTCONTENT_P_H_ */
