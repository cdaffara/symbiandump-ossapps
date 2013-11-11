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
 * Description:  Declaration of the search default runtime provider.
 *
 */

#ifndef SEARCH_RUNTIMEPROVIDERS_DEFAULTRUNTIMEPROVIDER_H
#define SEARCH_RUNTIMEPROVIDERS_DEFAULTRUNTIMEPROVIDER_H

#include <qobject.h>

#ifdef BUILD_SEARCHRUNTIMEPROVIDER
#define SEARCH_RUNTIMEPROVIDER Q_DECL_EXPORT
#else
#define SEARCH_RUNTIMEPROVIDER Q_DECL_IMPORT
#endif


class SearchRuntime;

/**
 * @ingroup group_Searchruntimeprovider
 * @brief Provides a default implementation of the SEARCH runtime.
 *
 * This provider includes a default implementation of the SEARCH runtime.
 * The runtime is described in the Searchruntimeprovider.manifest file.
 *
 * @since S60 ?S60_version.
 */
class SEARCH_RUNTIMEPROVIDER  SearchRuntimeProvider : public QObject
    {
Q_OBJECT

public:
    SearchRuntime* createPlugin() const;    
};

#endif //SEARCH_RUNTIMEPROVIDERS_DEFAULTRUNTIMEPROVIDER_H
