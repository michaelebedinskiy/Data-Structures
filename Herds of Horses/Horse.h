
#ifndef DS_HW1_HORSE_H
#define DS_HW1_HORSE_H
#include <memory>


class Horse {
public:
    class HorseLink {
    private:
        Horse* originHorsePtr;
        int id;
        int herdId;
        unsigned int publicKey;

    public:
        HorseLink() = delete;
        HorseLink(const std::shared_ptr<Horse>& origin) :
                originHorsePtr(origin.get()),
                id(origin->getId()),
                herdId(origin->getHerdId()),
                publicKey(origin->getKeyForSlaves()) {

        }
        ~HorseLink() = default;

        int getId() const {
            return this->id;
        }
        int getHerdId() const {
            return this->herdId;
        }
        Horse* getOrigin() const {
            return this->originHorsePtr;
        }
        unsigned int getPublicKey() const {
            return this->publicKey;
        }
        void setPublicKey(const unsigned int originKey) {
            this->publicKey = originKey;
        }
        void setNewHerdId(const int newHerdId) {
            this->herdId = newHerdId;
        }
    };

private:
    const int id;
    const int speed;
    int herdId = 0;
    //The key to hitting all runtime constrains:
    //I couldn't think of any better solution using any structure.
    //we get either search time or delete time constrains if we use/don't use structures. cuz cleanup take time.
    //I think lazy evaluations will be the best solution here, although it requires more logic in functions.
    HorseLink* shadowPtr = nullptr;
    unsigned int keyForSlaves = 0;
    HorseLink* masterLinkPtr = nullptr;
    unsigned int keyToMaster = 0;
    //This part is dedicated for can_run_together()
    //Assign a few more members to keep track of search mission:
    int sumOfSlaves = 0;
    int flagFoundIter = 0;

public:
    Horse(const int id, const int speed) : id(id), speed(speed) {};
    Horse(const Horse& oldHorse) = default;
    ~Horse() {
        this->makeNewKeyForSlaves();
    };
    int getId() const {
        return this->id;
    }
    int getSpeed() const {
        return this->speed;
    }
    int getHerdId() const {
        return this->herdId;
    }
    void setHerdId(const int newHerdId) {
        this->herdId = newHerdId;
    }
    void setShadowPtr(HorseLink* newShadowPtr) {
        this->shadowPtr = newShadowPtr;
    }

    //Slave-Master relation
    HorseLink* getShadowPtr() const {
        return this->shadowPtr;
    }
    Horse* getMasterIfConnectionIsGood() const {
        if(this->masterLinkPtr == nullptr) {
            return nullptr;
        }
        unsigned int correctKeyInLink = this->masterLinkPtr->getPublicKey();
        if(correctKeyInLink != this->keyToMaster) {
            return nullptr;
        }
        //more sanity checks in case the course tests want to see us fall:
        if(this->masterLinkPtr->getHerdId() != this->herdId) {
            return nullptr;
        }
        return this->masterLinkPtr->getOrigin();
    }
    //For Slaves
    unsigned int getKeyForSlaves() const {
        return this->keyForSlaves;
    }
    void makeNewKeyForSlaves() {
        this->keyForSlaves = this->keyForSlaves +1;
    }
    void setIsMaster() {
        //only if the horse didn't lead till now we give him a new key.
        if (this->keyForSlaves == 0) {
            this->makeNewKeyForSlaves();
        }
    };
    //To Master
    void resetKeyToMaster() {
        this->keyToMaster = 0;
    }
    void setNewMaster(const Horse* masterHorse) {
        this->masterLinkPtr = masterHorse->getShadowPtr();
        this->keyToMaster = masterHorse->getKeyForSlaves();
    }

    //Search mission:
    int getSumOfSlaves() const {
        return this-> sumOfSlaves;
    }
    int getFoundFlagIter() const {
        return this->flagFoundIter;
    }
    void searchMissionResetHorse() {
        this->sumOfSlaves = 0;
        this->flagFoundIter = 0;
    }
    void searchMissionEvaluateHorse() {
        this->sumOfSlaves = this->sumOfSlaves +1;
    }
    void searchMissionFlagIter(const int iter) {
        this->flagFoundIter = iter;
    }
};

struct HorseSharedPtrComparator {
    bool operator()(const std::shared_ptr<Horse>& a, const std::shared_ptr<Horse>& b) const {
        return a->getId() < b->getId();
    }
};
struct HorseSharedLinkPtrComparator {
    bool operator()(const std::shared_ptr<Horse::HorseLink>& a, const std::shared_ptr<Horse::HorseLink>& b) const {
        return a->getId() < b->getId();
    }
};
struct HorsePtrComparator {
    bool operator()(const Horse* a, const Horse* b) const {
        return a->getId() < b->getId();
    }
};

#endif //DS_HW1_HORSE_H



