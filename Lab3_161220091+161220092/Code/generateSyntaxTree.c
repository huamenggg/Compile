#include "generateSyntaxTree.h"

int IsRelop(char ch) {
	if(ch == '<' || ch == '>' || ch == '=' || ch == '!') return 1;
	return 0;
}

char* RelopFilter(char* string) {
	int i;
	for(i = 0;i < strlen(string);i++)
		if(IsRelop(string[i]) == 0) break;
	string[i] = '\0';
	return string;
}

Node CreateTerminal(char* nameF, enum NodeType typeF,
		char* content, int numContent, float fn) {
	Node p = (Node)malloc(sizeof(struct Node_));
	p->name = nameF;
	p->nodeType = typeF;
	p->childNum = 0;
	if(typeF == TypeN | typeF == Id) {
		strcpy(p->stringValue, content);
	}
	else if(typeF == Terminal && strcmp(nameF, "RELOP") == 0) {
		strcpy(p->stringValue, RelopFilter(content));
	}
	else if(typeF == Number) {
		if(strcmp(p->name, "INT") == 0) {
			p->numValue = numContent;
		}
		else {
			p->numValueF = fn;
		}
	}
	return p;
}

Node CreateVariable(char* nameF, int numOfChild,
		Node* childNode) {
	int i;
	Node p = (Node)malloc(sizeof(struct Node_));
	p->name = nameF;
	p->nodeType = Variable;
	p->childNum = numOfChild;
	for(i = 0;i < numOfChild;i++) {
		p->child[i] = childNode[i];
	}
	return p;
}

void preTraverse(Node root, int blankNum) {
	int i;
	for(i = 0;i < blankNum;i++)
		printf(" ");
	if(root->nodeType == Variable)
		printf("%s\n", root->name);
	else if(root->nodeType == Id || root->nodeType == TypeN) {
		printf(RED"%s: %s\n"NONE, root->name, root->stringValue);
	}
	else if(root->nodeType == Number) {
		if(strcmp(root->name, "INT") == 0)
			printf(RED"%s: %d\n"NONE, root->name, root->numValue);
		else
			printf(RED"%s: %f\n"NONE, root->name, root->numValueF);
	}
	else
		printf("%s\n", root->name);
	for(i = 0;i < root->childNum;i++) {
		if(root->child[i] != NULL) {
			preTraverse(root->child[i], blankNum + 2);
		}
	}
}

void printSytaxTree() {
	if(root == NULL) {
		printf("This is an empty syntax tree, but acceptable\n");
		return;
	}
	preTraverse(root, 0);	
}
