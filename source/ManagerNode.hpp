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
        msgsnd(msqId, &msg, msgSize, 0);
        msgrcv(msqId, &msg, msgSize, 1, 0);
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
        return msg.returnValue;
    }
    int Union(ManagerNode* mn) {
        return 0;
    }
    int UpdateCls() {
        return 0;
    }
private:
    pid_t pid;
    int id;
    int msqId;
};

#endif
