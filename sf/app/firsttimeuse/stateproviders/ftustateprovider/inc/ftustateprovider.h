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
* Description:  Declaration of the FTU state provider
*
*/


#ifndef FTU_STATEPROVIDERS_STATEPROVIDER_H
#define FTU_STATEPROVIDERS_STATEPROVIDER_H

#include "ftutest_global.h"
#include "ftustateprovider_global.h"

FTU_TEST_CLASS(FtuStateProviderTest)

class QState;
class FtuStateProviderPrivate;


    
/**
 * @ingroup group_ftustateprovider
 * @brief Provides a default implementation for each FTU state.
 *
 * This provider includes a default implementation for each FTU state
 * States are described in the ftustateprovider.manifest file.
 *
 * @lib ftustateprovider
 * @since S60 ?S60_version.
 */
class FTUSTATEPROVIDER_EXPORT FtuStateProvider : public QObject
{

    Q_OBJECT
public:

    FtuStateProvider(QObject *parent=NULL);

public:

    /**
     * Creates a state based on the given uri.
     *
     * @param uri Identifies the state to be created.
     * @since S60 ?S60_version.
     * @return Constructed QState, ownership transferred.
     */
    QState* createState(const QString& uri);

private:

    FtuStateProviderPrivate* mPrivate;
    friend class FtuStateProviderPrivate; 
    
        

    FTU_TEST_FRIEND_CLASS(FtuStateProviderTest)
};


#endif //FTU_STATEPROVIDERS_STATEPROVIDER_H
