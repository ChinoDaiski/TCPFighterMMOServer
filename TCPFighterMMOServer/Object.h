#pragma once

class CSession;

// �⺻ CObject Ŭ���� ����
class CObject {
public:
    explicit CObject(UINT16 _x = 0, UINT16 _y = 0) noexcept;
    virtual ~CObject() = default;

    virtual void Move();

    // ������Ʈ �޼��� (���� �������� ������Ʈ ���¸� �����ϴ� �� ���)
    virtual void Update(void);
    virtual void LateUpdate(void);


public:
    // ��ġ ���� �� ��������
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

public:
    CSession* m_pSession;
    UINT32 m_ID; // ID

protected:
    UINT16 m_x, m_y;
    bool m_bDead;

private:
    static UINT32 g_ID; // ID
};