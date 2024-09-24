#include "pch.h"
#include "ObjectManager.h"

#include "Session.h"
#include "SessionManager.h"

#include "SectorManager.h"

CObjectManager::CObjectManager() noexcept
{
}

CObjectManager::~CObjectManager() noexcept
{
    for (auto& mapIndex : m_ObjectHashMap)
    {
        delete mapIndex.second;
    }

    m_ObjectHashMap.clear();
}

void CObjectManager::Update(void)
{
    for (auto& mapIndex : m_ObjectHashMap)
    {
        mapIndex.second->Update();
    }
}

void CObjectManager::LateUpdate(void)
{
    auto it = m_ObjectHashMap.begin();
    while (it != m_ObjectHashMap.end())
    {
        // ������Ʈ�� ��Ȱ��ȭ �Ǿ��ٸ�
        if ((*it).second->isDead()) { 
            NotifyClientDisconnected((*it).second->m_pSession); // ������ �׾����� �˸�

            delete (*it).second;                // �÷��̾� ����
            it = m_ObjectHashMap.erase(it);     // ����Ʈ���� iter ����
        }
        else {
            (*it).second->LateUpdate();
            ++it; // ���ǿ� ���� ������ ���� ��ҷ� �̵�
        }
    }
}

void CObjectManager::RegisterObject(CObject* pObject)
{
    m_ObjectHashMap.emplace(pObject->m_ID, pObject);
}
