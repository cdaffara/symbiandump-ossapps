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

#ifndef ENGINEWRAPPER_H_
#define ENGINEWRAPPER_H_

#include <QObject>
#include <QVariantList>
#include <QSettings>

#include <memspysession.h>

typedef quint64 ProcessId;
typedef quint64 ThreadId;

enum ThreadPriority
{
	ThreadPriorityNull=(-30),
	ThreadPriorityMuchLess=(-20),
	ThreadPriorityLess=(-10),
	ThreadPriorityNormal=0,
	ThreadPriorityMore=10,
	ThreadPriorityMuchMore=20,
	ThreadPriorityRealTime=30,
	ThreadPriorityAbsoluteVeryLow=100,
	ThreadPriorityAbsoluteLowNormal=150,
	ThreadPriorityAbsoluteLow=200,
	ThreadPriorityAbsoluteBackgroundNormal=250,
	ThreadPriorityAbsoluteBackground=300,
	ThreadPriorityAbsoluteForegroundNormal=350,
	ThreadPriorityAbsoluteForeground=400,
	ThreadPriorityAbsoluteHighNormal=450,
	ThreadPriorityAbsoluteHigh=500,
	ThreadPriorityAbsoluteRealTime1=810,
	ThreadPriorityAbsoluteRealTime2=820,
	ThreadPriorityAbsoluteRealTime3=830,
	ThreadPriorityAbsoluteRealTime4=840,
	ThreadPriorityAbsoluteRealTime5=850,
	ThreadPriorityAbsoluteRealTime6=860,
	ThreadPriorityAbsoluteRealTime7=870, 
	ThreadPriorityAbsoluteRealTime8=880
};

enum KernelObjectType
{
	KernelObjectTypeUnknown = -1,
	KernelObjectTypeThread = 0,
	KernelObjectTypeProcess,
	KernelObjectTypeChunk,
	KernelObjectTypeLibrary,
	KernelObjectTypeSemaphore,
	KernelObjectTypeMutex,
	KernelObjectTypeTimer,
	KernelObjectTypeServer,
	KernelObjectTypeSession,
	KernelObjectTypeLogicalDevice,
	KernelObjectTypePhysicalDevice,
	KernelObjectTypeLogicalChannel,
	KernelObjectTypeChangeNotifier,
	KernelObjectTypeUndertaker,
	KernelObjectTypeMsgQueue,
	KernelObjectTypePropertyRef,
	KernelObjectTypeCondVar
};

enum ThreadInfoType
{
	ThreadInfoTypeGeneral = EMemSpyThreadInfoItemTypeGeneralInfo,
	ThreadInfoTypeHeap = EMemSpyThreadInfoItemTypeHeap,
	ThreadInfoTypeStack = EMemSpyThreadInfoItemTypeStack,
	ThreadInfoTypeChunk = EMemSpyThreadInfoItemTypeChunk,
	ThreadInfoTypeCodeSeg = EMemSpyThreadInfoItemTypeCodeSeg,
	ThreadInfoTypeOpenFiles = EMemSpyThreadInfoItemTypeOpenFiles,
	ThreadInfoTypeActiveObjects = EMemSpyThreadInfoItemTypeActiveObject,
	ThreadInfoTypeOwnedThreadHandles = EMemSpyThreadInfoItemTypeOwnedThreadHandles,
	ThreadInfoTypeOwnedProcessHandles = EMemSpyThreadInfoItemTypeOwnedProcessHandles,
	ThreadInfoTypeServer = EMemSpyThreadInfoItemTypeServer,
	ThreadInfoTypeSession = EMemSpyThreadInfoItemTypeSession,
	ThreadInfoTypeSemaphore = EMemSpyThreadInfoItemTypeSemaphore,
	ThreadInfoTypeOtherThreads = EMemSpyThreadInfoItemTypeOtherThreads,
	ThreadInfoTypeOtherProcesses = EMemSpyThreadInfoItemTypeOtherProcesses,
	ThreadInfoTypeMutex = EMemSpyThreadInfoItemTypeMutex,
	ThreadInfoTypeTimer = EMemSpyThreadInfoItemTypeTimer,
	ThreadInfoTypeChannel = EMemSpyThreadInfoItemTypeLogicalChannel,
	ThreadInfoTypeChangeNotifier = EMemSpyThreadInfoItemTypeChangeNotifier,
	ThreadInfoTypeUndertaker = EMemSpyThreadInfoItemTypeUndertaker,
	ThreadInfoTypeMessageQueue = EMemSpyThreadInfoItemTypeMessageQueue,
	ThreadInfoTypeConditionalVariable = EMemSpyThreadInfoItemTypeConditionalVariable,
	ThreadInfoTypeLDD = EMemSpyThreadInfoItemTypeLDD,
	ThreadInfoTypePDD = EMemSpyThreadInfoItemTypePDD,
};

