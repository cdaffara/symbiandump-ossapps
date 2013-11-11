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

#include <QStringListModel>

#include "memspykernelobjectview.h"
#include "viewmanager.h"

MemSpyKernelObjectModel::MemSpyKernelObjectModel(EngineWrapper &engine, int objectType, QObject *parent) :
	QAbstractListModel(parent),
	mObjects(engine.getKernelObjects(objectType))
{
}

MemSpyKernelObjectModel::~MemSpyKernelObjectModel()
{
	qDeleteAll(mObjects);
}
	
int MemSpyKernelObjectModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return mObjects.count();
}
	
QVariant MemSpyKernelObjectModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole) {
		QStringList lines;
		lines << mObjects.at(index.row())->name();
		
		return lines;
	}
	
	if (role == Qt::UserRole) {
		return qVariantFromValue<void*>(mObjects.at(index.row()));
	}
	
	return QVariant();
}

void MemSpyKernelObjectView::initialize(const QVariantMap& params)
{
	QStringList list = QStringList() << "Threads" << "Processes" << "Chunks" << "Libraries" <<
			"Semaphores" << "Mutexes" << "Timers" << "Servers" << "Sessions" << "Logical Devices" <<
			"Physical Devices" << "Logical Channels" << "Change Notifiers" << "Undertakers" <<
			"Message Queues" << "Property Refs." << "Conditional Vars.";
	
	int type = params.value("type").toInt();
	
	setTitle(list.at(type));
	
	MemSpyView::initialize(params);
	
	//mListView.setModel(new MemSpyKernelObjectTypeModel(mEngine, this));
	mListView.setModel(new MemSpyKernelObjectModel(mEngine, type, this));
	
	connect(&mListView, SIGNAL(activated(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
}

bool MemSpyKernelObjectView::isBreadCrumbVisible() const
{
    return true;
}
            
QString MemSpyKernelObjectView::getBreadCrumbText() const
{
    return tr("Kernel Objects");
}


void MemSpyKernelObjectView::itemClicked(const QModelIndex& index)
{
	QVariantMap map;
	map.insert("details", getDetails(static_cast<MemSpyKernelObject*>(qVariantValue<void*>(index.data(Qt::UserRole)))));
	map.insert("typeName", title());
	map.insert("objectName", static_cast<MemSpyKernelObject*>(qVariantValue<void*>(index.data(Qt::UserRole)))->nameDetail());
    mViewManager.showView(KernelObjectDetailView, map);
}

QStringList MemSpyKernelObjectView::getDetails(MemSpyKernelObject *object)
{
	QStringList result;
	
	result << QString("%1: %2").arg(tr("Name")).arg(object->nameDetail());
	result << QString("%1: %2").arg(tr("Full Name")).arg(object->name());
	result << QString("%1: %2").arg(tr("AccessCount")).arg(object->accessCount());
	result << QString("%1: %2").arg(tr("UniqueID")).arg(object->uniqueId());
	result << QString("%1: %2").arg(tr("Protection")).arg(object->protection());
	result << QString("%1: %2").arg(tr("OwnrAddr")).arg(object->addressOfKernelOwner());
	result << QString("%1: %2").arg(tr("KernelAddr")).arg(object->kernelAddress());
		    
	    // Object type specific attributes:
	    switch (object->type())
	        {
	        case KernelObjectTypeUnknown:
	            {
	            break;
	            }
	        case KernelObjectTypeThread:
	            {
	            result << QString("%1: %2").arg(tr("OwnrPrAddr")).arg(object->addressOfOwningProcess());
	            result << QString("%1: %2").arg(tr("ThreadID")).arg(object->id());
	            result << QString("%1: %2").arg(tr("Priority")).arg(object->priority());
	            result << QString("%1: %2").arg(tr("Proc")).arg(object->nameOfOwner());
	            break;
	            }
	        case KernelObjectTypeProcess:
	            {
	            result << QString("%1: %2").arg(tr("OwnrPrAddr")).arg(object->addressOfOwningProcess());
	            result << QString("%1: %2").arg(tr("CreatorId")).arg(object->creatorId());
	            result << QString("%1: %2").arg(tr("Attributes")).arg(object->attributes());
	            result << QString("%1: %2").arg(tr("StckChnk")).arg(object->addressOfDataBssStackChunk());
	            result << QString("%1: %2").arg(tr("ProcessID")).arg(object->id());
	            result << QString("%1: %2").arg(tr("Priority")).arg(object->priority());
	            result << QString("%1: %2").arg(tr("SecurityZone")).arg(object->securityZone());
	            
	            /* TODO: to solve process details
	            CMemSpyEngineObjectContainer& container = iEngine.Container();
	            TProcessId id( iKernelObjectItems[iListBox->CurrentItemIndex().iId );
	            TRAP_IGNORE(
	                CMemSpyProcess& process = container.ProcessByIdL( id );
	                AppendFormatString( messagePtr, _L("\nSID: 0x%08X\n"), process.SID() );
	                AppendFormatString( messagePtr, _L("VID: 0x%08X\n"), process.VID() );
	                AppendFormatString( messagePtr, _L("UID1: 0x%08X\n"), process.UIDs()[0].iUid  );
	                AppendFormatString( messagePtr, _L("UID2: 0x%08X\n"), process.UIDs()[1].iUid  );
	                AppendFormatString( messagePtr, _L("UID3: 0x%08X\n"), process.UIDs()[2].iUid );
	                AppendFormatString( messagePtr, _L("Caps: 0x%08X%08X"), process.Capabilities().iCaps[0], process.Capabilities().iCaps[1]);
	                );
	            */
	            break;
	            }
	        case KernelObjectTypeChunk:
	            {
	            result << QString("%1: %2").arg(tr("OwnrPrAddr")).arg(object->addressOfOwningProcess());
	            result << QString("%1: %2").arg(tr("Size")).arg(object->size());
	            result << QString("%1: %2").arg(tr("MaxSize")).arg(object->maxSize());
	            result << QString("%1: %2").arg(tr("Bottom")).arg(object->bottom());
	            result << QString("%1: %2").arg(tr("Top")).arg(object->top());
	            result << QString("%1: %2").arg(tr("Attr")).arg(object->attributes());
	            result << QString("%1: %2").arg(tr("Start")).arg(object->startPos());
	            result << QString("%1: %2").arg(tr("CntrlID")).arg(object->controllingOwner());
	            result << QString("%1: %2").arg(tr("Restrictions")).arg(object->restrictions());
	            result << QString("%1: %2").arg(tr("MapAttr")).arg(object->mapAttr());
	            result << QString("%1: %2").arg(tr("Type")).arg(object->chunkType());
	            result << QString("%1: %2").arg(tr("Proc")).arg(object->nameOfOwner());
	            break;
	            }
	        case KernelObjectTypeLibrary:
	            {
	            result << QString("%1: %2").arg(tr("MapCount")).arg(object->mapCount());
	            result << QString("%1: %2").arg(tr("State")).arg(object->state());
	            result << QString("%1: %2").arg(tr("CodeSeg")).arg(object->addressOfCodeSeg());
	            break;
	            }
	        case KernelObjectTypeSemaphore:
	            {
	            result << QString("%1: %2").arg(tr("Count")).arg(object->count());
	            result << QString("%1: %2").arg(tr("Resetting")).arg(object->resetting());
	            break;
	            }
	        case KernelObjectTypeMutex:
	            {
	            result << QString("%1: %2").arg(tr("HoldCount")).arg(object->count());
	            result << QString("%1: %2").arg(tr("WaitCount")).arg(object->waitCount());
	            result << QString("%1: %2").arg(tr("Resetting")).arg(object->resetting());
	            result << QString("%1: %2").arg(tr("Order")).arg(object->order());
	            break;
	            }
	        case KernelObjectTypeTimer:
	            {
	            result << QString("%1: %2").arg(tr("State")).arg(object->timerState());
	            result << QString("%1: %2").arg(tr("Type")).arg(object->timerType());
	            break;
	            }
	        case KernelObjectTypeServer:
	            {
	            result << QString("%1: %2").arg(tr("ThrdAddr")).arg(object->addressOfOwningThread());
	            result << QString("%1: %2").arg(tr("Thr")).arg(object->nameOfOwner());
	            result << QString("%1: %2").arg(tr("Type")).arg(object->sessionType());
//	            RArray<TMemSpyDriverServerSessionInfo> sessions;
//	            CleanupClosePushL( sessions );
	            /* TODO: to solve server sessions
	            iEngine.HelperServer().GetServerSessionsL( iKernelObjectItems[iListBox->CurrentItemIndex(), sessions );
	            const TInt count = sessions.Count();
	            for ( TInt i = 0; i < count; i++ )
	                {
	                const TMemSpyDriverServerSessionInfo& session = sessions[ i ];
	                AppendFormatString( messagePtr, _L("SessAddr: 0x%08X\n"), session.iAddress );
	                TFullName sessName;
	                sessName.Copy( session.iName );
	                AppendFormatString( messagePtr, _L("Sess: %S\n"), &sessName );
	                }
	            CleanupStack::PopAndDestroy( &sessions );
	            */
	            break;
	            }
	        case KernelObjectTypeSession:
	            {
	            result << QString("%1: %2").arg(tr("Server")).arg(object->addressOfServer());
				result << QString("%1: %2").arg(tr("Srv")).arg(object->name());
				result << QString("%1: %2").arg(tr("AccCount")).arg(object->totalAccessCount());
				result << QString("%1: %2").arg(tr("SesType")).arg(object->sessionType());
				result << QString("%1: %2").arg(tr("SvrType")).arg(object->svrSessionType());
				result << QString("%1: %2").arg(tr("MsgCount")).arg(object->msgCount());
				result << QString("%1: %2").arg(tr("MsgLimit")).arg(object->msgLimit());

	            break;
	            }
	        case KernelObjectTypeLogicalDevice:
	            {
	            result << QString("%1: %2").arg(tr("Version")).arg(object->version());
				result << QString("%1: %2").arg(tr("ParseMask")).arg(object->parseMask());
				result << QString("%1: %2").arg(tr("UnitsMask")).arg(object->unitsMask());
				result << QString("%1: %2").arg(tr("Open channels")).arg(object->openChannels());
	            break;
	            }
	        case KernelObjectTypePhysicalDevice:
	            {
	            result << QString("%1: %2").arg(tr("Version")).arg(object->version());
				result << QString("%1: %2").arg(tr("UnitsMask")).arg(object->unitsMask());
				result << QString("%1: %2").arg(tr("CodeSeg")).arg(object->addressOfCodeSeg());
	            break;
	            }
	        case KernelObjectTypeLogicalChannel:
	            {
	            // No other details
	            break;
	            }
	        case KernelObjectTypeChangeNotifier:
	            {
	            result << QString("%1: %2").arg(tr("Changes")).arg(object->changes());
				result << QString("%1: %2").arg(tr("ThrdAddr")).arg(object->addressOfOwningThread());
				result << QString("%1: %2").arg(tr("Thr")).arg(object->nameOfOwner());
	            break;
	            }
	        case KernelObjectTypeUndertaker:
	            {
	            result << QString("%1: %2").arg(tr("ThrdAddr")).arg(object->addressOfOwningThread());
	            result << QString("%1: %2").arg(tr("Thr")).arg(object->nameOfOwner());
				break;
	            }
	        case KernelObjectTypeMsgQueue:
	            {
	            // No other details
	            break;
	            }
	        case KernelObjectTypePropertyRef:
	            {
	            /*
	            Not listing details here, as propertyRef is not listed in TaskMgr.
	            Following propertyRef attributes are available at engine side. 
	            
	            IsReady
	            Type
	            Category
	            Key
	            RefCount
	            ThreadId
	            CreatorSID
	            */
	            break;
	            }
	        case KernelObjectTypeCondVar:
	            {
	            result << QString("%1: %2").arg(tr("Resetting")).arg(object->resetting());
				result << QString("%1: %2").arg(tr("Mutex")).arg(object->addressOfOwningThread());
				result << QString("%1: %2").arg(tr("Mtx")).arg(object->nameOfOwner());
				result << QString("%1: %2").arg(tr("WaitCount")).arg(object->waitCount());
	            
//	            RArray<TMemSpyDriverCondVarSuspendedThreadInfo> threads;
//	            CleanupClosePushL( threads );
	            /* TODO: to solve thread cond. vars.
	            iEngine.HelperCondVar().GetCondVarSuspendedThreadsL( iKernelObjectItems[iListBox->CurrentItemIndex(), threads );
	            const TInt count = threads.Count();
	            for ( TInt i = 0; i < count; i++ )
	                {
	                const TMemSpyDriverCondVarSuspendedThreadInfo& thr = threads[ i ];
	                AppendFormatString( messagePtr, _L("SuspThrdAddr: 0x%08X\n"), thr.iAddress );
	                TFullName thrName;
	                thrName.Copy( thr.iName );
	                AppendFormatString( messagePtr, _L("Thr: %S\n"), &thrName );
	                }
	            CleanupStack::PopAndDestroy( &threads );
	            */
	            break;
	            }
//	        default:
//	            {
//	            // Programming error
//	            __ASSERT_ALWAYS( EFalse, User::Panic( _L("MemSpy-View"), 0 ) );
//	            }
	        }
	
	return result;
}
