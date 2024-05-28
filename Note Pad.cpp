#include <iostream>
#include<conio.h>
#include <Windows.h>
#include <fstream>
#include <algorithm>
#include <stack>
#include <vector>

using namespace std;

//--------------------------Window-Resolution------------------------------//

const int screenWidth = 53;
const int screenHeight = 20;
const int suggBoxWidth = 75;
const int suggBoxHeight = 20;

//---------------------------Useful-Functions------------------------------//

void gotoxy(int x, int y)   //function to move cursor on console
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void Color(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void clearTypeBox()
{
	for (int i = 0; i < 17; ++i)
	{
		gotoxy(1, i + 2);
		for (int j = 0; j < 52; ++j)
		{
			cout << ' ';
		}
	}
}

int typeWords(vector<string>& words)
{
	int location = 0;
	size_t s = words.size();
	size_t wordSize;
	for (int i = 0; i < s; ++i)
	{
		wordSize = words[i].length();
		for (int j = 0; j < wordSize; ++j)
		{
			gotoxy((location % 52) + 1, (location / 52) + 2);
			cout << (words[i])[j];
			++location;
		}
		gotoxy((location % 52) + 1, (location / 52) + 2);
		cout << ' ';
		++location;
	}
	return location;
}

bool isValid(string toCheck)
{
	bool result = true;
	size_t length;
	length = toCheck.length();
	for (int j = 0; j < length; ++j)
	{
		if (toCheck[j] < 97 || toCheck[j] > 122)
		{
			result = false;
			break;
		}
	}
	return result;
}

string cStrToStr(char* str, int n)
{
	string temp;
	for (int i = 0; i < n; i++)
	{
		temp += str[i];
	}
	return temp;
}

void borders()
{
	gotoxy(0, 0);
	for (int i = 0; i < suggBoxWidth; ++i)
		cout << '=';
	gotoxy(0, screenHeight - 1);
	for (int i = 0; i < suggBoxWidth; ++i)
		cout << '=';
	for (int i = 0; i < screenHeight; ++i)
	{
		gotoxy(0, i);
		cout << "|";
		if (i != screenHeight - 1)
		{
			gotoxy(screenWidth, i);
			cout << "|";
		}
		gotoxy(suggBoxWidth, i);
		cout << "|";
	}
	gotoxy(1, 1);
	for (int i = 0; i < 22; ++i)
		cout << '_';
	cout << " NotePad ";
	for (int i = 0; i < 21; ++i)
		cout << '_';
	gotoxy(54, 1);
	for (int i = 0; i < 4; ++i)
		cout << '_';
	cout << " Suggestions ";
	for (int i = 0; i < 4; ++i)
		cout << '_';
	gotoxy(1, 2);
}

void clearSuggBox()
{
	int row = 2;
	for (int j = 0; j <= 16; ++j)
	{
		gotoxy(54, row);
		for (int i = 54; i <= 74; ++i)
			cout << ' ';
		++row;
	}
}

//----------------------------------------------Trie tree Node/Class Functions-------------------------------------//

struct TTNode {
	char key;
	TTNode* characters[26];				//26 characcters;
	bool isComplete;
	bool isDiscovered;
};

class Trie {

	TTNode* root;
public:
	TTNode* getRoot();
	Trie();
	TTNode* createNode();
	bool loadToTree(string[], int);		//the main function to load the array to tree

};

Trie::Trie()
{
	root = createNode();
}

TTNode* Trie::getRoot()
{
	return root;
}

TTNode* Trie::createNode()
{
	TTNode* temp = new TTNode;
	for (int i = 0; i < 26; ++i)
		temp->characters[i] = nullptr;

	temp->isComplete = false;
	temp->key = '*';
	return temp;
}

bool Trie::loadToTree(string input[], int x)
{
	string tempWord;									//temp string to store each word temporarily
	size_t alphaCount;
	TTNode* currNodePtr;
	for (int i = 0; i < x; i++)							//iterating through each word
	{
		tempWord = input[i];
		alphaCount = tempWord.length();
		currNodePtr = root;
		for (int j = 0; j < alphaCount; j++)			//iterating through each alphabet and inserting it [CAN BE DONE IN A SEPERATE FUNCTION BY PASSING IT ONLY A WORD STRING (WHICH CAN ALSO BE DONE RECURSIVELY, WILL THINK ABOUT IT LATER!!)]
		{
			if (currNodePtr->characters[static_cast<int>(tempWord[j]) - 97])
			{
				currNodePtr = currNodePtr->characters[static_cast<int>(tempWord[j]) - 97];
				if (j == alphaCount - 1)				//check if this is the last letter
					currNodePtr->isComplete = true;
			}
			else
			{
				currNodePtr->characters[static_cast<int>(tempWord[j]) - 97] = createNode();
				currNodePtr = currNodePtr->characters[static_cast<int>(tempWord[j]) - 97];
				currNodePtr->key = tempWord[j];
				if (j == alphaCount - 1)				//check if this is the last letter
					currNodePtr->isComplete = true;
			}

		}
	}
	return true;
}

void loadSuggestions(vector<string>& v, TTNode* root, char* possibleWord, string given, int n)
{
	if (root == nullptr)
	{
		return;
	}
	if (root->isComplete)
	{
		v.push_back(cStrToStr(possibleWord, n));
	}
	for (int i = 0; i < 26; i++)
	{
		if (root->characters[i] != NULL)
		{
			possibleWord[n] = root->characters[i]->key;
			loadSuggestions(v, root->characters[i], possibleWord, given, n + 1);
		}
	}
}

vector<string> getSuggestions(string tempWord, Trie dictionary)
{
	size_t alphaCount;
	alphaCount = tempWord.length();
	TTNode* currNodePtr = dictionary.getRoot(), * LLOGW = nullptr;		//LLOGW = Last Letter Of Given Word
	string possibleWord = "";
	bool isWord = true;													//to check if such word even exists

	for (int i = 0; i < alphaCount; ++i)								//traversing through the tree to the last alphabet of input word
	{
		currNodePtr = currNodePtr->characters[static_cast<int>(tempWord[i]) - 97];
		if (currNodePtr)
			possibleWord += currNodePtr->key;
		else
		{
			isWord = false;
			break;
		}
	}
	LLOGW = currNodePtr;
	possibleWord = "";
	char suggestedWord[100];
	vector<string> suggestedWords;		//vector to store suggestions
	if(isWord)							//check if such word even exists
		loadSuggestions(suggestedWords, LLOGW, suggestedWord, tempWord, 0);
	return suggestedWords;
}

//---------------------------------HUFFMAN------------------------------//

struct HuffTNode {											//Tree node
	HuffTNode* left, * right;
	char key;
	int frequ;
	HuffTNode();
};

HuffTNode::HuffTNode()
{
	key = '*';
	frequ = 0;
	left = nullptr;
	right = nullptr;
}

HuffTNode* createHuffTNode(char x, int p)
{
	HuffTNode* temp = new HuffTNode;
	temp->key = x;
	temp->frequ = p;
	return temp;
}

struct HuffLLNode {											//Linkedlist Node
	HuffLLNode* next;
	HuffTNode* ptr;
	HuffLLNode();
};

HuffLLNode::HuffLLNode()
{
	next = nullptr;
	ptr = nullptr;
}

struct HuffLL {												//Linkedlist
	HuffLLNode* head;

	HuffLL();
	HuffLLNode* createNode(char, int);
	HuffLLNode* createNode(HuffTNode*);				//overloaded insert used while merging
	void insert(char, int);
	void insert(HuffTNode*);						//overloaded insert used while merging
	HuffTNode* get_del();
	void merge();

};

HuffLL::HuffLL()
{
	head = nullptr;
}

HuffLLNode* HuffLL::createNode(char x, int p)
{
	HuffLLNode* temp = new HuffLLNode;
	temp->ptr = createHuffTNode(x, p);
	return temp;

}

HuffLLNode* HuffLL::createNode(HuffTNode* newNode)
{
	HuffLLNode* temp = new HuffLLNode;
	temp->ptr = newNode;
	return temp;

}

void HuffLL::insert(char x, int p)
{
	if (!head)
	{
		head = createNode(x, p);
		return;
	}
	else if (p <= head->ptr->frequ)								//case: insert at begin
	{
		HuffLLNode* temp = createNode(x, p);
		temp->next = head;
		head = temp;
		return;
	}
	else
	{
		HuffLLNode* curr = head;
		while (curr->next)
		{
			curr = curr->next;
		}
		if (p >= curr->ptr->frequ)								//case: insert at end
		{
			curr->next = createNode(x, p);
			return;
		}
		else													//case: insert in middle
		{
			curr = head;
			while (curr->next)
			{
				if (curr->next->ptr->frequ > p)					//insert here
				{
					HuffLLNode* temp = createNode(x, p);
					temp->next = curr->next;
					curr->next = temp;
					return;
				}
				curr = curr->next;
			}
			return;
		}
	}
}

void HuffLL::insert(HuffTNode* newNode)
{
	if (!head)
	{
		head = createNode(newNode);
		return;
	}
	else if (newNode->frequ <= head->ptr->frequ)					//case: insert at begin
	{
		HuffLLNode* temp = createNode(newNode);
		temp->next = head;
		head = temp;
		return;
	}
	else
	{
		HuffLLNode* curr = head;
		while (curr->next)
		{
			curr = curr->next;
		}
		if (newNode->frequ >= curr->ptr->frequ)						//case: insert at end
		{
			curr->next = createNode(newNode);
			return;
		}
		else														//case: insert in middle
		{
			curr = head;
			while (curr->next)
			{
				if (curr->next->ptr->frequ > newNode->frequ)		//insert here
				{
					HuffLLNode* temp = createNode(newNode);
					temp->next = curr->next;
					curr->next = temp;
					return;
				}
				curr = curr->next;
			}
			return;
		}
	}
}

HuffTNode* HuffLL::get_del()
{
	HuffTNode* temp = head->ptr;
	if (head->next)
		head = head->next;
	else
		head = nullptr;
	return temp;
}

void HuffLL::merge()
{
	HuffLLNode* curr = head;
	int count = 1;
	while (curr->next)
	{
		++count;
		curr = curr->next;
	}
	HuffTNode* node1, * node2, * node3;

	//loop start
	for (int i = 0; i < count - 1; ++i)
	{
		node3 = new HuffTNode;
		node1 = get_del();
		node2 = get_del();
		node3->left = node1;
		node3->right = node2;
		node3->frequ = node1->frequ + node2->frequ;
		insert(node3);
	}	//loop end

}

int* findFreq(vector<string> text, int freq[26])
{
	string word;
	size_t s = 0;
	for (auto i = text.begin(); i < text.end(); ++i)
	{
		word = *i;
		s = word.length();
		for (int j = 0; j < s; ++j)
		{
			++freq[static_cast<int>(word[j]) - 97];
		}
	}
	return freq;
}

HuffLL* genHuffTree(vector<string> text, bool mode)		// true = encode	false = decode
{

	fstream encode;
	int freq[26] = { 0 };
	if (mode)								//encode mode
	{
		findFreq(text, freq);
		encode.open("encoded.txt", ios::out);
		for (int i = 0; i < 26; ++i)
		{
			encode << freq[i] << ' ';
		}
		encode << '\n';
		encode.close();
	}
	else									//decode mode
	{
		encode.open("encoded.txt", ios::in);
		for(int i=0;i<26;++i)
			encode >> freq[i];
	}
	HuffLL* LL = new HuffLL;
	for (int i = 0; i < 26; ++i)
	{
		if (freq[i])
		{
			LL->insert(static_cast<char>(i + 97), freq[i]);
		}
	}
	LL->merge();
	//Now we have our first step (creating a LL) done.
	return LL;
}

void getCode(HuffTNode* node, string code, char x, string &fcode)
{
	if (node->left == NULL && node->right == NULL)
	{
		if(x == node->key)
			fcode = code;
		return;
	}
	else
	{
		getCode(node->left, code + '0', x, fcode);
		getCode(node->right, code + '1', x, fcode);
	}
}

void encode(vector<string>words, HuffLL* root)
{
	fstream encode;
	encode.open("encoded.txt", ios::app);
	size_t s = words.size();
	size_t noc;
	string code;
	for (int i = 0; i < s; ++i)
	{
		noc = words[i].length();
		for (int j = 0; j < noc; ++j)
		{
			getCode(root->head->ptr, "", (words[i])[j], code);
			encode << code;
		}
		encode << ' ';
	}
	encode.close();
}

int decode(vector<string>& words, HuffLL* root)
{
	words.clear();
	fstream decode;
	decode.open("encoded.txt", ios::in);
	string temp;
	size_t codeSize = 0;
	string word = "";
	int t;
	
	for (int i = 0; i < 26; ++i)
		decode >> t;
	clearTypeBox();
	clearSuggBox();
	int location = 0;
	HuffTNode* curr = root->head->ptr;
	while (1)
	{
		decode >> temp;
		if (decode.eof())
			break;
		codeSize = temp.length();
		for (int i = 0; i < codeSize; ++i)
		{
			if (temp[i] == '1')
			{
				curr = curr->right;
				if (curr->key != '*')
				{
					gotoxy((location % 52) + 1, (location / 52) + 2);
					cout << curr->key;
					word += curr->key;
					curr = root->head->ptr;
					++location;
				}
			}
			else if (temp[i] == '0')
			{
				curr = curr->left;
				if (curr->key != '*')
				{
					gotoxy((location % 52) + 1, (location / 52) + 2);
					cout << curr->key;
					word += curr->key;
					curr = root->head->ptr;
					++location;
				}
			}
		}
		cout << ' ';
		words.push_back(word);
		word = "";
		++location;
	}
	return location;
}

//-----------------------------------------------Main-Control-Function--------------------------------------//

void startTyping(Trie& dictionary)
{
	borders();
	char input;
	int location = 0;
	int selected = 0;
	size_t NOfSugg = 0;
	int j = 0;
	string word = "";
	string temp;
	vector<string> words;
	vector<string> suggestions;
	while (1)
	{
		input = _getch();
		if ((input >= 97 && input <= 122) || input == '/' || input == ';' || input == '=' || input == ' ' || input == '-')
			switch (input)
			{
			case'/':		//next option
			{
				if (selected < NOfSugg - 1)
					++selected;
				else
					selected = 0;
				j = 3;
				for (auto i = suggestions.begin(); i < suggestions.end() && j < 13; ++i)
				{
					if ((j - 3) == selected)
						Color(1);
					else
						Color(7);
					gotoxy(57, j);
					cout << word << *i;
					++j;
				}
				Color(7);
				break;
			}
			case';':		//select
			{
				temp = suggestions[selected];
				for (int i = 0; i < temp.length(); ++i)
				{
					gotoxy((location % 52) + 1, (location / 52) + 2);
					cout << temp[i];
					word += temp[i];
					++location;
				}
				cout << ' ';
				++location;
				words.push_back(word);
				word = "";
				clearSuggBox();
				selected = 0;
				break;
			}
			case'=':		//save
			{
				words.push_back(word);
				encode(words, genHuffTree(words, true));
				clearTypeBox();
				gotoxy(25, 9);
				cout << "Saved!";
				Sleep(2000);
				clearTypeBox();
				location = typeWords(words);
				break;
			}
			case'-':		//retrieve
			{
				location = decode(words, genHuffTree(words, false));
				break;
			}
			case' ':		//word complete [GIVE NO SUGGESTIONS]
			{
				gotoxy((location % 52) + 1, (location / 52) + 2);
				cout << ' ';
				++location;
				words.push_back(word);
				word = "";
				clearSuggBox();
				selected = 0;
				break;
			}
			default:
			{
				gotoxy((location % 52) + 1, (location / 52) + 2);
				cout << input;
				++location;
				word += input;
				suggestions = getSuggestions(word, dictionary);
				NOfSugg = suggestions.size();
				if (NOfSugg > 10)
					NOfSugg = 10;
				clearSuggBox();
				if (NOfSugg != 0)
				{
					j = 3;
					for (auto i = suggestions.begin(); i < suggestions.end() && j < 13; ++i)
					{
						if ((j - 3) == selected)
							Color(1);
						else
							Color(7);
						gotoxy(57, j);
						cout << word << *i;
						++j;
					}
					Color(7);
				}
				else
					clearSuggBox();
			}
			}
	}
}

int main()
{
	fstream input;
	input.open("outfile.txt", ios::in);
	int x = 0;												//to count words in the file initially
	string validWordCheck;									//a string to check if the word is valid [i.e.	IT CONTAIN ONLY ALPHABETS (a-z)
	while (!input.eof())
	{
		input >> validWordCheck;
		if (isValid(validWordCheck))
			++x;
	}
	string* wordArr = new string[x];
	input.close();								//to move the cursor back to top
	input.open("outfile.txt", ios::in);
	//length of string
	for (int i = 0; i < x; ++i)
	{
		input >> validWordCheck;
		if (isValid(validWordCheck))
			wordArr[i] = validWordCheck;
		else
			--i;
	}
	//whole dictionary now loaded in the array

	Trie tempObj;
	tempObj.loadToTree(wordArr, x);				//create tree
	startTyping(tempObj);
}