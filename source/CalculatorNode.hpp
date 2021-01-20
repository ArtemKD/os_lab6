#ifndef CALCULATORNODE_HPP
#define CALCULATORNODE_HPP

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include "MSQ_Protocol.hpp"

class CalculatorNode {
public:
    CalculatorNode() = delete;
    CalculatorNode(int clId, int msqId) : id(clId), msqId(msqId) {
        pid = fork();
        if(pid < 0) {
            std::cout << "«Error: Fork func error»" << std::endl;
        } else if(pid == 0) {
            char idStr[6], msqIdStr[6], parentId[6];
            sprintf(idStr, "%d", id);
            sprintf(msqIdStr, "%d", msqId);
            execl("CalculatorNode", "CalculatorNode", idStr, msqIdStr, NULL);
        }
    }
    ~CalculatorNode() {
        Message msg;
        size_t msgSize = sizeof(Message) - sizeof(long);
        msg.id = id+1;
        msg.code = 222;
        kill(pid, SIGTERM);
    }

    int GetId() {
        return id;
    }
    int GetPid() {
        return pid;
    }

    void Exec(ExecuteData &exData) {
        Message msg;
        size_t msgSize = sizeof(Message) - sizeof(long);
        msg.id = id+1;
        msg.code = 3;
        msg.amountOfNums = exData.size;
        msgsnd(msqId, &msg, msgSize, 0);

        SendDataMessage(msqId, id, exData);

        msgrcv(msqId, &msg, msgSize, 1, 0);
    }
private:
    pid_t pid;
    int id;
    int msqId;
};

#endif
