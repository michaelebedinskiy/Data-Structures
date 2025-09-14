
#ifndef DS_HW1_HERD_H
#define DS_HW1_HERD_H
#include <csignal>

#include "AVLtree.h"
#include "Horse.h"
#include "HorseStackLIFO.h"

class Herd {
    int herdId;
    int size = 0;
    std::shared_ptr<AVLTree<Horse*, HorsePtrComparator>> horseTree;

public:
    Herd() = delete;
    Herd(int id) : herdId(id), horseTree(std::make_shared<AVLTree<Horse*, HorsePtrComparator>>()) {}
    Herd(const Herd& herd) : herdId(herd.herdId), size(herd.size), horseTree(herd.horseTree) {};
    ~Herd() = default;

    int getId() const {
        return herdId;
    }
    int getSize() const {
        return this->size;
    }
    void insertHorse(Horse* horseToInsert) {
        this->horseTree->insert(horseToInsert);
        updateSizePlus();
    }
    void removeHorse(Horse* horseToRemove) {
        this->horseTree->remove(horseToRemove);
        updateSizeMinus();
    }
    void updateSizePlus() {
        this->size = this->size +1;
    }
    void updateSizeMinus() {
        this->size = this->size -1;
    }

    //search mission support to find if can_run_together.
    //Step1. is Reset
    //Step2. is Evaluate
    //Step3. is Gather Masters and Put Slaveless Horses in QUEUE
    //Step4. no need helper functions.
    //Step5. is Reset again.
    void searchMissionStep(int action, HorseStackLIFO* stackBots = nullptr, HorseStackLIFO* stackTops = nullptr) const{
        if (this->horseTree->getRoot() != nullptr) {
            if(action == 1 || action ==5) {
                auxSearchMissionReset(this->horseTree->getRoot());
            }
            if(action == 2) {
                auxSearchMissionEvaluate(this->horseTree->getRoot());
            }
            if(action == 3) {
                auxSearchMissionGather(this->horseTree->getRoot(), stackBots, stackTops);
            }
        }
    }
    void auxSearchMissionReset(const AVLTree<Horse*, HorsePtrComparator>::Node* currentNode) const {
        currentNode->data->searchMissionResetHorse();
        if(currentNode->left != nullptr) {
            auxSearchMissionReset(currentNode->left);
        }
        if(currentNode->right != nullptr) {
            auxSearchMissionReset(currentNode->right);
        }
    }
    void auxSearchMissionEvaluate(const AVLTree<Horse*, HorsePtrComparator>::Node* currentNode) const {
        if(currentNode->data->getMasterIfConnectionIsGood()) {
            currentNode->data->getMasterIfConnectionIsGood()->searchMissionEvaluateHorse();
        }
        if(currentNode->left != nullptr) {
            auxSearchMissionEvaluate(currentNode->left);
        }
        if(currentNode->right != nullptr) {
            auxSearchMissionEvaluate(currentNode->right);
        }
    }
    void auxSearchMissionGather(const AVLTree<Horse*, HorsePtrComparator>::Node* currentNode,
                                HorseStackLIFO* stackBots,
                                HorseStackLIFO* stackTops) const {
        //action on current node.
        if (currentNode->data->getMasterIfConnectionIsGood() == nullptr) {
            stackTops->push(currentNode->data);
        } else if(currentNode->data->getSumOfSlaves() == 0) {
            stackBots->push(currentNode->data);
        }
        //action on next nodes.
        if(currentNode->left != nullptr) {
            auxSearchMissionGather(currentNode->left, stackBots, stackTops);
        }
        if(currentNode->right != nullptr) {
            auxSearchMissionGather(currentNode->right, stackBots, stackTops);
        }
    }
};

struct HerdPtrComparator {
    bool operator()(const Herd* a, const Herd* b) const {
        return a->getId() < b->getId();
    }
};
struct HerdComparator {
    bool operator()(const Herd& a, const Herd& b) const {
        return a.getId() < b.getId();
    }
};
struct HerdSharedPtrComparator {
    bool operator()(const std::shared_ptr<Herd>& a, const std::shared_ptr<Herd>& b) const {
        return a->getId() < b->getId();
    }
};



#endif //DS_HW1_HERD_H
