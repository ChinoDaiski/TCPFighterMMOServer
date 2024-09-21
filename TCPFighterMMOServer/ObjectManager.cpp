#include "pch.h"
#include "ObjectManager.h"

#include "Session.h"
#include "SessionManager.h"

CObjectManager::CObjectManager() noexcept
{
}

CObjectManager::~CObjectManager() noexcept
{
    for (auto& Object : m_ObjectList)
    {
        delete Object;
    }

    m_ObjectList.clear();
}

void CObjectManager::Update(void)
{
	for (auto& Object : m_ObjectList)
		Object->Update();
}

void CObjectManager::LateUpdate(void)
{
    auto it = m_ObjectList.begin();
    while (it != m_ObjectList.end())
    {
        // 오브젝트가 비활성화 되었다면
        if ((*it)->isDead())
        {
            NotifyClientDisconnected((*it)->m_pSession); // 세션이 죽었음을 알림
            
            delete (*it);                   // 플레이어 삭제
            it = m_ObjectList.erase(it);    // 리스트에서 iter 삭제
        }
        // 활성 중이라면
        else
        {
            (*it)->LateUpdate();
            ++it;
        }
    }
}

void CObjectManager::RegisterObject(CObject* pObject)
{
    m_ObjectList.push_back(pObject);
}
