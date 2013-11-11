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
* Description:  First Time Use runtime content service.
*
*/


#include "ftucontentservice.h"
#include "ftucontentservice_p.h"
#include <ftuwizard.h>


// ---------------------------------------------------------------------------
// FtuContentServicePrivate::FtuContentServicePrivate()
// ---------------------------------------------------------------------------
//
FtuContentServicePrivate::FtuContentServicePrivate(FtuContentService *ftuContentService)
    : QObject(ftuContentService),
      mFtuContentService(ftuContentService),
      mActiveWizardIndex(-1)
{

}

// ---------------------------------------------------------------------------
// FtuContentServicePrivate::~FtuContentServicePrivate()
// ---------------------------------------------------------------------------
//
FtuContentServicePrivate::~FtuContentServicePrivate()
{
    qDeleteAll(mWizardList);
}

// ---------------------------------------------------------------------------
// FtuContentServicePrivate::appendWizard()
// ---------------------------------------------------------------------------
//
void FtuContentServicePrivate::appendWizard(FtuWizard *wizard)
{
    if (wizard && mWizardList.contains(wizard)) {
        return;
    }

    mWizardList.append(wizard);

    if (wizard) {
        int index = mWizardList.size() - 1;
        emit mFtuContentService->wizardAdded(index);
    }
}

// ---------------------------------------------------------------------------
// FtuContentServicePrivate::insertWizard()
// ---------------------------------------------------------------------------
//
void FtuContentServicePrivate::insertWizard(int index, FtuWizard *wizard)
{
    if (wizard && mWizardList.contains(wizard)) {
        return;
    }

    if (index < 0 || mWizardList.count() < index) {
        return;
    }

    mWizardList.insert(index, wizard);

    if (wizard) {
        emit mFtuContentService->wizardAdded(index);
    }
}

// ---------------------------------------------------------------------------
// FtuContentServicePrivate::setActiveWizard()
// ---------------------------------------------------------------------------
//
void FtuContentServicePrivate::setActiveWizard(int index)
{
    if (index < 0 || mWizardList.count() <= index) {
        index = -1;
    }

    mActiveWizardIndex = index;
}

// ---------------------------------------------------------------------------
// FtuContentServicePrivate::activeWizard()
// ---------------------------------------------------------------------------
//
int FtuContentServicePrivate::activeWizard() const
{
    return mActiveWizardIndex;
}

// ---------------------------------------------------------------------------
// FtuContentServicePrivate::wizard()
// ---------------------------------------------------------------------------
//
FtuWizard* FtuContentServicePrivate::wizard(int index) const
{
    if (index < 0 || mWizardList.count() <= index) {
        return 0;
    }

    return mWizardList[index];
}

// ---------------------------------------------------------------------------
// FtuContentServicePrivate::wizards()
// ---------------------------------------------------------------------------
//
QList<FtuWizard*> FtuContentServicePrivate::wizards() const
{
    return mWizardList;
}

// ---------------------------------------------------------------------------
// FtuContentService::FtuContentService()
// ---------------------------------------------------------------------------
//
FtuContentService::FtuContentService(QObject *parent)
    : QObject(parent),
	  mFtuContentServicePrivate(NULL)
{
    mFtuContentServicePrivate = new FtuContentServicePrivate(this);    
}

// ---------------------------------------------------------------------------
// FtuContentService::~FtuContentService()
// ---------------------------------------------------------------------------
//
FtuContentService::~FtuContentService()
{
    if(mFtuContentServicePrivate)
	{
		delete mFtuContentServicePrivate;
	}
}

// ---------------------------------------------------------------------------
// FtuContentService::appendWizard()
// ---------------------------------------------------------------------------
//
void FtuContentService::appendWizard(FtuWizard *wizard)
{
    mFtuContentServicePrivate->appendWizard(wizard);
}

// ---------------------------------------------------------------------------
// FtuContentService::insertWizard()
// ---------------------------------------------------------------------------
//
void FtuContentService::insertWizard(int index, FtuWizard *wizard)
{
    mFtuContentServicePrivate->insertWizard(index, wizard);
}

// ---------------------------------------------------------------------------
// FtuContentService::setActiveWizard()
// ---------------------------------------------------------------------------
//
void FtuContentService::setActiveWizard(int index)
{
    mFtuContentServicePrivate->setActiveWizard(index);
}

// ---------------------------------------------------------------------------
// FtuContentService::activeWizard()
// ---------------------------------------------------------------------------
//
int FtuContentService::activeWizard() const
{
    return mFtuContentServicePrivate->activeWizard();
}

// ---------------------------------------------------------------------------
// FtuContentService::wizard()
// ---------------------------------------------------------------------------
//
FtuWizard *FtuContentService::wizard(int index) const
{
    return mFtuContentServicePrivate->wizard(index);
}

// ---------------------------------------------------------------------------
// FtuContentService::wizards()
// ---------------------------------------------------------------------------
//
QList<FtuWizard*> FtuContentService::wizards() const
{
    return mFtuContentServicePrivate->wizards();
}
