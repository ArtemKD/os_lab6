#include <iostream>
#include <string>

#include "NodeVector.hpp"
#include "ManagerNode.hpp"
#include "MSQ_Protocol.hpp"

void StartConfig(NodeVector<ManagerNode*>&);
void RemoveConfig(NodeVector<ManagerNode*>&);
int MainLoop(NodeVector<ManagerNode*>&);

int main(int argc, char* cargv[]) {
    NodeVector<ManagerNode*> mns;
    StartConfig(mns);

    int code = MainLoop(mns);
    //std::cout << "Code: " << code << std::endl;

    RemoveConfig(mns);
    return 0;
}

void StartConfig(NodeVector<ManagerNode*> &mns) {
    int msqId = msgget(0, IPC_CREAT | 0660);
    mns.SetMsqId(msqId);
    ManagerNode* tmp = new ManagerNode(8080, msqId);
    mns.push_back(tmp);
    tmp = new ManagerNode(8081, msqId);
    mns.push_back(tmp);
    tmp = new ManagerNode(8082, msqId);
    mns.push_back(tmp);
    tmp = new ManagerNode(8083, msqId);
    mns.push_back(tmp);
    tmp = new ManagerNode(8084, msqId);
    mns.push_back(tmp);
    tmp = new ManagerNode(8085, msqId);
    mns.push_back(tmp);
}
void RemoveConfig(NodeVector<ManagerNode*> &mns) {
    for(size_t i = 0; i < mns.size(); ++i) {
        delete mns[i];
    }
    msgctl(mns.GetMsqId(), IPC_RMID, NULL);
}
int MainLoop(NodeVector<ManagerNode*> &mns) {
    std::string command;
    int id, parentId;
    while(std::cin >> command) {
        if(command == "create") {
            std::cin >> id >> parentId;
            int index;
            if((index = mns.CheckId(parentId)) == -1) {
                std::cout << "«Error: Parent not found»" << std::endl;
                continue;
            }
            int pid;
            if(pid = mns[index]->Create(id)) {
                std::cout << "«Ok: " << pid << "»" << std::endl;
            } else {
                std::cout << "«Error: Already exists»" << std::endl;
            }
        } else if(command == "remove") {
            std::cin >> id >> parentId;
            int index;
            if((index = mns.CheckId(parentId)) == -1) {
                std::cout << "«Error: Parent not found»" << std::endl;
                continue;
            }
            if(mns[index]->Remove(id)) {
                std::cout << "«Ok»" << std::endl;
            } else {
                std::cout << "«Error: Not found»" << std::endl;
            }
        } else if(command == "exec") {
            std::cin >> id >> parentId;
            int index;
            if((index = mns.CheckId(parentId)) == -1) {
                std::cout << "«Error: Parent not found»" << std::endl;
                continue;
            }
            ExecuteData exData;
            std::cin >> exData.size;
            exData.nums = new double[exData.size];
            for(size_t i = 0; i < exData.size; ++i) {
                std::cin >> exData.nums[i];
            }
            int code;
            if((code = mns[index]->Exec(id, exData)) == 0) {
                std::cout << "«Error:" << id << ": Not found»" << std::endl;
            }
        } else if(command == "union") {
            std::cin >> id >> parentId;
            int code = mns.Union(id, parentId);
            if(code == 1) {
                std::cout << "«Error: id "<< id << " and " << parentId << ": Not found»" << std::endl;
            } else if(code == 2) {
                std::cout << "«Error: id "<< id << ": Not found»" << std::endl;
            } else if(code == 3) {
                std::cout << "«Error: id "<< parentId << ": Not found»" << std::endl;
            } else if(code == 4) {
                std::cout << "«Error: Union same nodes»" << std::endl;
            } else if(code == 5) {
                std::cout << "«Error: Already union»" << std::endl;
            } else {
                std::cout << "«Ok»" << std::endl;
            }
        } else if(command == "print") {
            mns.Print();
        }
    }
    return 0;
}
