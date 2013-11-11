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
#ifndef MSGERRORNOTIFIERSVC_H_
#define MSGERRORNOTIFIERSVC_H_

#include <QObject>
#include <xqserviceprovider.h>
#include <qvariant.h>

class MsgErrorNotifierSvc: public XQServiceProvider
{

Q_OBJECT

public:
    /**
     * constructor
     */
    MsgErrorNotifierSvc(QObject* parent = 0);

    /**
     * destructor
     */
    virtual ~MsgErrorNotifierSvc();

public slots:

    /**
     * The service slot for displaying the error note.
     * @param displayParams QVariantList with contact name
     * and conversation id.
     */
    void displayErrorNote(QVariantList displayParams);

};

#endif /* MSGERRORNOTIFIERSVC_H_ */
