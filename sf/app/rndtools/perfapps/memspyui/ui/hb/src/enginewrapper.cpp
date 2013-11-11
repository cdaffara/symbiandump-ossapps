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

#include <QMessageBox>

#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>

#include "enginewrapper.h"

// CONSTANTS

const QString OUTPUT_MODE = "output/mode";
const QString OUTPUT_PATH = "output/path";
const QString SWMT_PERIOD = "swmt/period";
const QString SWMT_MODE = "swmt/mode";
const QString SWMT_CATEGORIES = "swmt/categories";
const QString HEAP_DUMPS_MODE = "heapdumps/mode";

QString MemSpyProcess::exitInfo() const
{
    QChar exitType[] = {'K', 'T', 'P', 'R'};
    QString result = QString("[%1]").arg(exitType[mProcess->ExitType()]);
    
    if (mProcess->ExitType() != EExitKill && mProcess->ExitType() != EExitPending) {
        
        QString exitCategory = QString((QChar*) mProcess->ExitCategory().Ptr(), mProcess->ExitCategory().Length());
        result.append(QString(" %1-%2").arg(exitCategory).arg(mProcess->ExitReason()));
    }
    
    return result;
}


MemSpyDwoProgressTracker::MemSpyDwoProgressTracker(RMemSpySession &session) :
	CActive(EPriorityStandard), mSession(session)
{
	CActiveScheduler::Add(this);
}

MemSpyDwoProgressTracker::~MemSpyDwoProgressTracker()
{
	Cancel();
}

void MemSpyDwoProgressTracker::start()
{
	mSession.NotifyDeviceWideOperationProgress(mProgress, iStatus);
	
	SetActive();
}

void MemSpyDwoProgressTracker::cancel()
{
	Cancel();
}

// Event handler method.
 
void MemSpyDwoProgressTracker::RunL()
    {
 
    // If an error occurred handle it in RunError().
    User::LeaveIfError(iStatus.Int());
 
    // Resubmit the request immediately
    
    mSession.NotifyDeviceWideOperationProgress(mProgress, iStatus);
 
    SetActive();
 
    emit progress(mProgress.Progress(), QString((QChar*) mProgress.Description().Ptr(), mProgress.Description().Length()));
    }
 
void MemSpyDwoProgressTracker::DoCancel()
{ 
	// this is not yet implemented, as it is not required in current use cases
}
 
TInt MemSpyDwoProgressTracker::RunError(TInt aError)
{ 
	// KErrNotReady and KErrCancel errors are OK, they just notify 
	// us about the outstanding notification request that won't be 
	// processed.
	Q_UNUSED(aError);
	
    return KErrNone;
}

MemSpyDwoTracker::MemSpyDwoTracker(RMemSpySession &session, DeviceWideOperation operation) :
	CActive(EPriorityStandard), 
	mSession(session),
	mProgressTracker(new MemSpyDwoProgressTracker(session)),
	mOperation(operation)
{
	CActiveScheduler::Add(this);
	connect(mProgressTracker, SIGNAL(progress(int,QString)), this, SIGNAL(progress(int,QString)));
}

MemSpyDwoTracker::~MemSpyDwoTracker()
{
	Cancel();
	
	delete mProgressTracker;
}

void MemSpyDwoTracker::start()
{
	void (RMemSpySession::*functions[])(TRequestStatus&) = { 
			&RMemSpySession::OutputPhoneInfo,
			&RMemSpySession::OutputDetailedPhoneInfo,
			&RMemSpySession::OutputHeapInfo,
			&RMemSpySession::OutputCompactHeapInfo,
			&RMemSpySession::OutputHeapCellListing,
			&RMemSpySession::OutputHeapData,
			&RMemSpySession::OutputStackInfo,
			&RMemSpySession::OutputCompactStackInfo,
			&RMemSpySession::OutputUserStackData,
			&RMemSpySession::OutputKernelStackData };
	
	(mSession.*functions[mOperation])(iStatus);
	
	mProgressTracker->start();
	
	SetActive();
}

void MemSpyDwoTracker::cancel()
{
	Cancel();
}

// Event handler method.
 
