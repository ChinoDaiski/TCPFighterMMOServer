#pragma once

#include <iostream>
#include <chrono>
#include <vector>
#include <new>
#include <Windows.h>

#define GUARD_VALUE 0xaaaabbbbccccdddd

// Node 구조체 정의

#ifdef _DEBUG
#pragma pack(1)
#endif // _DEBUG

template<typename T>
struct Node
{
    // x64 환경에서 빌드될것을 고려해서 UINT64 형을 사용
#ifdef _DEBUG

    UINT64 BUFFER_GUARD_FRONT;
    T data;
    UINT64 BUFFER_GUARD_END;
    ULONG_PTR POOL_INSTANCE_VALUE;
    Node* next;

#endif // _DEBUG
#ifndef _DEBUG

    T data;
    Node* next;

#endif // !_DEBUG
};

#ifdef _DEBUG
#endif // _DEBUG

// MemoryPool 클래스 정의
template<typename T, bool bPlacementNew>
class MemoryPool
{
public:
    // 생성자
    MemoryPool(UINT32 sizeInitialize = 0);

    // 소멸자
    virtual ~MemoryPool(void);

    // 풀에 있는 객체를 넘겨주거나 새로 할당해 넘김
    T* Alloc(void);

    // 객체를 풀에 반환
    bool Free(T* ptr);

    // 메모리 풀의 사용량 확인
    void PrintUsage() const;

private:
    Node<T>* m_freeNode;
    UINT32 m_countPool;         // 풀 전체에 할당된 객체 수
    UINT32 m_allocatedCount;    // 현재 풀에서 할당된 객체 수, 0개라면 풀에서 사용되고 있는 객체가 없는 것이다.
};

template<typename T, bool bPlacementNew>
inline MemoryPool<T, bPlacementNew>::MemoryPool(UINT32 sizeInitialize)
{
    m_freeNode = nullptr;
    m_countPool = 0;
    m_allocatedCount = 0;

    // 초기 메모리 공간 준비
    if (sizeInitialize > 0)
    {
        Node<T>* newNode;

        // 새로운 객체 할당
        newNode = (Node<T>*)malloc(sizeof(Node<T>));

#ifdef _DEBUG
        newNode->BUFFER_GUARD_FRONT = GUARD_VALUE;
        newNode->BUFFER_GUARD_END = GUARD_VALUE;

        newNode->POOL_INSTANCE_VALUE = reinterpret_cast<ULONG_PTR>(this);
#endif // _DEBUG

        // placement New 옵션 여부와 상관없이 무조건 생성자 호출
        new (&(newNode->data)) T();     //new (reinterpret_cast<char*>(newNode) + offsetof(Node<T>, data)) T();

        // 처음엔 m_freeNode가 비어있으므로 newNode에 대입
        m_freeNode = newNode;


        for (UINT32 i = 1; i < sizeInitialize; i++)
        {
            // 새로운 객체 할당
            newNode = (Node<T>*)malloc(sizeof(Node<T>));

#ifdef _DEBUG
            newNode->BUFFER_GUARD_FRONT = GUARD_VALUE;
            newNode->BUFFER_GUARD_END = GUARD_VALUE;

            newNode->POOL_INSTANCE_VALUE = reinterpret_cast<ULONG_PTR>(this);
#endif // _DEBUG

            // placement New 옵션 여부와 상관없이 무조건 생성자 호출
            new (&(newNode->data)) T();     //new (reinterpret_cast<char*>(newNode) + offsetof(Node<T>, data)) T();

            // m_freeNode를 다음 노드로 설정 -> 마치 stack을 사용하듯이 사용
            newNode->next = m_freeNode;
            m_freeNode = newNode;
        }

        // 풀에서 관리하는 오브젝트 갯수 초기화
        m_countPool = sizeInitialize;
    }
}

template<typename T, bool bPlacementNew>
inline MemoryPool<T, bPlacementNew>::~MemoryPool(void)
{
    // 모든 노드 해제
    Node<T>* deleteNode = m_freeNode;
    Node<T>* nextNode;
    while (deleteNode != nullptr)
    {
        nextNode = deleteNode->next;

        // bPlacementNew 옵션이 켜져 있던지 말던지 Alloc에서 생성자를 호출해준다.
        // 그러니 메모리 풀 소멸시 bPlacementNew 옵션이 꺼져있다면 소멸자를 호출해줘야한다. 
        // bPlacementNew 옵션을 사용하지 않을 시 생성자가 호출된다면 동적할당을 내부에서 할 수 있기 때문에 소멸자를 호출해야 동적할당한 것을 해제할 수 있기 떄문이다.
        if constexpr (!bPlacementNew)
        {
            deleteNode->data.~T();
        }

        delete deleteNode;
        deleteNode = nextNode;
        m_countPool--;
    }

    // 만약 할당 해제가 전부 완료되지 않았다면
    if (m_countPool != 0)
    {
        // 음... 어떻게할지 나중에 정하자구.
    }
}

