#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define RED "\033[0;32;31m"
#define NONE "\033[m"

#ifndef YST_N
#define YST_N
typedef struct Node_* Node;
enum NodeType { Variable, Terminal, Number, TypeN, Id };
struct Node_ {
	char *name;
	enum NodeType nodeType;
	char stringValue[300];
	int numValue;
	float numValueF;
	int childNum;
	Node child[8];
};
#endif

Node root;
Node CreateTerminal(char* nameF, enum NodeType typeF,
		char* content, int numContent, float fn);
Node CreateVariable(char* nameF, int numOfChild,
		Node* childNode);
void preTraverse(Node root, int blankNum);
void printSytaxTree();
