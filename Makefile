FLAGS = --std=c++17

SOURCE_DIR = source/

OS_LAB6_REQ = 	$(SOURCE_DIR)main.cpp $(SOURCE_DIR)NodeVector.hpp $(SOURCE_DIR)ManagerNode.hpp $(SOURCE_DIR)MSQ_Protocol.hpp

MANAGER_NODE_REQ = $(SOURCE_DIR)ManagerNode.cpp $(SOURCE_DIR)NodeVector.hpp $(SOURCE_DIR)CalculatorNode.hpp $(SOURCE_DIR)MSQ_Protocol.hpp
CALCULATOR_NODE_REQ = $(SOURCE_DIR)CalculatorNode.cpp $(SOURCE_DIR)NodeVector.hpp $(SOURCE_DIR)MSQ_Protocol.hpp

all: os_lab6 ManagerNode CalculatorNode

os_lab6 : $(OS_LAB6_REQ)
	g++ $(FLAGS) $(SOURCE_DIR)main.cpp -o os_lab6

ManagerNode: $(MANAGER_NODE_REQ)
	g++ $(FLAGS) $(SOURCE_DIR)ManagerNode.cpp -o ManagerNode

CalculatorNode: $(CALCULATOR_NODE_REQ)
	g++ $(FLAGS) $(SOURCE_DIR)CalculatorNode.cpp -o CalculatorNode
clean:
	rm os_lab6 ManagerNode CalculatorNode
