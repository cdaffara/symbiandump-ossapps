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
 * Description:  Declaration of the Search state provider
 *
 */

#ifndef SEARCH_STATEPROVIDERS_STATEPROVIDER_H
#define SEARCH_STATEPROVIDERS_STATEPROVIDER_H

#include "search_global.h"

SEARCH_CLASS( SearchStateProviderTest)

#ifdef BUILD_SEARCHSTATEPROVIDER
#define SEARCH_STATEPROVIDER Q_DECL_EXPORT
#else
#define SEARCH_STATEPROVIDER Q_DECL_IMPORT
#endif

class QState;

/**
 * @ingroup group_Searchstateprovider
 * @brief Provides a default implementation for each Search state.
 *
 * This provider includes a default implementation for each Search state
 * States are described in the Searchstateprovider.manifest file.
 *
 * @lib Searchstateprovider
 * @since S60 ?S60_version.
 */
class SEARCH_STATEPROVIDER SearchStateProvider : public QObject
    {

Q_OBJECT

public:

    /**
     * Default constructor.
     *
     * @since S60 ?S60_version.
     */
    SearchStateProvider(QObject *parent=NULL);    
    

public:   

    /**
     * Creates a state based on the given token.
     *
     * @param aToken Identifies the state to be created.
     * @since S60 ?S60_version.
     */
    QState* createState(const QString& uri);

private:   

    SEARCH_FRIEND_CLASS    ( SearchStateProviderTest)
    };

#endif //SEARCH_STATEPROVIDERS_STATEPROVIDER_H