void MemSpyDwoTracker::RunL()
    {
 
    // If an error occurred handle it in RunError().
    User::LeaveIfError(iStatus.Int());
 
    // Operation has finished successfully 
    emit finished(0);
    }
 
void MemSpyDwoTracker::DoCancel()
{
	// Cancel progress tracker
	mProgressTracker->cancel();
	
	mSession.CancelDeviceWideOperationL();
}
 
TInt MemSpyDwoTracker::RunError(TInt aError)
{ 
	// Emit the finished signal to notify user 
	// operation was canceled
	emit finished(aError);
	
    return KErrNone;
}

MemSpyAsyncTracker::MemSpyAsyncTracker(RMemSpySession& session, void (RMemSpySession::*function)(TRequestStatus&)) :
	CActive(EPriorityStandard), 
	mFunction(function),
	mSession(session)
{
	CActiveScheduler::Add(this);
}

void MemSpyAsyncTracker::RunL()
{ 
    // If an error occurred handle it in RunError().
    User::LeaveIfError(iStatus.Int());
 
    // Operation has finished successfully 
    emit finished(0);
}

void MemSpyAsyncTracker::DoCancel()
{
	// nothing to do here
}

TInt MemSpyAsyncTracker::RunError(TInt aError)
{ 
	// Emit the finished signal to notify user 
	// that operation was canceled
	emit finished(aError);
	
    return KErrNone;
}

void MemSpyAsyncTracker::start()
{
	(mSession.*mFunction)(iStatus);
	
	SetActive();
}

MemSpySettings::MemSpySettings() : 
	QSettings("Nokia", "MemSpy") 
{
}

OutputMode MemSpySettings::outputMode() const
{
	return static_cast<OutputMode>(value(OUTPUT_MODE, 0).toInt());
}

void MemSpySettings::setOutputMode(OutputMode mode)
{
	setValue(OUTPUT_MODE, mode);
}
	
QString MemSpySettings::outputPath() const
{
	return value(OUTPUT_PATH).toString();
}

void MemSpySettings::setOutputPath(const QString& path)
{
	setValue(OUTPUT_PATH, path);
}

int MemSpySettings::swmtTimerPeriod() const
{
	return value(SWMT_PERIOD, 30).toInt();
}

void MemSpySettings::setSwmtMode(SwmtMode mode)
{
	setValue(SWMT_MODE, mode);
}

SwmtMode MemSpySettings::swmtMode() const
{
	return static_cast<SwmtMode>(value(SWMT_MODE, 0).toInt());
}

void MemSpySettings::setSwmtTimerPeriod(int period)
{
	setValue(SWMT_PERIOD, period);
}


QVariantList MemSpySettings::swmtCategories() const
{
	return value(SWMT_CATEGORIES).toList();
}

void MemSpySettings::setSwmtCategories(const QVariantList& categories)
{
	setValue(SWMT_CATEGORIES, categories);
}

HeapDumpsMode MemSpySettings::heapDumpsMode() const
{
	return static_cast<HeapDumpsMode>(value(HEAP_DUMPS_MODE).toInt());
}

void MemSpySettings::setHeapDumpsMode(HeapDumpsMode mode)
{
	setValue(HEAP_DUMPS_MODE, mode);
}


EngineWrapper::EngineWrapper() :
	mSwmtRunning(false)
{
}

EngineWrapper::~EngineWrapper()
{
	mSession.Close();
}

bool EngineWrapper::initialize()
{
	return mSession.Connect() == KErrNone;
}

MemSpySettings& EngineWrapper::settings()
{
	return mSettings;
}

const MemSpySettings& EngineWrapper::settings() const
{
	return mSettings;
}



QList<MemSpyProcess*> EngineWrapper::getProcesses()
{
	QList<MemSpyProcess*> result;
	
	RArray<CMemSpyApiProcess*> proc;
	
	QT_TRAP_THROWING(mSession.GetProcessesL(proc));
	
	for (TInt i=0; i<proc.Count(); i++)
        result.append(new MemSpyProcess(proc[i]));
	
	return result;
}

QList<MemSpyThread*> EngineWrapper::getThreads(ProcessId processId)
{
	QList<MemSpyThread*> result;
	
	RArray<CMemSpyApiThread*> proc;
	
	QT_TRAP_THROWING(mSession.GetThreadsL(processId, proc));
	
    for (TInt i=0; i<proc.Count(); i++)
        result.append(new MemSpyThread(proc[i]));
	
	return result;
}