template<typename T, bool bPlacementNew>
inline T* MemoryPool<T, bPlacementNew>::Alloc(void)
{
    Node<T>* returnNode;

    // m_freeNode가 nullptr이 아니라는 것은 풀에 객체가 존재한다는 의미이므로 하나 뽑아서 넘겨줌
    if (m_freeNode != nullptr)
    {
        returnNode = m_freeNode;            // top
        m_freeNode = m_freeNode->next;      // pop

        // placement New 옵션이 켜져있다면 생성자 호출
        if constexpr (bPlacementNew)
        {
            //new (reinterpret_cast<char*>(returnNode) + offsetof(Node<T>, data)) T();
            new (&(returnNode->data)) T();
        }

        // 할당된 객체 수 증가
        m_allocatedCount++;

        // 객체의 T타입 데이터 반환
        return &returnNode->data;
    }

    // m_freeNode가 nullptr이라면 풀에 객체가 존재하지 않는다는 의미이므로 새로운 객체 할당

    // 새 노드 할당
    Node<T>* newNode = (Node<T>*)malloc(sizeof(Node<T>));

#ifdef _DEBUG
    // 디버깅용 가드. 가드 값도 확인하고, 반환되는 풀의 정보가 올바른지 확인하기 위해 사용
    newNode->BUFFER_GUARD_FRONT = GUARD_VALUE;
    newNode->BUFFER_GUARD_END = GUARD_VALUE;

    newNode->POOL_INSTANCE_VALUE = reinterpret_cast<ULONG_PTR>(this);
#endif // _DEBUG

    newNode->next = nullptr;    // 정확힌 m_freeNode를 대입해도 된다. 근데 이 자체가 nullptr이니 보기 쉽게 nullptr 넣음.

    // bPlacementNew 옵션과 별개로 무조건 생성자를 호출해줌.
    new (&(newNode->data)) T();

    // 풀 갯수를 1 증가
    m_countPool++;

    // 할당된 객체 수 증가
    m_allocatedCount++;

    // 객체의 T타입 데이터 반환
    return reinterpret_cast<T*>(reinterpret_cast<char*>(newNode) + offsetof(Node<T>, data));
}

template<typename T, bool bPlacementNew>
inline bool MemoryPool<T, bPlacementNew>::Free(T* ptr)
{
#ifdef _DEBUG
    // 반환하는 값이 존재하지 않는다면
    if (ptr == nullptr)
    {
        // 실패
        return false;
    }
#endif // _DEBUG

    // offsetof(Node<T>, data)) => Node 구조체의 data 변수와 Node 구조체와의 주소 차이값을 계산
    // 여기선 debug 모드일때 가드가 있으므로 4, release일 경우 0으로 처리
    Node<T>* pNode = reinterpret_cast<Node<T>*>(reinterpret_cast<char*>(ptr) - offsetof(Node<T>, data));

#ifdef _DEBUG 
    // 스택 오버, 언더 플로우 감지
    if (
        //(pNode->BUFFER_GUARD_FRONT != GUARD_VALUE) ||
        //(pNode->BUFFER_GUARD_END != GUARD_VALUE)
        pNode->BUFFER_GUARD_FRONT != pNode->BUFFER_GUARD_END
        )
    {
        // 만약 다르다면 false 반환
        return false;
    }

    //  풀 반환이 올바른지 검사
    if (pNode->POOL_INSTANCE_VALUE != reinterpret_cast<ULONG_PTR>(this))
    {
        // 이 시점에서 어떻게 할지... 나중에 결정.


        // 만약 다르다면 false 반환
        return false;
    }
#endif // _DEBUG

    // 만약 placement new 옵션이 켜져 있다면, 생성을 placement new로 했을 것이므로 해당 객체의 소멸자를 수동으로 불러줌
    // 꺼져 있다면 메모리풀의 소멸자에서 일괄로 처리해줄 예정
    if constexpr (bPlacementNew)
    {
        pNode->data.~T();
    }

    // 프리 리스트 스택에 정보를 넣고
    pNode->next = m_freeNode;
    m_freeNode = pNode;

    // 사용되어 지고 있는 객체 수 감소
    m_allocatedCount--;

    // 반환 성공
    return true;
}

template<typename T, bool bPlacementNew>
inline void MemoryPool<T, bPlacementNew>::PrintUsage() const
{
    std::cout << "메모리 풀 타입 : " << typeid(T).name() << ":\n";
    std::cout << "할당된 총 오브젝트 갯수 : " << m_countPool << "\n";
    std::cout << "현재 사용중인 오브젝트 갯수 : " << m_allocatedCount << "\n\n";
}