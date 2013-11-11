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

#ifndef QMOBEXAMPLEDYNAMICACTION_H
#define QMOBEXAMPLEDYNAMICACTION_H

#include "cntexamplebaseaction.h"

#include <QIcon>

class MobExampleDynamicAction : public MobBaseAction
{
    Q_OBJECT

public:
    MobExampleDynamicAction(const QString& actionName, const QString& vendorName);
    MobExampleDynamicAction(const QString& actionName, const QString& vendorName, const MobExampleDynamicAction& copy);
    ~MobExampleDynamicAction();
    
    QVariantMap metaData() const;
    QContactFilter contactFilter(const QVariant& value) const;
    bool isDetailSupported(const QContactDetail &detail, const QContact &contact = QContact()) const;
    QList<QContactDetail> supportedDetails(const QContact& contact) const;
    State state() const;
    MobExampleDynamicAction* clone() const;
    
    void setDefinitionName(const QString& definitionName);
    void setFilter(QContactFilter filter);
    void setIcon(QIcon icon);
    void setTitleField(QString titleField);
    void setValueField(QString valueField);
    void setTitleFieldDetail(QString titleField);
    void setValueFieldDetail(QString valueField);

private:
    QString mTitleField;
    QString mValueField;
    QString mTitleFieldDetail;
    QString mValueFieldDetail;
    QIcon mIcon;
    QContactFilter mFilter;
    QString mDefinitionName;
};

#endif
