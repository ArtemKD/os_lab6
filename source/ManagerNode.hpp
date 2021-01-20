#ifndef MANAGERNODE_HPP
#define MANAGERNODE_HPP

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include "MSQ_Protocol.hpp"

class ManagerNode {
public:
    ManagerNode() = delete;
    ManagerNode(int mnId, int msqId) : id(mnId), msqId(msqId) {
        pid = fork();
        if(pid < 0) {
            std::cout << "«Error: Fork func error»" << std::endl;
        } else if(pid == 0) {
            char idStr[6], msqIdStr[6], parentId[6];
            sprintf(idStr, "%d", id);
            sprintf(msqIdStr, "%d", msqId);
            execl("ManagerNode", "ManagerNode", idStr, msqIdStr, NULL);
        }
    }
    ~ManagerNode() {
        Message msg;
        size_t msgSize = sizeof(Message) - sizeof(long);
        msg.id = id+1;
        msg.code = 222;
        msgsnd(msqId, &msg, msgSize, 0);
        msgrcv(msqId, &msg, msgSize, 1, 0);
        kill(pid, SIGTERM);
    }

    int GetId() {
        return id;
    }

    int Create(int cnId) {
        Message msg;
        size_t msgSize = sizeof(Message) - sizeof(long);
        msg.id = id+1;
        msg.code = 1;
        msg.cnId = cnId;
        for(size_t i = 0; i < unionNodes.size(); ++i) {
            if(unionNodes[i]->CheckCnIds(cnId) != -1) {
                return 0;
            }
        }
        msgsnd(msqId, &msg, msgSize, 0);
        msgrcv(msqId, &msg, msgSize, 1, 0);
        cnIds.push_back(cnId);
        return msg.returnValue;
    }
    int Remove(int cnId) {
        Message msg;
        size_t msgSize = sizeof(Message) - sizeof(long);
        msg.id = id+1;
        msg.code = 2;
        msg.cnId = cnId;
        msgsnd(msqId, &msg, msgSize, 0);
        msgrcv(msqId, &msg, msgSize, 1, 0);
        int index = CheckCnIds(cnId);
        if(index != -1) {
            cnIds.erase(cnIds.begin() + index);
        }
        return msg.returnValue;
    }
    int Exec(int cnId, ExecuteData &exData) {
        Message msg;
        size_t msgSize = sizeof(Message) - sizeof(long);
        msg.id = id+1;
        msg.code = 3;
        msg.cnId = cnId;
        msg.amountOfNums = exData.size;
        msgsnd(msqId, &msg, msgSize, 0);

        SendDataMessage(msqId, id, exData);

        msgrcv(msqId, &msg, msgSize, 1, 0);
        if(msg.returnValue == 0) {
            for(size_t i = 0; i < unionNodes.size(); ++i) {
                if(unionNodes[i]->Exec(cnId, exData) == 1) {
                    return 1;
                }
            }
        }
        return msg.returnValue;
    }
    int Union(ManagerNode* mn) {
        if(this == mn) {
            return 4;
        }
        if(CheckId(mn->id) != -1) {
            return 5;
        }
        RecursiveUnion(mn);
        RemoveRepeatCns();

        return 0;
    }

    int RecursiveUnion(ManagerNode* mn) {
        if(this == mn) {
            return 5;
        }
        if(unionNodes.CheckId(mn->id) != -1) {
            return 4;
        }

        unionNodes.push_back(mn);
        mn->unionNodes.push_back(this);

        for(size_t i = 0; i < unionNodes.size(); ++i) {
            if(mn != unionNodes[i]) {
                mn->Union(unionNodes[i]);
            }
        }
        for(size_t i = 0; i < mn->unionNodes.size(); ++i) {
            if(this != mn->unionNodes[i]) {
                Union(mn->unionNodes[i]);
            }
        }

        return 0;
    }

    int RecursiveUnionDebug(ManagerNode* mn) {
        if(this == mn) {
            return 5;
        }
        if(unionNodes.CheckId(mn->id) != -1) {
            std::cout << "00000\n";
            return 4;
        }

        unionNodes.push_back(mn);
        mn->unionNodes.push_back(this);
        std::cout << "id1: " << this->id << " id2: " << mn->id << std::endl;

        for(size_t i = 0; i < unionNodes.size(); ++i) {
            std::cout << "1!!\n";
            if(mn != unionNodes[i]) {
                std::cout << "1!\n";
                mn->Union(unionNodes[i]);
            }
        }
        std::cout << "--------\n";
        for(size_t i = 0; i < mn->unionNodes.size(); ++i) {
            std::cout << "2!!\n";
            if(this != mn->unionNodes[i]) {
                std::cout << "2!\n";
                Union(mn->unionNodes[i]);
            }
        }

        return 0;
    }

    void RemoveRepeatCns() {
        for(size_t i = 0; i < unionNodes.size(); ++i) {
            for(size_t j = 0; j < unionNodes[i]->cnIds.size(); ++j) {
                if(CheckCnIds(unionNodes[i]->cnIds[j]) != -1) {
                    unionNodes[i]->Remove(unionNodes[i]->cnIds[j]);
                    --j;
                }
            }
        }
    }

    int CheckCnIds(int id) {
        for(size_t i = 0; i < cnIds.size(); ++i) {
            if(cnIds[i] == id) {
                return i;
            }
        }
        return -1;
    }
    void Print() {
        std::cout << "ManagerNode:: id: " << id << "; pid: " << pid << "; msqId: " << msqId << std::endl;
        std::cout << "CalculatorNode ids: ";
        for(size_t i = 0; i < cnIds.size(); ++i) {
            std::cout << cnIds[i] << ", ";
        }
        std::cout << std::endl;
        std::cout << "Union ManagerNodes: ";
        for(size_t i = 0; i < unionNodes.size(); ++i) {
            std::cout << unionNodes[i]->id << ", ";
        }
        std::cout << std::endl;
    }
private:
    pid_t pid;
    int id;
    int msqId;
    std::vector<int> cnIds;
    NodeVector<ManagerNode*> unionNodes;
};

#endif