enum DeviceWideOperation
{
	OutputPhoneInfo = 0,
	    
	OutputDetailedPhoneInfo,
	    
	OutputHeapInfo,
	    
	OutputCompactHeapInfo,
	    
	OutputHeapCellListing,
	    
	OutputHeapData,
	    
	OutputStackInfo,
	    
	OutputCompactStackInfo,
	    
	OutputUserStackData,
	    
	OutputKernelStackData
};

enum SwmtMode
{
	SwmtModeBasic = 0,
	SwmtModeFull,
	SwmtModeCustom
};

enum HeapDumpsMode
{
	HeapDumpsModeKernel = 0,
	HeapDumpsModeUser,
	HeapDumpsModeBoth
};

enum OutputMode
{
	OutputModeTrace = 0,
	OutputModeFile
};

class MemSpyProcess
{
public:
	MemSpyProcess(CMemSpyApiProcess* process)
		: mProcess(process)
	{}
	
	virtual  ~MemSpyProcess() { delete mProcess; }
	
	ProcessId id() const { return mProcess->Id(); }
	
	QString name() const { return QString((QChar*) mProcess->Name().Ptr(), mProcess->Name().Length()); }
	
	QString exitInfo() const;
	
	int priority() const { return mProcess->Priority(); }
	
	int threadCount() const { return mProcess->ThreadCount(); }
	
	int sid() const { return mProcess->SID(); }
	
	int vid() const { return mProcess->VID(); }
	
	bool isDead() const { return mProcess->IsDead(); }
	
private:
	CMemSpyApiProcess *mProcess;
};

class MemSpyThread
{
public:
	MemSpyThread(CMemSpyApiThread* thread)
		: mThread(thread)
	{}
	
	virtual ~MemSpyThread() { delete mThread;	}
	
	ThreadId id() const { return mThread->Id(); }
	
	QString name() const { return QString((QChar*) mThread->Name().Ptr(), mThread->Name().Length()); }
	
	int priority() const { return mThread->ThreadPriority(); }
	
	
private:
	CMemSpyApiThread *mThread;
};

class MemSpyKernelObjectType
{
public:
	MemSpyKernelObjectType(CMemSpyApiKernelObject* type)
		: mType(type)
	{}
	
	virtual ~MemSpyKernelObjectType() { delete mType; }
	
	int id() const { return mType->Type(); }
	
	QString name() const { return QString((QChar*) mType->Name().Ptr(), mType->Name().Length()); }
	
	int count() const { return mType->Count(); }
	
	qint64 size() const { return mType->Size(); }
	
private:
	CMemSpyApiKernelObject *mType;
};

class MemSpyKernelObject
{
public:
	MemSpyKernelObject(CMemSpyApiKernelObjectItem* object)
		: mObject(object)
	{}
	
	virtual ~MemSpyKernelObject() { delete mObject; }
	
	int type() const { return mObject->Type(); }
	
	QString name() const { return QString::fromLatin1((char*)mObject->Name().Ptr(), mObject->Name().Length()); }
	
	QString nameDetail() const { return QString::fromLatin1((char*)mObject->NameDetail().Ptr(), mObject->NameDetail().Length()); }
	
	int accessCount() const { return mObject->AccessCount(); }
	
	int uniqueId() const { return mObject->UniqueID(); }
	
	unsigned int protection() const { return mObject->Protection(); }
	
	unsigned int addressOfKernelOwner() const { return reinterpret_cast<unsigned int>(mObject->AddressOfKernelOwner()); }
	
	unsigned int kernelAddress() const { return reinterpret_cast<unsigned int>(mObject->Handle()); }
	