QList<MemSpyThreadInfoItem*> EngineWrapper::getThreadInfo(ThreadId threadId, ThreadInfoType type)
{
	QList<MemSpyThreadInfoItem*> result;
	RArray<CMemSpyApiThreadInfoItem*> threadInfo;
	qt_symbian_throwIfError(mSession.GetThreadInfoItems(threadInfo, threadId, 
			static_cast<TMemSpyThreadInfoItemType>(type)));
	
	for (TInt i=0; i<threadInfo.Count(); i++)
		result.append(new MemSpyThreadInfoItem(threadInfo[i]));
	
	return result;
}

void EngineWrapper::setThreadPriority(ThreadId threadId, ThreadPriority priority)
{
	TRAP_IGNORE(mSession.SetThreadPriorityL(threadId, priority));
}

QList<MemSpyKernelObjectType*> EngineWrapper::getKernelObjectTypes()
{
	QList<MemSpyKernelObjectType*> result;
		
	RArray<CMemSpyApiKernelObject*> types;
	qt_symbian_throwIfError(mSession.GetKernelObjects(types));
	
	for(TInt i=0; i<types.Count(); i++)
		result.append(new MemSpyKernelObjectType(types[i]));
	
	return result;
}

QList<MemSpyKernelObject*> EngineWrapper::getKernelObjects(int type)
{
	QList<MemSpyKernelObject*> result;
		
	RArray<CMemSpyApiKernelObjectItem*> objects;
	qt_symbian_throwIfError(mSession.GetKernelObjectItems(objects, 
			static_cast<TMemSpyDriverContainerType>(type)));
	
	for (TInt i=0; i<objects.Count(); i++)
		result.append(new MemSpyKernelObject(objects[i]));
	
	return result;
}

QList<MemSpyServer*> EngineWrapper::getServers()
{
    QList<MemSpyServer*> result;
        
    RArray<CMemSpyApiServer*> proc;
    
    QT_TRAP_THROWING(mSession.GetServersL(proc));
    
    for (TInt i=0; i<proc.Count(); i++)
        result.append(new MemSpyServer(proc[i]));
    
    return result;
}

QList<MemSpyEComCategory*> EngineWrapper::getEComCategories()
{
    QList<MemSpyEComCategory*> result;
        
    RArray<CMemSpyApiEComCategory*> cat;
    
    QT_TRAP_THROWING(mSession.GetEComCategoriesL(cat));
    
    for (TInt i=0; i<cat.Count(); i++)
        result.append(new MemSpyEComCategory(cat[i]));
    
    return result;
}

QList<MemSpyEComInterface*> EngineWrapper::getEComInterfaces(int categoryId)
{
    QList<MemSpyEComInterface*> result;
            
    RArray<CMemSpyApiEComInterface*> interfaces;
    
    QT_TRAP_THROWING(mSession.GetEComInterfacesL(TUid::Uid(categoryId), interfaces));
    
    for (TInt i=0; i<interfaces.Count(); i++)
        result.append(new MemSpyEComInterface(interfaces[i]));
    
    return result;
}

QList<MemSpyEComImplementation*> EngineWrapper::getEComImplementations(int interfaceId)
{
    QList<MemSpyEComImplementation*> result;
                
    RArray<CMemSpyApiEComImplementation*> implementations;
    
    QT_TRAP_THROWING(mSession.GetEComImplementationsL(TUid::Uid(interfaceId), implementations));
    
    for (TInt i=0; i<implementations.Count(); i++)
        result.append(new MemSpyEComImplementation(implementations[i]));
    
    return result;
}

QList<MemSpyWindowGroup*> EngineWrapper::getWindowGroups()
{
    QList<MemSpyWindowGroup*> result;
                    
    RArray<CMemSpyApiWindowGroup*> groups;
    
    QT_TRAP_THROWING(mSession.GetWindowGroupsL(groups));
    
    for (TInt i=0; i<groups.Count(); i++)
        result.append(new MemSpyWindowGroup(groups[i]));
    
    return result;
}


