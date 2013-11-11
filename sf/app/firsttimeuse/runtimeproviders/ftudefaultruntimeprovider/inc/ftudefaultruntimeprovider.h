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
* Description:  Declaration of the FTU default runtime provider.
*
*/


#ifndef FTU_RUNTIMEPROVIDERS_DEFAULTRUNTIMEPROVIDER_H
#define FTU_RUNTIMEPROVIDERS_DEFAULTRUNTIMEPROVIDER_H

#include <QObject>
#include <qserviceplugininterface.h>


QTM_USE_NAMESPACE

/**
 * @ingroup group_ftudefaultruntimeprovider
 * @brief Provides a default implementation of the ftu runtime.
 *
 * This provider includes a default implementation of the ftu runtime.
 * The runtime is described in the ftudefaultruntimeprovider.manifest file.
 *
 * @since S60 ?S60_version.
 */
class FtuDefaultRuntimeProvider : public QObject, public QServicePluginInterface
{    
    Q_OBJECT
    Q_INTERFACES(QtMobility::QServicePluginInterface)
public:

public:
    QObject *createInstance(const QServiceInterfaceDescriptor &descriptor,
                            QServiceContext *context,
                            QAbstractSecuritySession *session);

};

#endif //FTU_RUNTIMEPROVIDERS_DEFAULTRUNTIMEPROVIDER_H