	unsigned int addressOfOwningProcess() const { return reinterpret_cast<unsigned int>(mObject->AddressOfOwningProcess()); }
	
	int id() const { return mObject->Id(); }
	
	int priority() const { return mObject->Priority(); }
	
	QString nameOfOwner() const { return QString::fromLatin1((char*)mObject->NameOfOwner().Ptr(), mObject->NameOfOwner().Length()); }
	
	unsigned int creatorId() const { return mObject->CreatorId(); }
	
	int attributes() const { return mObject->Attributes(); }
	
	unsigned int addressOfDataBssStackChunk() const { return reinterpret_cast<unsigned int>(mObject->AddressOfDataBssStackChunk()); }
	
	unsigned int securityZone() const { return mObject->SecurityZone(); }
	
	unsigned int size() const { return mObject->Size(); }
	
	unsigned int maxSize() const { return mObject->MaxSize(); }
	
	unsigned int bottom() const { return mObject->Bottom(); }
	
	unsigned int top() const { return mObject->Top(); }
	
	unsigned int startPos() const { return mObject->StartPos(); }
	
	unsigned int controllingOwner() const { return mObject->ControllingOwner(); }
	
	unsigned int restrictions() const { return mObject->Restrictions(); }
	
	unsigned int mapAttr() const { return mObject->Restrictions(); }
	
	unsigned int chunkType() const { return mObject->ChunkType(); }
	
	int mapCount() const { return mObject->MapCount(); }
	
	unsigned int state() const { return mObject->State(); }
	
	unsigned int addressOfCodeSeg() const { return reinterpret_cast<unsigned int>(mObject->AddressOfCodeSeg()); }
	
	unsigned int resetting() const { return mObject->Resetting(); }
	
	unsigned int order() const { return mObject->Order(); }
	
	QString version() const { return QString((QChar*) mObject->Version().Name().Ptr(), mObject->Version().Name().Length()); }
	
	unsigned int parseMask() const { return mObject->ParseMask(); }
	
	unsigned int unitsMask() const { return mObject->UnitsMask(); }
	
	unsigned int changes() const { return mObject->Changes(); }
	
	int count() const { return mObject->Count(); }
	
	int waitCount() const { return mObject->WaitCount(); }

	int sessionType() const { return mObject->SessionType(); }

	int timerType() const { return mObject->TimerType(); }
	
	int timerState() const { return mObject->TimerState(); }
	
	unsigned int addressOfOwningThread() const { return reinterpret_cast<unsigned int>(mObject->AddressOfOwningThread()); }
	
	unsigned int addressOfServer() const { return reinterpret_cast<unsigned int>(mObject->AddressOfServer()); }
	
	unsigned int svrSessionType() const { return mObject->SvrSessionType(); }
	
	int msgCount() const { return mObject->MsgCount(); }
	
	int msgLimit() const { return mObject->MsgLimit(); }
	
	unsigned int totalAccessCount() const { return mObject->TotalAccessCount(); }
	
	int openChannels() const { return mObject->OpenChannels(); }
	
private:
	CMemSpyApiKernelObjectItem *mObject;
};

class MemSpyThreadInfoItem 
{
public:
	MemSpyThreadInfoItem(CMemSpyApiThreadInfoItem *item)
		: mItem(item)
	{}
	
	virtual  ~MemSpyThreadInfoItem() { delete mItem;	}
	
	QString caption() const { return QString((QChar*) mItem->Caption().Ptr(), mItem->Caption().Length()); }
	
	QString value() const { return QString((QChar*) mItem->Value().Ptr(), mItem->Value().Length()); }
	
private:
	CMemSpyApiThreadInfoItem* mItem;
};

class MemSpyServer
{
public:
    MemSpyServer(CMemSpyApiServer* server)
        : mServer(server)
    {}
    
    virtual  ~MemSpyServer() { delete mServer; }
    
    ProcessId processId() const { return mServer->ProcessId(); }
    
    ThreadId threadId() const { return mServer->ThreadId(); }
    
    QString name() const { return QString((QChar*) mServer->Name().Ptr(), mServer->Name().Length()); }
        
    int sessionCount() const { return mServer->SessionCount(); }
    
private:
    CMemSpyApiServer *mServer;
};

class MemSpyEComCategory
{
public:
    MemSpyEComCategory(CMemSpyApiEComCategory* category)
        : mData(category)
    {}
    
