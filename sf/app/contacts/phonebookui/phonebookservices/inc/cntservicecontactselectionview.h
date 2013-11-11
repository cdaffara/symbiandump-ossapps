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

#ifndef CNTSERVICECONTACTSELECTIONVIEW_H
#define CNTSERVICECONTACTSELECTIONVIEW_H

#include <QObject>

#include "cntbaseselectionview.h"
#include "cntabstractserviceprovider.h"

class QModelIndex;
class CntServiceHandler;

class CntServiceContactSelectionView : public CntBaseSelectionView
{
    Q_OBJECT

public:
    CntServiceContactSelectionView( CntAbstractServiceProvider& aServiceProvider );
    ~CntServiceContactSelectionView();

    int viewId() const { return serviceContactSelectionView; }
    
public slots:
    void onListViewActivated(const QModelIndex &aIndex);
    void aboutToOpenView(CntAbstractViewManager* aMgr, const CntViewParameters viewParameters);
    void aboutToCloseView();
        
private:
    CntAbstractServiceProvider& mProvider;
    QContactDetail     mDetail;
};

#endif /* CNTSERVICECONTACTSELECTIONVIEW_H */