MemSpyDwoTracker* EngineWrapper::createDeviceWideOperation(DeviceWideOperation operation)
{
	return new MemSpyDwoTracker(mSession, operation);
}

MemSpyKernelHeapDumpTracker* EngineWrapper::createKernelHeapDumpTracker()
{
	return new MemSpyKernelHeapDumpTracker(mSession);
}

MemSpySwmtDumpTracker* EngineWrapper::createSwmtDumpTracker()
{
	return new MemSpySwmtDumpTracker(mSession);
}

void EngineWrapper::setSwmtSettings(SwmtMode mode, const QVariantList& categories)
{
	int bits = 0;
	bool heapDumps = false;
	switch (mode)
	{
	case SwmtModeBasic:
		bits = TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryUserHeap | 
			//TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryUserStacks |
			//TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryGlobalData |
			TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategorySystemMemory;
		
		break;
		
	case SwmtModeFull:
		bits = TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryAll;
		heapDumps = true;
		
		break;
		
	case SwmtModeCustom:
		bits = 0;
		// this needs to be in sync. with swmt view categories
		TMemSpyEngineHelperSysMemTrackerConfig::TMemSpyEngineSysMemTrackerCategories cats[] = {
            TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryFileServerCache,
            //TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryBitmapHandles,
            TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryUserHeap,
            //TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryKernelHeap,
            TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryLocalChunks,
            TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryGlobalChunks,
            TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryRAMDrive,
            //TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryUserStacks,
            //TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryGlobalData,
            TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryRAMLoadedCode,
            TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryKernelHandles,
            TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryOpenFiles,
            TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryDiskusage,
            TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategorySystemMemory,
            TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryWindowGroups
		};

		foreach (const QVariant& bit, categories)
			bits |= cats[bit.toInt()];
		
		break;
	}
	
	TRAP_IGNORE(mSession.SetSwmtCategoriesL(bits));
	TRAP_IGNORE(mSession.SetSwmtHeapDumpsEnabledL(heapDumps));
}

bool EngineWrapper::isSwmtRunning()
{
	return mSwmtRunning;
}
	
void EngineWrapper::startSwmt(int period)
{
	mSwmtRunning = true;
	updateOutputSettings();
	TRAP_IGNORE(mSession.StartSwmtTimerL(period));
}

void EngineWrapper::stopSwmt()
{
	mSwmtRunning = false;
	TRAP_IGNORE(mSession.StopSwmtTimerL());
}

void EngineWrapper::forceSwmtDump()
{
	updateOutputSettings();	
	TRAP_IGNORE(mSession.ForceSwmtUpdateL());
}

void EngineWrapper::outputKernelHeapData()
{
	updateOutputSettings();
	TRAP_IGNORE(mSession.OutputKernelHeapDataL());
}

int EngineWrapper::outputThreadHeapData(const QString& filter)
{
	TPtrC customFilterDesc(static_cast<const TUint16*>(filter.utf16()), filter.length());
	
	TRAPD(err, mSession.OutputThreadHeapDataL(customFilterDesc));
	
	return err;
}

void EngineWrapper::updateOutputSettings()
{
	switch (mSettings.outputMode()) {
		case OutputModeTrace:
			TRAP_IGNORE(mSession.SwitchOutputToTraceL());
			break;
		
		case OutputModeFile:
		
			if (mSettings.outputPath().isEmpty()) {
				TRAP_IGNORE(mSession.SwitchOutputToFileL(KNullDesC));
			
			} else {
				
				QString root = mSettings.outputPath();
				TPtrC rootDesc (static_cast<const TUint16*>(root.utf16()), root.length());
			
				TRAP_IGNORE(mSession.SwitchOutputToFileL(rootDesc));
			}
	
	}
}

void EngineWrapper::killProcess(ProcessId process)
{
    TRAP_IGNORE(mSession.EndProcessL(process, EKill));
}
    
void EngineWrapper::panicProcess(ProcessId process)
{
    TRAP_IGNORE(mSession.EndProcessL(process, EPanic));
}

void EngineWrapper::terminateProcess(ProcessId process)
{
    TRAP_IGNORE(mSession.EndProcessL(process, ETerminate));
}
