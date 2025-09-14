// You can edit anything you want in this file.
// However you need to implement all public Plains function, as provided below as a template

#include "plains25a1.h"

#include <memory>

using namespace std;
void auxCleanFollowFlags(Horse* currentHorse) {
    currentHorse->searchMissionResetHorse();
    Horse* masterHorse = currentHorse->getMasterIfConnectionIsGood();
    if (masterHorse) {
        if (masterHorse->getFoundFlagIter() == 0 && masterHorse->getSumOfSlaves() == 0) {
            return;
        }
        auxCleanFollowFlags(masterHorse);
    }
}


Plains::Plains() :
    allHorses(new AVLTree<shared_ptr<Horse>, HorseSharedPtrComparator>()),
    allLinks(new AVLTree<shared_ptr<Horse::HorseLink>, HorseSharedLinkPtrComparator>()),
    fullHerds(new AVLTree<shared_ptr<Herd>, HerdSharedPtrComparator>()),
    emptyHerds(new AVLTree<shared_ptr<Herd>, HerdSharedPtrComparator>()) {

}
Plains::~Plains()
{
    //delete horses last is important.
    //deleting is done with Theta[(sizeof(Tree)]
    //Thus we get: Theta[n+m+m0]
    delete fullHerds;
    delete emptyHerds;
    delete allHorses;
    delete allLinks;
}
StatusType Plains::add_herd(int herdId)
{
    if (herdId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    try {
        const shared_ptr<Herd> newHerdPtr = make_shared<Herd>(herdId);
        if (fullHerds->find(newHerdPtr) != nullptr || emptyHerds->find(newHerdPtr) != nullptr) {
            return StatusType::FAILURE;
        }

        //insert the new Herd into emptyHerds
        emptyHerds->insert(newHerdPtr);
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    } catch (...) {
        return StatusType::FAILURE;
    }
    return StatusType::SUCCESS;
}
StatusType Plains::remove_herd(int herdId)
{
    if (herdId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    try {
        const shared_ptr<Herd> newHerdPtr = make_shared<Herd>(herdId);
        //check if the herd exists and is EMPTY - here we get Theta[log(m0)]
        const AVLTree<shared_ptr<Herd>, HerdSharedPtrComparator>::Node *foundOrNot = emptyHerds->find(newHerdPtr);
        if (foundOrNot == nullptr || foundOrNot->data->getSize() != 0) {
            return StatusType::FAILURE;
        }
        //remove the herd - here we get Theta[log(m0)]
        emptyHerds->remove(newHerdPtr); //removes the herdNode from the AVLTree

    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    } catch (...) {
        return StatusType::FAILURE;
    }
    return StatusType::SUCCESS;
}
StatusType Plains::add_horse(int horseId, int speed)
{
    if (horseId <= 0 || speed <= 0) {
        return StatusType::INVALID_INPUT;
    }

    shared_ptr<Horse> newHorsePtr = nullptr;
    try {
        newHorsePtr = make_shared<Horse>(horseId, speed);
        //check if the herd already exists - here we get Theta[log(n)]
        if(allHorses->find(newHorsePtr) != nullptr) {
            return StatusType::FAILURE;
        }
        //insert the new Herd into the tree - here we get Theta[log(n)]
        allHorses->insert(newHorsePtr);
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    } catch (...) {
        return StatusType::FAILURE;
    }

    //now create the HorseLink
    shared_ptr<Horse::HorseLink> newHorseLink = nullptr;
    try {
        newHorseLink = make_shared<Horse::HorseLink>(newHorsePtr);
        this->allLinks->insert(newHorseLink); //also inserts with Theta[log(n)]

        //make the link
        newHorsePtr->setShadowPtr(newHorseLink.get());

    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    } catch (...) {
        return StatusType::FAILURE;
    }
    return StatusType::SUCCESS;
}
StatusType Plains::join_herd(int horseId, int herdId) {
    if (horseId <= 0 || herdId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    try {
        const shared_ptr<Horse> tempHorse = make_shared<Horse>(horseId, 1);
        const shared_ptr<Herd> tempHerdPtr = make_shared<Herd>(herdId);
        //try to find if either don't exist - here we get Theta[log(n)+log(m)+log(m0)]
        const AVLTree<shared_ptr<Horse>, HorseSharedPtrComparator>::Node *horseNodePtr = allHorses->find(tempHorse); //Theta[log(n)]
        const AVLTree<shared_ptr<Herd>, HerdSharedPtrComparator>::Node *herdNodePtr1 = fullHerds->find(tempHerdPtr); //Theta[log(m)]
        const AVLTree<shared_ptr<Herd>, HerdSharedPtrComparator>::Node *herdNodePtr2 = emptyHerds->find(tempHerdPtr); //Theta[log(m0)]
        if (horseNodePtr == nullptr || (herdNodePtr1 == nullptr && herdNodePtr2 == nullptr)) {
            return StatusType::FAILURE;
        }

        //now we can work on real data - careful to deref NULLPTR
        const shared_ptr<Herd> herdPtr = herdNodePtr1 != nullptr ? herdNodePtr1->data : herdNodePtr2->data;
        Horse* horsePtr = horseNodePtr->data.get();

        //now we check if horse is legal to move
        if (horsePtr->getHerdId() != 0) {
            return StatusType::FAILURE;
        }

        //if here legal conditions are met - set the horse to be in a herd
        horsePtr->setHerdId(herdPtr->getId());
        herdPtr->insertHorse(horsePtr); //just a pointer! not shared_ptr. Theta[log(m~m0)]

        //here important SSH level encryption magic happens:
        horsePtr->resetKeyToMaster(); //disconnects the horse from its leader.
        horsePtr->makeNewKeyForSlaves(); //disconnects all horse's followers.
        horsePtr->getShadowPtr()->setPublicKey(horsePtr->getKeyForSlaves()); //link is ready for new connections
        horsePtr->getShadowPtr()->setNewHerdId(herdPtr->getId());

        //check for need to move herd [from m0, to m]
        if(herdPtr->getSize() == 1) {
            this->emptyHerds->remove(herdPtr); //Theta[log(m0)]
            this->fullHerds->insert(herdPtr); //Theta[log(m)]
        }

    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }catch (...) {
        return StatusType::FAILURE;
    }
    return StatusType::SUCCESS;
}
StatusType Plains::follow(int horseId, int horseToFollowId)
{
    if (horseId <= 0 || horseToFollowId <= 0 || horseId == horseToFollowId) {
        return StatusType::INVALID_INPUT;
    }

    try {
        const shared_ptr<Horse> tempSlaveHorse = make_shared<Horse>(horseId, 1);
        const shared_ptr<Horse> tempMasterHorse= make_shared<Horse>(horseToFollowId, 1);
        //check for the existence of both horses - here we get 2*Theta[log(n)]
        const AVLTree<shared_ptr<Horse>, HorseSharedPtrComparator>::Node* slaveHorseNodePtr = allHorses->find(tempSlaveHorse);
        const AVLTree<shared_ptr<Horse>, HorseSharedPtrComparator>::Node* masterHorseNodePtr = allHorses->find(tempMasterHorse);
        if(slaveHorseNodePtr == nullptr || masterHorseNodePtr == nullptr) {
            return StatusType::FAILURE;
        }
        //here we know both exist - check legality conditions
        if(slaveHorseNodePtr->data->getHerdId() != masterHorseNodePtr->data->getHerdId()) {
            return StatusType::FAILURE;
        }
        if (slaveHorseNodePtr->data->getHerdId() == 0) {
            //here we know both horses have the same herdId so checking one is enough
            return StatusType::FAILURE;
        }
        Horse* slaveHorsePtr = slaveHorseNodePtr->data.get();
        Horse* masterHorsePtr = masterHorseNodePtr->data.get();

        //establish new SSH connection between horses.
        //prepare horses for SSH connection
        slaveHorsePtr->resetKeyToMaster();
        masterHorsePtr->setIsMaster();
        //connect - sets to hold the MasterLinkPtr & key
        slaveHorsePtr->setNewMaster(masterHorsePtr);
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    } catch (...) {
        return StatusType::FAILURE;
    }
    return StatusType::SUCCESS;
}
StatusType Plains::leave_herd(int horseId)
{
    if (horseId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    try {
        const shared_ptr<Horse> tempHorse = make_shared<Horse>(horseId, 1);
        //here we find the horse with Theta[log(n)]
        const AVLTree<shared_ptr<Horse>, HorseSharedPtrComparator>::Node *horseNodePtr = allHorses->find(tempHorse); //Theta[log(n)]
        if(horseNodePtr == nullptr) {
            return StatusType::FAILURE;
        }
        //now we sure it exists in a herd - careful not to deref nullptr
        Horse* horse = horseNodePtr->data.get();
        const int oldHerdId = horse->getHerdId();
        if(oldHerdId == 0) {
            return StatusType::FAILURE;
        }
        //find the herd - knowing existence!!! CAN! deref (not nullptr)
        const shared_ptr<Herd> tempHerdPtr = make_shared<Herd>(oldHerdId);
        const AVLTree<shared_ptr<Herd>, HerdSharedPtrComparator>::Node* oldHerdNodePtr = this->fullHerds->find(tempHerdPtr); //Theta[log(m)]
        if (oldHerdNodePtr == nullptr) {
            return StatusType::FAILURE;
        }
        const shared_ptr<Herd> oldHerdPtr = oldHerdNodePtr->data;


        //initiate leave changes
        horse->setHerdId(0);
        oldHerdPtr->removeHorse(horse); //Theta[log(m~m0)]

        //SSH level encryption SHA256 algo:
        horse->makeNewKeyForSlaves();
        horse->resetKeyToMaster();
        horse->getShadowPtr()->setPublicKey(horse->getKeyForSlaves());
        horse->getShadowPtr()->setNewHerdId(0);

        //check if herd is now empty - runs with Theta[log(m)] + Theta[log(m0)]
        if(oldHerdPtr->getSize() == 0) {
            this->fullHerds->remove(oldHerdPtr); //Theta[log(m0)]
            this->emptyHerds->insert(oldHerdPtr); //Theta[log(m)]
        }

    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    } catch (...) {
        return StatusType::FAILURE;
    }
    return StatusType::SUCCESS;
}

output_t<int> Plains::get_speed(int horseId)
{
    if (horseId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    try {
        const shared_ptr<Horse> tempHorse = make_shared<Horse>(horseId, 1);
        const AVLTree<shared_ptr<Horse>, HorseSharedPtrComparator>::Node *horseNodePtr = this->allHorses->find(tempHorse);
        if(horseNodePtr == nullptr) {
            return StatusType::FAILURE;
        }
        return horseNodePtr->data->getSpeed();
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    } catch (...) {
        return StatusType::FAILURE;
    }
}
output_t<bool> Plains::leads(int horseId, int otherHorseId)
{
    if (horseId <= 0 || otherHorseId <= 0 || horseId == otherHorseId) {
        return StatusType::INVALID_INPUT;
    }

    try {
        const shared_ptr<Horse> tempHorse = make_shared<Horse>(horseId, 1);
        const shared_ptr<Horse> tempOtherHorse = make_shared<Horse>(otherHorseId, 1);
        //the following 2 lines will run with 2*Theta[log(n)]
        const AVLTree<shared_ptr<Horse>, HorseSharedPtrComparator>::Node *horseNodePtr1 = this->allHorses->find(tempHorse);
        const AVLTree<shared_ptr<Horse>, HorseSharedPtrComparator>::Node *horseNodePtr2 = this->allHorses->find(tempOtherHorse);
        if(horseNodePtr1 == nullptr || horseNodePtr2 == nullptr) {
            return StatusType::FAILURE;
        }
        if(horseNodePtr1->data->getHerdId() != horseNodePtr2->data->getHerdId()) {
            return false;
        }
        //let's flag our beginning and end
        //int flagStartId = horseNodePtr1->data->getId();
        const int flagEndId = horseNodePtr2->data->getId();

        Horse* startHorse = horseNodePtr1->data.get();
        Horse* currentHorse = horseNodePtr1->data.get();

        //this while loop will run at most Theta[n-herdId]
        //we also need to mark visited horses.
        //not to forget to clean the visit flag.
        auxCleanFollowFlags(startHorse); //Theta[n-herd]
        while(currentHorse != nullptr && currentHorse->getFoundFlagIter() != 1) {
            currentHorse->searchMissionFlagIter(1);
            if(currentHorse->getId() == flagEndId) {
                //call func to clean the flags
                auxCleanFollowFlags(startHorse); //Theta[n-herd]
                return true;
            }
            currentHorse = currentHorse->getMasterIfConnectionIsGood();
        }
        auxCleanFollowFlags(startHorse); //Theta[n-herd]
        return false;
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    } catch (...) {
        return StatusType::FAILURE;
    }
}

//this algorithm runs Theta[log(m)] + 5*Theta[n-herd]
output_t<bool> Plains::can_run_together(int herdId)
{
    if (herdId <= 0) {
        return false;
    }

    try {
        Herd tempHerd(herdId);
        const shared_ptr<Herd> tempHerdPtr = make_shared<Herd>(tempHerd);
        const AVLTree<shared_ptr<Herd>, HerdSharedPtrComparator>::Node *herdNodePtr = this->fullHerds->find(tempHerdPtr); //Theta[log(m)]
        if (herdNodePtr == nullptr) {
            return StatusType::FAILURE;
        }

        //here we know herd exists and we found the node. can deref.
        //edge case:
        if(herdNodePtr->data->getSize() == 1) {
            return true;
        }

        //step 1. reset the search mission information on the tree in case not reset.
        herdNodePtr->data->searchMissionStep(1);
        //step 2. start evaluating how many slaves each Horse got.
        herdNodePtr->data->searchMissionStep(2);
        //step 3. gather masters + build Queue of Horses with no slave.
        HorseStackLIFO* stackOfBots = new HorseStackLIFO();
        HorseStackLIFO* stackOfTops = new HorseStackLIFO();
        herdNodePtr->data->searchMissionStep(3, stackOfBots, stackOfTops);
        if(stackOfBots->getSize() == 0 || stackOfTops->getSize() != 1) {
            delete stackOfBots;
            delete stackOfTops;
            herdNodePtr->data->searchMissionStep(5);
            return false;
        }
        delete stackOfTops;
        //step 4. start walking from bots upwards, each iter leaving flag
        int counter = 0;
        const int stackBotsSize = stackOfBots->getSize();;
        for(int iter = 1; iter <= stackBotsSize; iter++) {
            Horse* currentHorsePtr = stackOfBots->pop();
            //this is edge case where one of the bots is a single-node and will never be a master nor stumble upon branch.
            if(currentHorsePtr->getMasterIfConnectionIsGood() == nullptr) {
                delete stackOfBots;
                herdNodePtr->data->searchMissionStep(5);
                return false;
            }
            while(currentHorsePtr != nullptr) {
                const int horseIterFlag = currentHorsePtr->getFoundFlagIter();
                //this case we got a loop. delete & return false.
                if (iter == horseIterFlag) {
                    delete stackOfBots;
                    herdNodePtr->data->searchMissionStep(5);
                    return false;
                }
                //case we finished with this branch. look at next branch.
                if(horseIterFlag != 0) {
                   break;
                }
                //case vertex not visited yet
                currentHorsePtr->searchMissionFlagIter(iter);
                counter++;
                currentHorsePtr = currentHorsePtr->getMasterIfConnectionIsGood();
            }
        }
        delete stackOfBots;
        //step 5. reset the tree for future proofing. in cases where returning false in logic, also clean data.
        herdNodePtr->data->searchMissionStep(5);

        //now we know that we counted all the edges.
        //also in this connected and directed graph each slave vertex contributes 1 edge.
        //but the overall leader doesn't (as looping is not allowed).
        //also knowing in fact no loops exist in our graph, we can safely point that:
        //if counter==sizeof(herd)
        //we meet the right conditions.
        if(counter != herdNodePtr->data->getSize()) {
            return false;
        }
        return true;

    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    } catch (...) {
        return StatusType::FAILURE;
    }
}
