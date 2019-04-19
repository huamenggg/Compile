#include"semantic.h"
#include"string.h"

int IsId(char ch) {
	if(ch == '_') return 1;
	if(ch >= '0' && ch <= '9') return 1;
	if(ch >= 'a' && ch <= 'z') return 1;
	if(ch >= 'A' && ch <= 'Z') return 1;
	return 0;
}
char* Filter(char* string) {
	int i;
	for(i = 0;i < strlen(string);i++)
		if(IsId(string[i]) == 0) break;
	string[i] = '\0';
	return string;
}
