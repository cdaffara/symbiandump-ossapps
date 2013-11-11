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

#ifndef MOBEXAMPLENODETAILACTION_H_
#define MOBEXAMPLENODETAILACTION_H_

#include "cntexamplebaseaction.h"

class MobExampleNoDetailAction : public MobBaseAction
{
    Q_OBJECT

public:
    MobExampleNoDetailAction(const QString& actionName, const QString& vendorName);
    ~MobExampleNoDetailAction();
    
    QVariantMap metaData() const;
    QContactFilter contactFilter(const QVariant& value) const;
    bool isDetailSupported(const QContactDetail &detail, const QContact &contact = QContact()) const;
    QList<QContactDetail> supportedDetails(const QContact& contact) const;
    State state() const;
    MobExampleNoDetailAction* clone() const;
};

#endif /* MOBEXAMPLENODETAILACTION_H_ */
