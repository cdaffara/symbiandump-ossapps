#include <QObject>
#include <QVariant>

#include <XQCentralRepositorySearchCriteria.h>
#include <XQCentralRepositoryUtils.h>
#include <xqsettingsmanager.h>

//#include <XQCentralRepositorySearchCriteria.h>
//#include "XQCentralRepositoryUtils.h"
//#include "xqsettingsmanager.h"

QMap<unsigned long int, QString> UTmap;

XQSettingsManager::XQSettingsManager(QObject* parent)
    : QObject(parent)
{
//    Q_UNUSED(parent);
    UTmap.clear();

    QVariant n = 1;
    UTmap[0] = n.toString();

    n = 0;
    UTmap[1] = n.toString();
}

XQSettingsManager::~XQSettingsManager()
{
    UTmap.clear();
}
/*
QVariant XQSettingsManager::readItemValue(const XQCentralRepositorySettingsKey& key, XQSettingsManager::Type)
{
    return QVariant::fromValue(UTmap.value(key.key()));
}
*/


QVariant XQSettingsManager::readItemValue(const XQSettingsKey& key, XQSettingsManager::Type)
{
    return QVariant::fromValue(UTmap.value(key.key()));
}

/*
bool XQSettingsManager::writeItemValue(const XQCentralRepositorySettingsKey& key, const QVariant& value)
{
    UTmap[key.key()] = value.toString();
    return true;
}
*/

bool XQSettingsManager::writeItemValue(const XQSettingsKey& key, const QVariant& value)
{
    UTmap[key.key()] = value.toString();
    return true;
}

XQCentralRepositoryUtils::XQCentralRepositoryUtils(XQSettingsManager& settingsManager, QObject* parent)
{
    Q_UNUSED(settingsManager);
    Q_UNUSED(parent);
}

XQCentralRepositoryUtils::~XQCentralRepositoryUtils()
{
}

bool XQCentralRepositoryUtils::resetRepository(long int repositoryUid)
{
    Q_UNUSED(repositoryUid);
    UTmap.clear();
    return true;
}

bool XQCentralRepositoryUtils::createKey(const XQCentralRepositorySettingsKey& key, const QVariant& value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
    return true;
}

bool XQCentralRepositoryUtils::deleteKey(const XQCentralRepositorySettingsKey& key)
{
    Q_UNUSED(key);
    return true;
}

QList<XQCentralRepositorySettingsKey> XQCentralRepositoryUtils::findKeys(const XQCentralRepositorySearchCriteria& criteria)
    {
    QList<XQCentralRepositorySettingsKey> emptyList;

    int size = UTmap.size();
    int mockId = UTmap.key(criteria.value().toString());

    if (mockId)
        {
        XQCentralRepositorySettingsKey key(mockId, mockId);
        emptyList.append(key);
        }

    return emptyList;
    }

XQCentralRepositorySearchCriteria::XQCentralRepositorySearchCriteria(long int repositoryUid, unsigned long int partialKey, unsigned long int bitMask)
{
    Q_UNUSED(repositoryUid);
    Q_UNUSED(partialKey);
    Q_UNUSED(bitMask);
}

XQCentralRepositorySearchCriteria::~XQCentralRepositorySearchCriteria()
{
}

void XQCentralRepositorySearchCriteria::setValueCriteria(const QVariant& value, bool negativeCriteria)
    {
    Q_UNUSED(negativeCriteria);
    m_value = value;
    }

const QVariant& XQCentralRepositorySearchCriteria::value() const
{
    return m_value;
}

XQSettingsKey::XQSettingsKey(XQSettingsKey::Target target, long int uid, unsigned long int key) :
    m_target(target),
    m_uid(uid),
    m_key(key)
{

}

XQSettingsKey::~XQSettingsKey()
{
}

XQSettingsKey::Target XQSettingsKey::target() const
{
    return m_target;
}

long int XQSettingsKey::uid() const
{
    return m_uid;
}

unsigned long int XQSettingsKey::key() const
{
    return m_key;
}

XQCentralRepositorySettingsKey::XQCentralRepositorySettingsKey(long int uid, unsigned long int key) :
        XQSettingsKey(XQSettingsKey::TargetCentralRepository, uid, key)
{
}

XQCentralRepositorySettingsKey::~XQCentralRepositorySettingsKey()
    {
    }
