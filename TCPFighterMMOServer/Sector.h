#pragma once

class CObject;
class CRingBuffer;

class CSector
{
public:
	// ���ڷ� ��ü ���Ϳ��� ��ġ�� �ε����� ����
	explicit CSector(UINT16 Width, UINT16 Height) noexcept;
	~CSector(void);

public:
	inline std::unordered_map<UINT32, CObject*>& GetSectorObjectMap(void) { return m_sectorObjectMap; }
	void RegisterObject(CObject* pObject);
	void DeleteObject(CObject* pObject);

private:
	// ���ͺ��� ��� �ִ� ������Ʈ ����
	std::unordered_map<UINT32, CObject*> m_sectorObjectMap;

private:
	// UINT8�̱� ������ �ε��� ���� 0 ~ 255������ ���� �� �ִ�. �߰��� �����÷ο� ����
	UINT8 m_posX;
	UINT8 m_posY;
};

