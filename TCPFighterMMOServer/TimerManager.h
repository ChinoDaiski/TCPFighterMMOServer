#pragma once

#include "Singleton.h"

class CTimerManager : public SingletonBase<CTimerManager>
{
public:
	explicit CTimerManager() noexcept;
	~CTimerManager() noexcept;

	// ���� �����ڿ� ���� �����ڸ� �����Ͽ� ���� ����
	CTimerManager(const CTimerManager&) = delete;
	CTimerManager& operator=(const CTimerManager&) = delete;

public:
	// ��ǥ ������. �ظ��ϸ� 1000�� ����� ������ ��.
	void InitTimer(DWORD _targetFPS);
	bool CheckFrame(void);

private:
	DWORD m_targetFPS;				// 1�ʴ� ��ǥ ������
	DWORD m_givenFrameTime;			// 1�ʴ� �־����� �ð� -> 1000 / m_targetFPS
	DWORD m_standardServerTime;		// ���� ������ ���۵� �� �ʱ�ȭ�Ǵ� ��
	DWORD m_currentServerTime;		// ���� ������ ���۵� �� �ʱ�ȭ�ǰ�, ���Ŀ� �������� ���� �� ���� m_givenFrameTime ��ŭ ����.

	UINT16 m_FPS;	// ���� ������
};


