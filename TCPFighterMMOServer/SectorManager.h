#pragma once

#include "Singleton.h"

class CSector;
class CObject;

using CreateSectorObjectCallback = void(*)(CObject*, CObject*);
using DeleteSectorObjectCallback = void(*)(CObject*, CObject*);

class CSectorManager : public SingletonBase<CSectorManager> {
private:
    friend class SingletonBase<CSectorManager>;

public:
    explicit CSectorManager() noexcept;
    ~CSectorManager() noexcept;

    // 복사 생성자와 대입 연산자를 삭제하여 복사 방지
    CSectorManager(const CSectorManager&) = delete;
    CSectorManager& operator=(const CSectorManager&) = delete;

public:
    CSector* GetSectorInfo(UINT8 posX, UINT8 poxY) { return m_Sectors[poxY][posX]; }

    // 위치 정보를 인자로 받아 현재 있는 섹터의 인덱스를 반환하는 함수
    static std::pair<UINT8, UINT8> GetSectorIndexFromWorldCoords(UINT16 prePosX, UINT16 prePosY);

    // 섹터를 이동할 때 추가되어야하는 섹터와 소멸되어야하는 섹터 계산를 계산
    void CalculateSectorChanges(CObject* pObject);

    // 소멸되어야하는 섹터들이 가지고 있는 오브젝트들에게 내 오브젝트 정보 삭제 패킷을 보내고, 내 오브젝트에게 해당 섹터들이 가지고 있는 오브젝트들 삭제 패킷을 주입
    // 생성되어야하는 섹터들이 가지고 있는 오브젝트들에게 내 오브젝트 정보 생성 패킷을 보내고, 내 오브젝트에게 해당 섹터들이 가지고 있는 오브젝트들 생성 패킷을 주입
    // 생성 패킷을 보낼 때 내 오브젝트의 상태도 함께 보낸다. 마찬가지로 생성이 되어야하는 오브젝트들 정보를 받을 때 해당 오브젝트들의 상태로 주입 받음.
    void ProcessSectorObjectPackets(CObject* pObject);

public:
    void RegisterObjectToSector(CObject* pObject);
    void DeleteObjectFromSector(CObject* pObject);

public:
    static void RegisterCreateSectorObjectCallback(CreateSectorObjectCallback callback) { m_callbackCreateSector = callback; }
    static void RegisterDeleteSectorObjectCallback(DeleteSectorObjectCallback callback) { m_callbackDeleteSector = callback; }

private:
    // 섹터 정보
    CSector* m_Sectors[dfSECTOR_HEIGHT_CNT][dfSECTOR_WIDTH_CNT];

    // 이동 연산시 삭제되거나 추가되어야할 섹터의 인덱스 정보
    std::vector<POINT> m_sectorsToDelete;
    std::vector<POINT> m_sectorsToAdd;

    static UINT16 sectorWidth;
    static UINT16 sectorHeight;

    static CreateSectorObjectCallback m_callbackCreateSector;
    static DeleteSectorObjectCallback m_callbackDeleteSector;
};