    virtual  ~MemSpyEComCategory() { delete mData; }
    
    int id() const { return mData->Id().iUid; }
    
    QString name() const { return QString((QChar*) mData->Name().Ptr(), mData->Name().Length()); }
        
    int interfaceCount() const { return mData->InterfaceCount(); }
    
private:
    CMemSpyApiEComCategory *mData;
};

class MemSpyEComInterface
{
public:
    MemSpyEComInterface(CMemSpyApiEComInterface* interface)
        : mData(interface)
    {}
    
    virtual  ~MemSpyEComInterface() { delete mData; }
    
    int id() const { return mData->Id().iUid; }
    
    int categoryId() const { return mData->CategoryId().iUid; }
    
    QString name() const { return QString((QChar*) mData->Name().Ptr(), mData->Name().Length()); }
        
    int implementationCount() const { return mData->ImplementationCount(); }
    
private:
    CMemSpyApiEComInterface *mData;
};

class MemSpyEComImplementation
{
public:
    MemSpyEComImplementation(CMemSpyApiEComImplementation* interface)
        : mData(interface)
    {}
    
    virtual  ~MemSpyEComImplementation() { delete mData; }
    
    int id() const { return mData->ImplementationUid().iUid; }
    
    QString name() const { return QString((QChar*) mData->Name().Ptr(), mData->Name().Length()); }
    
    int version() const { return mData->Version(); }
    
    QString dataType() const { return QString((QChar*) mData->DataType().Ptr(), mData->DataType().Length()); }
    
    QString opaqueData() const { return QString((QChar*) mData->OpaqueData().Ptr(), mData->OpaqueData().Length()); }
    
    QString drive() const { return QString((QChar*) mData->Drive().Name().Ptr(), mData->Drive().Name().Length()); }
    
    bool romOnly() const { return mData->RomOnly(); }
    
    bool romBased() const { return mData->RomBased(); }
    
    int vendorId() const { return mData->VendorId(); }
    
    bool disabled() const { return mData->Disabled(); }
    
private:
    CMemSpyApiEComImplementation *mData;
};

class MemSpyWindowGroup
{
public:
    MemSpyWindowGroup(CMemSpyApiWindowGroup* group)
        : mData(group)
    {}
    
    virtual  ~MemSpyWindowGroup() { delete mData; }
    
    int id() const { return mData->Id(); }
    
    QString name() const { return QString((QChar*) mData->Name().Ptr(), mData->Name().Length()); }
    
    QString fullName() const { return QString((QChar*) mData->FullName().Ptr(), mData->FullName().Length()); }
    
    ThreadId threadId() const { return mData->ThreadId(); }
    
    int ordinalPosition() const { return mData->OrdinalPosition(); }
    
    int priority() const { return mData->Priority(); }
    
    int windowGroupHandle() const { return mData->WindowGroupHandle(); }
    
    int uid() const { return mData->Uid().iUid; }
    
    bool isBusy() const { return mData->IsBusy(); }
    
    bool isSystem() const { return mData->IsSystem(); }
    
    bool isHidden() const { return mData->IsHidden(); }
        
    bool isFocused() const { return mData->IsFocused(); }
    
    QString caption() const { return QString((QChar*) mData->Caption().Ptr(), mData->Caption().Length()); }
    
    QString docName() const { return QString((QChar*) mData->DocName().Ptr(), mData->DocName().Length()); }
    
private:
    CMemSpyApiWindowGroup *mData;
};


class MemSpyDwoProgressTracker : public QObject, public CActive
{
	Q_OBJECT

public:
	MemSpyDwoProgressTracker(RMemSpySession &session);
	virtual ~MemSpyDwoProgressTracker();
	
public slots:

	void start();

	void cancel();
	
protected: // from CActive
	
	virtual void RunL();
	 
	virtual void DoCancel();
	 
	virtual TInt RunError(TInt aError);
	
signals:
	void progress(int progress, const QString& processName);
	
private:
	TMemSpyDeviceWideOperationProgress mProgress;
	RMemSpySession mSession;
};

class MemSpyDwoTracker : public QObject, public CActive
{
	Q_OBJECT

public:
	MemSpyDwoTracker(RMemSpySession &session, DeviceWideOperation operation);
	virtual ~MemSpyDwoTracker();
	
public slots:

