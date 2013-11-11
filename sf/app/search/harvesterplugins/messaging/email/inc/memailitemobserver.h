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

#ifndef MEMAILITEMOBSERVER_H
#define MEMAILITEMOBSERVER_H

#include <e32base.h>
#include <common.h>

/**
 * @file 
 * @ingroup Email Plugin internal API
 * @brief Contains Interface to handle e-mail documents from e-mail fetcher
 */

class CSearchDocument;

/**
 * @brief The interface providing way to handle search document and update index database from action
 * @ingroup Email Plugin internal API
 * 
 * MEmailItemObserver provides callback to email plugin from email fetcher. 
 * 
 */
class MEmailItemObserver
{
public:
	    	
	/**
	 * A pure virtual method which is called by the email fetcher when it wants to send a document to
	 * e-mail plugin for action on that document add/delete or update.
	 *
	 * @param aSearchDocument search document created by email fetcher
   * @param aActionType action to be taken on document
	 */
	virtual void HandleDocumentL(const CSearchDocument* aSearchDocument, TCPixActionType aActionType) = 0;
	
	/**
     * A pure virtual method which is called by the email fetcher when harvesting completed     
     */
    virtual void HarvestingCompleted() = 0;

};

#endif // MEMAILITEMOBSERVER_H
