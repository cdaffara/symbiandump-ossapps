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

#ifndef IRFILEVIEWSERVICE_H_
#define IRFILEVIEWSERVICE_H_

#include <xqserviceprovider.h>

class XQSharableFile;
class IRPlayList;
class IRApplication;

class IRFileViewService : public XQServiceProvider
{
    Q_OBJECT
    
public:
    IRFileViewService(IRApplication *aApplication);
    ~IRFileViewService();
    IRPlayList* getPlayList() const;
    
public slots:
    void view(const QString &aFileName);
    void view(const XQSharableFile &aSharableFile);
    
private:
    IRApplication *iApplication;
    IRPlayList *iPlayList;
    
    Q_DISABLE_COPY(IRFileViewService)
};

#endif /* IRFILEVIEWSERVICE_H_ */
