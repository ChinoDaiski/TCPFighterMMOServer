#pragma once

#include "Singleton.h"
#include "Object.h"

class CSession;
class CPlayer;

class CObjectManager : public SingletonBase<CObjectManager> {
private:
    friend class SingletonBase<CObjectManager>;

public:
    explicit CObjectManager() noexcept;
    ~CObjectManager() noexcept;
    
    // 복사 생성자와 대입 연산자를 삭제하여 복사 방지
    CObjectManager(const CObjectManager&) = delete;
    CObjectManager& operator=(const CObjectManager&) = delete;

public:
    void Update(void);
    void LateUpdate(void);

public:
    void RegisterObject(CObject* pObject);
    void DeleteObject(CObject* pObject);

public:
    inline std::unordered_map<UINT32, CObject*>& GetObjectMap(void) { return m_ObjectHashMap; }

private:
    std::unordered_map<UINT32, CObject*> m_ObjectHashMap;
};