	void start();
	
	void cancel();
	
protected: // from CActive
	
	virtual void RunL();
	 
	virtual void DoCancel();
	 
	virtual TInt RunError(TInt aError);
	
signals:
	void finished(int errorCode);
	void progress(int progress, const QString& processName);
	
private:
	RMemSpySession mSession;
	MemSpyDwoProgressTracker *mProgressTracker;
	DeviceWideOperation mOperation;
};

class MemSpyAsyncTracker : public QObject, public CActive
{
	Q_OBJECT
	
public:
	
	MemSpyAsyncTracker(RMemSpySession& session, void (RMemSpySession::*function)(TRequestStatus&));
	
	void start();
	
	virtual void RunL();
	
	virtual void DoCancel();
	
	virtual TInt RunError(TInt aError);
	
signals:
	
	void finished(int errorCode);
	
private:
	void (RMemSpySession::*mFunction)(TRequestStatus&);
	RMemSpySession& mSession;
};

class MemSpySwmtDumpTracker : public MemSpyAsyncTracker
{
public:
	MemSpySwmtDumpTracker(RMemSpySession& session) : 
		MemSpyAsyncTracker(session, &RMemSpySession::ForceSwmtUpdate)
	{}
};

class MemSpyKernelHeapDumpTracker : public MemSpyAsyncTracker
{
public:
	MemSpyKernelHeapDumpTracker(RMemSpySession& session) : 
		MemSpyAsyncTracker(session, &RMemSpySession::OutputKernelHeapData)
	{}
};

class MemSpySettings : private QSettings
{
public:
	
	MemSpySettings();
	
	OutputMode outputMode() const;
	void setOutputMode(OutputMode mode);
	
	QString outputPath() const;
	void setOutputPath(const QString& path);
		
	int swmtTimerPeriod() const;
	void setSwmtTimerPeriod(int period);
	
	SwmtMode swmtMode() const;
	void setSwmtMode(SwmtMode mode);
	
	QVariantList swmtCategories() const;
	void setSwmtCategories(const QVariantList& categories);
	
	HeapDumpsMode heapDumpsMode() const;
	void setHeapDumpsMode(HeapDumpsMode mode);
};

class EngineWrapper : public QObject
{
	Q_OBJECT
	
public:
	EngineWrapper();
	
	virtual ~EngineWrapper();
	
	bool initialize();
	
	MemSpySettings& settings();
	
	const MemSpySettings& settings() const;
	
	
	
	QList<MemSpyProcess*> getProcesses();
	
	QList<MemSpyThread*> getThreads(ProcessId processId);
	
	QList<MemSpyThreadInfoItem*> getThreadInfo(ThreadId threadId, ThreadInfoType type);
	
	void setThreadPriority(ThreadId threadId, ThreadPriority priority);
	
	QList<MemSpyKernelObjectType*> getKernelObjectTypes();
	
	QList<MemSpyKernelObject*> getKernelObjects(int type);
	
	QList<MemSpyServer*> getServers();
	
	QList<MemSpyEComCategory*> getEComCategories();
	
	QList<MemSpyEComInterface*> getEComInterfaces(int categoryId);
	
	QList<MemSpyEComImplementation*> getEComImplementations(int interfaceId);
	
	QList<MemSpyWindowGroup*> getWindowGroups();
	
	MemSpyDwoTracker* createDeviceWideOperation(DeviceWideOperation operation);
	
	MemSpyKernelHeapDumpTracker* createKernelHeapDumpTracker();
	
	MemSpySwmtDumpTracker* createSwmtDumpTracker();
	
	void setSwmtSettings(SwmtMode mode, const QVariantList& categories);
	
	bool isSwmtRunning();
	
	void startSwmt(int period);
	
	void stopSwmt();
	
	void forceSwmtDump();
	
	void outputKernelHeapData();
	
	int outputThreadHeapData(const QString& filter);
	
	void updateOutputSettings();
	
	void killProcess(ProcessId process);
	
	void panicProcess(ProcessId process);
	
	void terminateProcess(ProcessId process);
	
private:
	RMemSpySession mSession;
	
	bool mSwmtRunning;
	
	MemSpySettings mSettings;
};

#endif /* ENGINEWRAPPER_H_ */
