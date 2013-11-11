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
 * Description:  Implementation indevice handler
 *
 */
#include "indevicehandler.h"
#include <cpixsearcher.h>
#include <cpixdocument.h>

// ---------------------------------------------------------------------------
// InDeviceHandler::InDeviceHandler()
// ---------------------------------------------------------------------------
//
InDeviceHandler::InDeviceHandler() :
    mSearchInterface(0), mSearchResultCount(0)
    {
    }
// ---------------------------------------------------------------------------
// InDeviceHandler::~InDeviceHandler()
// ---------------------------------------------------------------------------
//
InDeviceHandler::~InDeviceHandler()
    {
    if (mSearchInterface)
        {
        disconnect(mSearchInterface, SIGNAL(handleSearchResults(int,int)),
                this, SLOT(getSearchResult(int,int)));
        disconnect(mSearchInterface,
                SIGNAL(handleDocument(int,CpixDocument*)), this,
                SLOT(getDocumentAsync(int,CpixDocument*)));
        disconnect(mSearchInterface,
                SIGNAL(handleDocument(int,CpixDocument*)), this,
                SLOT(getDocumentAsync(int,CpixDocument*)));
        delete mSearchInterface;
        }
    }
// ---------------------------------------------------------------------------
// InDeviceHandler::getSearchResult(int aError, int estimatedResultCount)
// aError: error code
// estimatedResultCount: number of hits
// ---------------------------------------------------------------------------
//
void InDeviceHandler::getSearchResult(int aError, int estimatedResultCount)
    {
    mSearchResultCount = estimatedResultCount;
    emit handleAsyncSearchResult(aError, estimatedResultCount);
    }
// ---------------------------------------------------------------------------
// InDeviceHandler::getDocumentAsync(int aError, CpixDocument* aDocument)
// aError: error code
// aDocument: holding the result item
// ---------------------------------------------------------------------------
//
void InDeviceHandler::getDocumentAsync(int aError, CpixDocument* aDocument)
    {
    emit handleDocument(aError, aDocument);
    }
// ---------------------------------------------------------------------------
// InDeviceHandler::getBatchDocumentAsync(int aError, CpixDocument* aDocument)
// aError: error code
// aDocument: holding the result item
// ---------------------------------------------------------------------------
//
void InDeviceHandler::getBatchDocumentAsync(int aError, int aCount,
        CpixDocument** aDocument)
    {
    emit handleBatchDocument(aError, aCount, aDocument);
    }
// ---------------------------------------------------------------------------
// InDeviceHandler::getDocumentAtIndex(int aIndex)
// aIndex: item index to be found
// ---------------------------------------------------------------------------
//
CpixDocument* InDeviceHandler::getDocumentAtIndex(int aIndex)
    {
    CpixDocument* doc = NULL;
    if (mSearchInterface)
        {
        try
            {
            doc = mSearchInterface->document(aIndex);

            }
        catch (...)
            {
            delete doc;
            return NULL;
            }
        }
    return doc;
    }

// ---------------------------------------------------------------------------
// InDeviceHandler::getSearchResultCount()
// gets the number of hits
// ---------------------------------------------------------------------------
//
int InDeviceHandler::getSearchResultCount()
    {
    return mSearchResultCount;
    }
// ---------------------------------------------------------------------------
// InDeviceHandler::getDocumentAsyncAtIndex(int aIndex)
// aIndex :  index of item to be found async
// ---------------------------------------------------------------------------
//
void InDeviceHandler::getDocumentAsyncAtIndex(int aIndex)
    {
    if (mSearchInterface)
        {
        try
            {
            mSearchInterface->documentAsync(aIndex);
            }
        catch (...)
            {
            // handle the exception
            return;
            }
        }
    }
// ---------------------------------------------------------------------------
// InDeviceHandler::getBatchDocumentAsyncAtIndex
// aIndex :  index of item to be found async
// aCount :  number of result items
// ---------------------------------------------------------------------------
//
void InDeviceHandler::getBatchDocumentAsyncAtIndex(int aIndex, int aCount)
    {
    if (mSearchInterface)
        {
        try
            {
            mSearchInterface->batchdocumentAsync(aIndex, aCount);
            }
        catch (...)
            {
            // handle the exception
            return;
            }
        }
    }
// ---------------------------------------------------------------------------
// InDeviceHandler::searchAsync
// aSearchAsyncString: string to be searched
// async
// ---------------------------------------------------------------------------
//
void InDeviceHandler::searchAsync(QString aSearchAsyncString, QString/* aDefaultSearchField*/)
    {
    if (aSearchAsyncString.length())
        {
        mSearchResultCount = 0;
        if (mSearchInterface)
            {
            try
                {
                mSearchInterface->searchAsync(aSearchAsyncString);
                }
            catch (...)
                {
                // handle the exception
                return;
                }
            }
        }
    }
// ---------------------------------------------------------------------------
// InDeviceHandler::cancelLastSearch()
// ---------------------------------------------------------------------------
//
void InDeviceHandler::cancelLastSearch()
    {
    if (mSearchInterface)
        {
        try
            {
            mSearchInterface->cancelSearch();
            }
        catch (...)
            {
            // handle the exception
            return;
            }
        }
    }
// ---------------------------------------------------------------------------
// InDeviceHandler::setCategory
// astring: setting categories to be searched
// ---------------------------------------------------------------------------
//
void InDeviceHandler::setCategory(QString astring)
    {
    if (mSearchInterface)
        {
        delete mSearchInterface;
        mSearchInterface = NULL;
        }
    if (astring.length())
        {       
        mSearchInterface = CpixSearcher::newInstance(astring,
                DEFAULT_SEARCH_FIELD);
        }
    if (mSearchInterface)
        {
        connect(mSearchInterface, SIGNAL(handleSearchResults(int,int)), this,
                SLOT(getSearchResult(int,int)));
        connect(mSearchInterface, SIGNAL(handleDocument(int,CpixDocument*)),
                this, SLOT(getDocumentAsync(int,CpixDocument*)));
        connect(mSearchInterface,
                SIGNAL(handleBatchDocuments(int,int,CpixDocument**)), this,
                SLOT(getBatchDocumentAsync(int,int,CpixDocument**)));
        }
    }
// ---------------------------------------------------------------------------
// InDeviceHandler::isPrepared()
// verify the mSearchInterface is prepared or not
// ---------------------------------------------------------------------------
//
bool InDeviceHandler::isPrepared()
    {
    if (mSearchInterface)
        {
        return true;
        }
    return false;
    }
