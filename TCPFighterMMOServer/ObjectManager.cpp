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
        // 오브젝트가 비활성화 되었다면
        if ((*it).second->isDead()) { 
            NotifyClientDisconnected((*it).second->m_pSession); // 세션이 죽었음을 알림

            delete (*it).second;                // 플레이어 삭제
            it = m_ObjectHashMap.erase(it);     // 리스트에서 iter 삭제
        }
        else {
            (*it).second->LateUpdate();
            ++it; // 조건에 맞지 않으면 다음 요소로 이동
        }
    }
}

void CObjectManager::RegisterObject(CObject* pObject)
{
    m_ObjectHashMap.emplace(pObject->m_ID, pObject);
}
