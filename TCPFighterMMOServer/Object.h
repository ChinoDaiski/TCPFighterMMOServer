#pragma once

class CSession;

// 기본 CObject 클래스 정의
class CObject {
public:
    explicit CObject(UINT16 _x = 0, UINT16 _y = 0) noexcept;
    virtual ~CObject() = default;

    virtual void Move();

    // 업데이트 메서드 (서버 로직에서 오브젝트 상태를 갱신하는 데 사용)
    virtual void Update(void);
    virtual void LateUpdate(void);


public:
    // 위치 설정 및 가져오기
    constexpr void SetPosition(UINT16 _x, UINT16 _y) {
        m_x = _x;
        m_y = _y;
    }

    constexpr void getPosition(UINT16& _x, UINT16& _y) const {
        _x = m_x;
        _y = m_y;
    }

public:
    inline bool isDead(void) { return m_bDead; }
    
private:
    void CheckTimeout(void);    // 추기적으로 Timeout을 확인하기 위해 불러주는 함수
    void SetCurTimeout(void);   // Timeout 값을 설정하는 함수. 패킷 처리할 때 마다 호출해서 Timeout 갱신
    friend bool PacketProc(CSession* pSession, PACKET_TYPE packetType, CPacket* pPacket);

public:
    CSession* m_pSession;
    UINT32 m_ID; // ID
    POINT m_preSectorPos;
    POINT m_curSectorPos;

protected:
    UINT16 m_x, m_y;
    bool m_bDead;

private:
    // Timeout 관련
    UINT32 m_lastTimeoutCheckTime; // timegettime 기준 값

private:
    static UINT32 g_ID; // ID
};