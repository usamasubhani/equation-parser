/*
WRITTEN BY
 _   _ ____    _    __  __    _      ____  _   _ ____  _   _    _    _   _ ___ 
| | | / ___|  / \  |  \/  |  / \    / ___|| | | | __ )| | | |  / \  | \ | |_ _|
| | | \___ \ / _ \ | |\/| | / _ \   \___ \| | | |  _ \| |_| | / _ \ |  \| || | 
| |_| |___) / ___ \| |  | |/ ___ \   ___) | |_| | |_) |  _  |/ ___ \| |\  || | 
 \___/|____/_/   \_|_|  |_/_/   \_\ |____/ \___/|____/|_| |_/_/   \_|_| \_|___|
                                                                               

 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <stack>
#include <stdlib.h>
using namespace std;



typedef enum{ RPR, LPR, PLUS, MINUS, DIVIDE, MULTIPLY,
			EQUALS, IDENTIFIER, INT, SEMICOLON }TokenType;

/*****************/
/* 	--- Tree --- */
/*****************/

struct treeNode{
	int value; //Leaf
	char opr;  //Non-leaf
	treeNode* left = NULL;
	treeNode* right = NULL;
};

//Creates and returns non leaf node
treeNode* CreateNode(char parentOpr, treeNode* left, treeNode* right)
{
	treeNode* node = new treeNode;
	node->opr = parentOpr;
	node->left = left;
	node->right = right;
	return node;
}

//Creates a leaf Node
treeNode* CreateNumberNode(int value)
{
	  treeNode* node = new treeNode;
	  node->value = value;

	  return node;
}

void printNode(treeNode* node)
{
	if (node->left == NULL && node->right == NULL)
		cout<<node->value;
	else
		cout<<node->opr;
}


/*
 * Prints the tree
 *		 -
 *	   /   \
 *    2     *
 *         /  \
 * 		  4    3
 * in form -(2)(*(4)(3))
 */
void printTree(treeNode* node, int indent=0)
{
	if(node != NULL)
	{
		printNode(node);
		
		if(node->left != NULL)
		{ 
			cout<<"(";
            printTree(node->left,indent+4);
			cout<<")";
		}
		
		
		if (node->right != NULL)
		{
			cout<<"(";
			printTree(node->right,indent+4);
			cout<<")";
		}
		
	}
}



int solveTree(treeNode* root)
{
	if(root != NULL)
	{
		if(root->left == NULL && root->right == NULL)
			return root->value;
		
		int lVal = solveTree(root->left);
		int rVal = solveTree(root->right);
		
		if( root->opr == '+')
			return lVal + rVal;
			
		if( root->opr == '-')
			return lVal - rVal;
		
		if( root->opr == '*')
			return lVal * rVal;
			
		if( root->opr == '/')
			return lVal / rVal;
		
	}
	else return 0;
}



// Global Variables
ifstream readFile; 
int tempVal; //Stores value if an INT is read by lexical
string tempString; //Stores identifier when read by lexical
bool useCurrent = false; //Flag for parser putback function, if true lex return current token
TokenType temp;
TokenType tempToken; //updates every time lexical is called
char lastOprE1; //last operator in function E1
char lastOprT1; //last operator in function T1
//-------------------


// Function Prototypes
TokenType getToken(char c);
void lexical();
bool match(TokenType tok, TokenType tok2);
void putBack(TokenType tokenToUse);

treeNode* E();
treeNode* E1();
treeNode* T();
treeNode* T1();
treeNode* F();

void ShowError(string message);
//--------------------------



int main()
{
	treeNode* tree;
	readFile.open("exp.txt");
	if(readFile.is_open())
	{
		lexical();
		tree = E();

		cout<<"Tree: \n";
		printTree(tree);

		cout<<"\n\nResult = ";
		cout<<solveTree(tree);
	}
	else
		ShowError("Cant Open File.");
	
	readFile.close();
	getchar();

	return 0;
}

void ShowError(string message)
{
	cout<<message;
	exit(1);
}



/**************************
 * --- EQUATION PARSER --- *
**************************/

treeNode* E() // E -> T E1
{
	treeNode* t = T();
	treeNode* e1 = E1();
	
	if( e1 )
		return CreateNode(lastOprE1, t, e1);
	else
		return t;
}

treeNode* E1() // E1 -> P T E1 | e
{
	treeNode* t;
	treeNode* e1;
	
	if( match(tempToken, PLUS) )
	{
		lastOprE1 = '+';
		lexical();
		t = T();
		e1 = E1();
		if(e1)
			return CreateNode('+', e1, t);
		else
			return t;
	}
	else if( match(tempToken, MINUS) )
	{
		lastOprE1 = '-';
		lexical();
		t = T();
		e1 = E1();
		
		if(e1)
			return CreateNode('-', e1, t);
		else
			return t;
	}
	else
		return NULL;
	
}

treeNode* T() // T -> F T1
{
	treeNode* f = F();
	treeNode* t1 = T1();
	
	if (t1)
		return CreateNode(lastOprT1, f, t1);
	else
		return f;
}

treeNode* T1() //T1 -> M F T1 | e
{
	treeNode* f;
	treeNode* t1;
	
	if( match(tempToken, MULTIPLY) )
	{
		lastOprT1 = '*';
		lexical();
		f = F();
		t1 = T1();
		if (t1)
			return CreateNode('*', t1, f);
		else
			return f;
	}
	else if( match(tempToken, DIVIDE) )
	{
		lastOprT1 = '/';
		lexical();
		f = F();
		t1 = T1();
		
		if (t1)
			return CreateNode('/', t1, f);
		else
			return f;
	}
	else
		return NULL;
	
}

treeNode* F() // F -> INT | (E)
{
	treeNode* temp;
	if( match(tempToken, INT) )
	{
		lexical();
		return CreateNumberNode(tempVal);
	}
	else if(match(tempToken, LPR) )
	{
		lexical();
		temp = E();
		if( match(tempToken, RPR) )
			{
				lexical();
				return temp;
			}
	}
	
	ShowError("Syntax Error!");
}


//--------------------------------//



bool match(TokenType tok, TokenType tok2)
{
	if(tok == tok2)
		return true;
	else
		return false;
}

void putBack(TokenType tokenToUse)
{
	useCurrent = true;
	temp = tokenToUse;
}



/*******************
 * --- LEXICAL --- *
 *******************/ 

void lexical()
{	if(useCurrent)
		{
			useCurrent = false;
			tempToken = temp;
		}
	else
	{
		char c;
		readFile.get(c);
		tempToken = getToken(c);
	}
}

TokenType getToken(char c)
{
	if (isspace(c))
   {
      while(readFile.peek() == isspace(c))
         readFile.get(c);
         readFile.get(c);
   }
   
   switch (c) {
      case '(':   return LPR;
      case ')':   return RPR;
      case '+':   return PLUS;
      case '-':   return MINUS;
      case '*':   return MULTIPLY;
      case '/':   return DIVIDE;
      case '=':   return EQUALS;
      case ';':   return SEMICOLON;
      default:
         if (isdigit(c))
         {
            tempVal = 0;
            string temp = "";
            while (isdigit(readFile.peek()))
               {
                  temp += c;
                  readFile.get(c);
               }
            temp += c;
            tempVal = atoi(temp.c_str());
            return INT;
         }

               
         else if (isalpha(c))
         {
            tempString = "";
            if ( isdigit(readFile.peek() ))
               {
                  tempString+=c;
                  readFile.get(c);
               }
            tempString+=c;
            return IDENTIFIER;
         }
   }
   
}

