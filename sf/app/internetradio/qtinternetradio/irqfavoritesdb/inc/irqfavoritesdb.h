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
#ifndef IRQFAVORITESDB_H
#define IRQFAVORITESDB_H

#include "irqfavoritesdbexporter.h"

class IRQPreset;
class favoritesWrapper;
class channelHistoryWrapper;
class urlInfoWrapper;

class IRQFAVORITES_DLL_EXPORT IRQFavoritesDB
{
public:

    IRQFavoritesDB();

    ~IRQFavoritesDB();

    /* add a preset to favorites
     * @param IRQPreset& the isds preset,
     * @return  errcode
     */
    int addPreset(const IRQPreset& aPreset);

    /* delete a preset
     * @return errcode
     * @param
     */
    int deletePreset(const IRQPreset& aPreset);

    /* replace with new preset
     * @return errcode 
     */
    void replacePreset(const IRQPreset& aNewPreset);

    /* change the preset type to user defined
     * @return errcode
     */
    int makePresetUserDefined(int aChannelId, int aUserDefinedChannelId);

    /* for CIRPreset is just an interface so we can wrapper it into the IRQPreset.
     * the interface get the IRQPreset list. The upper layer will free all the items
     * in the list and the list self*/
    QList<IRQPreset*>* getPresets() const;
    
    int renamePreset(const IRQPreset &aPreset, const QString &aNewName);

private:
    favoritesWrapper *iFavoritesWrapper;
    channelHistoryWrapper *iHistoryWrapper;
    urlInfoWrapper *iUrlWrapper;
};

#endif
