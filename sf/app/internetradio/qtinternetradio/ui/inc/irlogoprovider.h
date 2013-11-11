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

#ifndef IRLOGOPROVIDER_H_
#define IRLOGOPROVIDER_H_

#include <QObject>

class IRQIsdsClient;
class IRQPreset;

class IRLogoProvider : public QObject
{
    Q_OBJECT
public:
    explicit IRLogoProvider(IRQIsdsClient *aIsdsClient);
    ~IRLogoProvider();
    
    void getLogo(IRQPreset *aPreset);
    
    void activate(const QObject *aReceiver, const char *aFunc);
    void deactivate();
    
signals:
    void logoData(const QByteArray &aLogoData);
    
private slots:
    void presetLogoDownload(IRQPreset* aPreset);
    void presetLogoDownloadError(); 
    
private:
    IRQIsdsClient *iIsdsClient;
};

#endif /* IRLOGOPROVIDER_H_ */
