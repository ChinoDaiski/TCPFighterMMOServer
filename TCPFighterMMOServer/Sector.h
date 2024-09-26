#pragma once

class CObject;
class CRingBuffer;

class CSector
{
public:
	// 인자로 전체 섹터에서 위치한 인덱스를 받음
	explicit CSector(UINT8 Width, UINT8 Height) noexcept;
	~CSector(void);

public:
	inline std::unordered_map<UINT32, CObject*>& GetSectorObjectMap(void) { return m_sectorObjectMap; }
	void RegisterObject(CObject* pObject);
	void DeleteObject(CObject* pObject);

public:
	const std::list<CSector*>& GetAroundSectorList(void) { return m_aroundSectorlist; }

public:
	void InsertAroundSector(CSector* pSector) { m_aroundSectorlist.push_back(pSector); }

private:
	// 섹터별로 들고 있는 오브젝트 정보
	std::unordered_map<UINT32, CObject*> m_sectorObjectMap;

private:
	// UINT8이기 때문에 인덱스 값을 0 ~ 255까지만 받을 수 있다. 추가시 오버플로우 주의
	UINT8 m_posX;
	UINT8 m_posY; 

private:
	std::list<CSector*> m_aroundSectorlist;	// 주위에 있는 섹터 정보
};

