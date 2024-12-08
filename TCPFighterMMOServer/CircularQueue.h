#pragma once

#define CIRCULAR_QUEUE_SIZE 10000


template <typename T>
class CircularQueue {
public:
    // 생성자
    CircularQueue(int size = CIRCULAR_QUEUE_SIZE) : front(0), rear(-1), capacity(size), count(0) {
        //queue = new T[capacity];  // 동적 배열 할당
    }

    // 소멸자
    ~CircularQueue() {
        //delete[] queue;  // 동적 배열 해제
    }

    // 큐가 비었는지 확인
    bool isEmpty() const {
        return count == 0;
    }

    // 큐가 꽉 찼는지 확인
    bool isFull() const {
        return count == capacity;
    }

    // 큐에 데이터 추가 (enqueue)
    void enqueue(const T& data) {
        /*if (isFull()) {
            dequeue();
        }*/
        rear = (rear + 1) % capacity;  // 원형 배열 처리
        queue[rear] = data;
        count++;
    }

    // 큐에서 데이터 제거 (dequeue)
    void dequeue() {
        /*if (isEmpty()) {
            return;
        }*/
        front = (front + 1) % capacity;  // 원형 배열 처리
        count--;
    }

    // 큐의 앞에 있는 데이터를 반환 (peek)
    T peek() const {
        if (isEmpty()) {
            return T();  // 기본 생성된 T 반환
        }
        return queue[front];
    }

    // 큐에 저장된 데이터의 개수 반환
    int size() const {
        return count;
    }

public:
    T queue[CIRCULAR_QUEUE_SIZE];         // 큐 배열
    int front, rear;  // 큐의 앞과 뒤 인덱스
    int capacity;     // 큐의 최대 크기
    int count;        // 현재 큐에 저장된 원소의 개수
};