#pragma once

#include <iostream>
#include <chrono>
#include <vector>
#include <new>
#include <Windows.h>

#include "Profile.h"

#define GUARD_VALUE 0xAAAABBBBCCCCDDDD

// Node ����ü ����

#ifdef _DEBUG
#pragma pack(1)
#endif // _DEBUG

template<typename T>
struct Node
{
    // x64 ȯ�濡�� ����ɰ��� ����ؼ� UINT64 ���� ���
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
#pragma pop
#endif // _DEBUG

// MemoryPool Ŭ���� ����
template<typename T, bool bPlacementNew>
class MemoryPool
{
public:
    // ������
    MemoryPool(UINT32 sizeInitialize = 0);

    // �Ҹ���
    virtual ~MemoryPool(void);
    
    // Ǯ�� �ִ� ��ü�� �Ѱ��ְų� ���� �Ҵ��� �ѱ�
    T* Alloc(void);
    
    // ��ü�� Ǯ�� ��ȯ
    bool Free(T* ptr);
   

private:
    Node<T>* m_freeNode;
    UINT32 m_countPool;
};

template<typename T, bool bPlacementNew>
inline MemoryPool<T, bPlacementNew>::MemoryPool(UINT32 sizeInitialize)
{
    m_freeNode = nullptr;
    m_countPool = 0;

    // �ʱ� �޸� ���� �غ�
    if (sizeInitialize > 0)
    {
        Node<T>* newNode;
        for (UINT32 i = 0; i < sizeInitialize; i++)
        {
            // ���ο� ��ü �Ҵ�
            newNode = (Node<T>*)malloc(sizeof(Node<T>));

#ifdef _DEBUG
            newNode->BUFFER_GUARD_FRONT = GUARD_VALUE;
            newNode->BUFFER_GUARD_END = GUARD_VALUE;

            newNode->POOL_INSTANCE_VALUE = reinterpret_cast<ULONG_PTR>(this);
#endif // _DEBUG

            // placement New �ɼ��� �����ִٸ� ������ ȣ��
            if constexpr (bPlacementNew)
            {
                //new (reinterpret_cast<char*>(newNode) + offsetof(Node<T>, data)) T();
                new (&(returnNode->data)) T();
            }

            // m_freeNode�� ���� ���� ���� -> ��ġ stack�� ����ϵ��� ���
            newNode->next = m_freeNode;
            m_freeNode = newNode;
        }

        // Ǯ���� �����ϴ� ������Ʈ ���� �ʱ�ȭ
        m_countPool = sizeInitialize;
    }
}

template<typename T, bool bPlacementNew>
inline MemoryPool<T, bPlacementNew>::~MemoryPool(void)
{
    // ��� ��� ����
    Node<T>* deleteNode = m_freeNode;
    Node<T>* nextNode;
    while (deleteNode != nullptr)
    {
        nextNode = deleteNode->next;
        
        // bPlacementNew �ɼ��� ���� �ִ��� ������ Alloc���� �����ڸ� ȣ�����ְ�� �޸� Ǯ �Ҹ��� ��  
        if constexpr (!bPlacementNew)
        {
            nextNode->data.~T();
        }

        delete deleteNode;
        deleteNode = nextNode;
        m_countPool--;
    }

    // ���� �Ҵ� ������ ���� �Ϸ���� �ʾҴٸ�
    if (m_countPool != 0)
    {
        // ��... ������� ���߿� �����ڱ�.
    }
}

template<typename T, bool bPlacementNew>
inline T* MemoryPool<T, bPlacementNew>::Alloc(void)
{
    Node<T>* returnNode;

    // m_freeNode�� nullptr�� �ƴ϶�� ���� Ǯ�� ��ü�� �����Ѵٴ� �ǹ��̹Ƿ� �ϳ� �̾Ƽ� �Ѱ���
    if (m_freeNode != nullptr)
    {
        returnNode = m_freeNode;            // top
        m_freeNode = m_freeNode->next;      // pop

        // placement New �ɼ��� �����ִٸ� ������ ȣ��
        if constexpr (bPlacementNew)
        {
            //new (reinterpret_cast<char*>(returnNode) + offsetof(Node<T>, data)) T();
            new (&(returnNode->data)) T();
        }

        // ��ü�� TŸ�� ������ ��ȯ
        return &returnNode->data;
    }

    // m_freeNode�� nullptr�̶�� Ǯ�� ��ü�� �������� �ʴ´ٴ� �ǹ��̹Ƿ� ���ο� ��ü �Ҵ�

    // �� ��� �Ҵ�
    Node<T>* newNode = (Node<T>*)malloc(sizeof(Node<T>));

#ifdef _DEBUG
    // ������ ����. ���� ���� Ȯ���ϰ�, ��ȯ�Ǵ� Ǯ�� ������ �ùٸ��� Ȯ���ϱ� ���� ���
    newNode->BUFFER_GUARD_FRONT = GUARD_VALUE;
    newNode->BUFFER_GUARD_END = GUARD_VALUE;

    newNode->POOL_INSTANCE_VALUE = reinterpret_cast<ULONG_PTR>(this);
#endif // _DEBUG

    newNode->next = nullptr;    // ��Ȯ�� m_freeNode�� �����ص� �ȴ�. �ٵ� �� ��ü�� nullptr�̴� ���� ���� nullptr ����.

    // placement New �ɼ��� �����ִٸ� ������ ȣ��
    if constexpr (bPlacementNew)
    {
        //new (reinterpret_cast<char*>(newNode) + offsetof(Node<T>, data)) T();
        new (&(returnNode->data)) T();
    }

    // Ǯ ������ 1 ����
    m_countPool++;

    // ��ü�� TŸ�� ������ ��ȯ
    return reinterpret_cast<T*>(reinterpret_cast<char*>(newNode) + offsetof(Node<T>, data));
}

template<typename T, bool bPlacementNew>
inline bool MemoryPool<T, bPlacementNew>::Free(T* ptr)
{
#ifdef _DEBUG
    // ��ȯ�ϴ� ���� �������� �ʴ´ٸ�
    if (ptr == nullptr)
    {
        // ����
        return false;
    }
#endif // _DEBUG

    // offsetof(Node<T>, data)) => Node ����ü�� data ������ Node ����ü���� �ּ� ���̰��� ���
    // ���⼱ debug ����϶� ���尡 �����Ƿ� 4, release�� ��� 0���� ó��
    Node<T>* pNode = reinterpret_cast<Node<T>*>(reinterpret_cast<char*>(ptr) - offsetof(Node<T>, data));

#ifdef _DEBUG 
    // ���� ����, ��� �÷ο� ����
    if (
        pNode->BUFFER_GUARD_FRONT != GUARD_VALUE ||
        pNode->BUFFER_GUARD_END != GUARD_VALUE
        )
    {
        // ���� �ٸ��ٸ� false ��ȯ
        return false;
    }

    //  Ǯ ��ȯ�� �ùٸ��� �˻�
    if (pNode->POOL_INSTANCE_VALUE != reinterpret_cast<ULONG_PTR>(this))
    {
        // �� �������� ��� ����... ���߿� ����.


        // ���� �ٸ��ٸ� false ��ȯ
        return false;
    }
#endif // _DEBUG

    // ���� placement new �ɼ��� ���� �ִٸ�, ������ placement new�� ���� ���̹Ƿ� �ش� ��ü�� �Ҹ��ڸ� �������� �ҷ���
    if constexpr (bPlacementNew)
    {
        pNode->data.~T();
    }

    // ���� ����Ʈ ���ÿ� ������ �ְ�
    pNode->next = m_freeNode;
    m_freeNode = pNode;

    // Ǯ ������ 1 ����
    m_countPool--;

    // ��ȯ ����
    return true;
}