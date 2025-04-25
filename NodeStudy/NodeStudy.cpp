#include <iostream>

struct Node
{
	int num = 0;

	Node* previousNode = nullptr;
	Node* nextNode = nullptr;
};

int main()
{
	int inputNum = 0;
	Node* baseNode = new Node;
	Node* baseNodePrev = new Node;
	Node* baseNodeNext = new Node;
	Node* printNode = new Node;
	Node* firstNode = baseNode;
	baseNode->num = 0;
	while (true)
	{
		std::cin >> inputNum;
		if (std::cin.fail() || inputNum <= 0)
		{
			break;
		}
		while (true)
		{
			if (baseNode->num < inputNum)
			{
				if (baseNode->nextNode == nullptr)
				{
					Node* newNode = new Node;
					newNode->num = inputNum;
					newNode->previousNode = baseNode;
					baseNode->nextNode = newNode;
					baseNode = newNode;
					baseNode = firstNode;
					break;
				}
				else
				{
					baseNode = baseNode->nextNode;
				}
			}
			else if (baseNode->num > inputNum)
			{
				baseNodePrev = baseNode->previousNode;
				Node* newNode = new Node;
				newNode->num = inputNum;
				newNode->previousNode = baseNodePrev;
				baseNodePrev->nextNode = newNode;
				newNode->nextNode = baseNode;
				baseNode->previousNode = newNode;
				baseNode = firstNode;
				break;
			}
			else
			{
				baseNodePrev = baseNode->previousNode;
				baseNodeNext = baseNode->nextNode;
				if (baseNodePrev)
				{
					baseNodePrev->nextNode = baseNodeNext;
				}
				if (baseNodeNext)
				{
					baseNodeNext->previousNode = baseNodePrev;
				}
				baseNode = firstNode;
				break;
			}
		}
		printNode = firstNode;
		while (printNode)
		{
			std::cout << printNode->num << " ";
			printNode = printNode->nextNode;
		}
	}
}