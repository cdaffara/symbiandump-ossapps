/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef CXUISETTINGRADIOBUTTONLIST_H_
#define CXUISETTINGRADIOBUTTONLIST_H_

#include <QVariantList>
#include <hbradiobuttonlist.h>

class CxeEngine;
namespace CxUiSettings {
    class RadioButtonListParams;
}
class CxuiSettingRadioButtonListModel;


class CxuiSettingRadioButtonList : public HbRadioButtonList
{
    Q_OBJECT

public:

    enum ListBoxType
    {
        SingleLine = 1,
        TwoLine = 2
    };

public:
    explicit CxuiSettingRadioButtonList(QGraphicsItem *parent, CxeEngine *engine);
    void init(CxUiSettings::RadioButtonListParams *data);

    void setOriginalSelectedItemByValue(const QVariant &value);
signals:
    void valueSelected(int value);
    void selectionCommitted();

public slots:
    void handleSelectionAccepted();
    void handleClose();

protected slots:
    void handleItemSelected(int index);

private:
    // helper methods
    void setSettingId(const QString &id);
    void setItems(const QStringList &list);
    void setListBoxType(int type);
    void commit(int index);

protected:
    CxeEngine *mEngine;
    CxuiSettingRadioButtonListModel *mListModel;
    QString mSettingId;
    QVariantList mSettingValues;
    int mOriginalIndex;
    bool mPreview;

private:
    Q_DISABLE_COPY(CxuiSettingRadioButtonList)

};

#endif /* CXUISETTINGRADIOBUTTONLIST_H_ */
