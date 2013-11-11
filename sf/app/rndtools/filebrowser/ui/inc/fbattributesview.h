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
*
*/

#ifndef FBATTRIBUTESVIEW_H
#define FBATTRIBUTESVIEW_H

#include <HbView>

class HbDataForm;
class HbDataFormModelItem;

class FbAttributesView : public HbView
{
    Q_OBJECT

public:
    FbAttributesView(quint32 &, quint32 &, bool &);
    virtual ~FbAttributesView();

    quint32 &setAttributesMask() { return mSetAttributesMask; }
    quint32 &clearAttributesMask() { return mClearAttributesMask; }
    bool &recurse() { return mRecurse; }

signals:
    void finished(bool ok);

public slots:
    void accept();
    void reject();

private:
    void initDataForm();
    void createToolbar();
    void loadSettings();
    void saveSettings();

private:
    quint32 mSetAttributesMask;
    quint32 mClearAttributesMask;
    bool mRecurse;

    HbDataForm *mForm;
    HbDataFormModelItem *mArchiveItem;
    HbDataFormModelItem *mHiddenItem;
    HbDataFormModelItem *mReadOnlyItem;
    HbDataFormModelItem *mSystemItem;
    HbDataFormModelItem *mRecurseItem;
};

#endif // FBATTRIBUTESVIEW_H
