#include "pch.h"
#include "ObjectManager.h"

#include "Session.h"
#include "SessionManager.h"

#include "SectorManager.h"

static CSectorManager& sectorManager = CSectorManager::GetInstance();

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
        // 오브젝트가 비활성화 되었다면
        if ((*it).second->isDead()) { 
            NotifyClientDisconnected((*it).second->m_pSession); // 세션이 죽었음을 알림
        }
        else {
            (*it).second->LateUpdate();
        }
        ++it; // 다음 요소로 이동
    }
}

void CObjectManager::RegisterObject(CObject* pObject)
{
    m_ObjectHashMap.emplace(pObject->m_ID, pObject);
}

void CObjectManager::DeleteObject(CObject* pObject)
{
    m_ObjectHashMap.erase(pObject->m_ID);
}
