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
        // ������Ʈ�� ��Ȱ��ȭ �Ǿ��ٸ�
        if ((*it)->isDead())
        {
            NotifyClientDisconnected((*it)->m_pSession); // ������ �׾����� �˸�
            
            delete (*it);                   // �÷��̾� ����
            it = m_ObjectList.erase(it);    // ����Ʈ���� iter ����
        }
        // Ȱ�� ���̶��
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
