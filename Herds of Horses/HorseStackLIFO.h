//
// Created by eitan on 12/15/24.
//

#ifndef QUEUEOFHORSES_H
#define QUEUEOFHORSES_H
#include <memory>
#include "Horse.h"

class HorseStackLIFO {
private:
    struct Node {
        Horse* horsePtr;
        Node* next;

        Node(Horse* horsePtr, Node* next) : horsePtr(horsePtr), next(next) {}
    };

    Node* head;
    int size;


public:
    HorseStackLIFO() : head(nullptr), size(0) {}
    HorseStackLIFO(const HorseStackLIFO& other) = delete;
    ~HorseStackLIFO() {
        while(size>0) {
            this->pop();
        }
    }

    void push(Horse* horsePtr) {
        Node* newNode = new Node(horsePtr, this->head);
        this->head = newNode;
        this->size = this->size +1;
    }
    Horse* pop() {
        if(this->size == 0) {
            return nullptr;
        }
        const Node* tempNode = head;
        Horse* horsePtr = this->head->horsePtr;
        this->head = this->head->next;
        delete tempNode;
        this->size = this->size -1;
        return horsePtr;
    }
    int getSize() const {
        return this->size;
    }
};



#endif //QUEUEOFHORSES_H
