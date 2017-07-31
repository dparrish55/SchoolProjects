#include <ctime>
#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <iomanip>
#include <fstream>
#include <stack>

using namespace std;

const int MAX_SYMBOL_TABLE_SIZE = 256;
enum storeType {INTEGER, BOOLEAN, PROG_NAME, UNKNOWN};
enum allocation {YES,NO};
enum modes {VARIABLE, CONSTANT};
bool errorBus = false;
bool newLine = false;
int boolCount = 0;
int progCount = 0;
int  intCount = 0;
int lineCount = 0;
int depthCounter = -1;
int currentTempNo = -1;
int maxTempNo = -1;
int labelCount = -1;
string regContents = "";
bool trueInTable = false;
bool falseInTable = false;
bool zeroInTable = false;

void CreateListingHeader();
void Parser();
void CreateListingTrailer();
char NextChar();
string NextToken();
string nextLabel();
void Prog();
void ProgStmt();
void Consts();
void ConstStmts();
void Vars();
void VarStmts();
void BeginEndStmt();
string Ids();
void PrintSymbolTable();
void EXEC_STMTS();
void EXEC_STMT();
void ASSIGN_STMT();
void READ_STMT();
string READ_LIST();
void WRITE_STMT();
string WRITE_LIST();
void EXPRESS();
void EXPRESSES();
void TERM();
void TERMS();
void FACTOR();
void FACTORS();
void PART();
void REL_OP();
void ADD_LEVEL_OP();
void MULT_LEVEL_OP();
void IF_STMT();
void ELSE_PT();
void WHILE_STMT();
void REPEAT_STMT();
void NULL_STMT();
void Insert(string, storeType, modes, string, allocation, int);
storeType WhichType(string);
string WhichValue(string);
string GenInternalName(storeType);
void errorCatch(string);
bool isBoolean(string);
bool isInteger(string);
void PushOperator(string);
void PushOperand(string);
string PopOperator();
string PopOperand();
void Code(string operatorio, string operand1 = "", string operand2 = "");
bool isKeyword(string);
bool isTemp(string);
int getTablePos(string);
void EmitAdditionCode(string, string);
void EmitSubtractionCode(string,string);
void EmitNegationCode(string);
void EmitSpanishInquisitionCode(string); //operator unary plus
void EmitNotCode(string);
void EmitMultiplicationCode(string, string);
void EmitDivisionCode(string,string);
void EmitModuloCode(string, string);
void EmitAndCode(string,string);
void EmitOrCode(string,string);
void EmitEqualsCode(string,string);
void EmitLessThanCode(string, string);
void EmitLessThanEqualCode(string, string);
void EmitGreaterThanCode(string, string);
void EmitGreaterThanEqualCode(string, string);
void EmitNotEqualCode(string, string);
void EmitAssignCode(string,string);
void EmitThenCode(string);
void EmitElseCode(string);
void EmitPostIfCode(string);
void EmitWhileCode();
void EmitDoCode(string);
void EmitPostWhileCode(string, string);
void EmitRepeatCode();
void EmitUntilCode(string, string);
void FreeTemp();
string GetTemp();

bool operator!=(string, storeType);

struct entry //define symbol table entry format
{
	string internalName;
	string externalName;
	storeType dataType;
	modes mode;
	string value;
	allocation alloc;
	int units;
};

vector<entry> symbolTable;
ifstream sourceFile;
ofstream listingFile,objectFile;
string token;
char charac;
char prev;
int charPos = 0;
const char END_OF_FILE = '$'; // arbitrary choice

stack <string> operatorStk;
stack <string> operandStk;

int main(int argc, char **argv)
{
	//this program is the stage2 compiler for Pascallite. It will accept
	//input from argv[1], generating a listing to argv[2], and object code to
	//argv[3]
	sourceFile.open(argv[1], ios::in);
	listingFile.open(argv[2], ios::out | ios::app);
	objectFile.open(argv[3], ios::out | ios::app);
	
	CreateListingHeader();
	Parser();
	CreateListingTrailer();
	
	// for(uint i = 0; i < symbolTable.size(); i++)
	// {
		// objectFile << symbolTable[i].externalName << setw((17-symbolTable[i].externalName.size())+2) << symbolTable[i].internalName << setw(13)
				   // << (symbolTable[i].dataType == 2 ? "PROG_NAME" : (symbolTable[i].dataType == 1 ? "BOOLEAN" : "INTEGER")) << setw(10) 
				   // << (symbolTable[i].mode == 1 ? "CONSTANT" : "VARIABLE") << setw(17) 
				   // << (symbolTable[i].value == "true" ? "1" : (symbolTable[i].value == "false" ? "0" : symbolTable[i].value)) << setw(5) 
				   // << (symbolTable[i].alloc == 1 ? "NO" : "YES") << setw(3) << symbolTable[i].units << endl;
	// }
	
	sourceFile.close();
	listingFile.close();
	objectFile.close();
	
	return 0;
}

bool operator!=(string lhs, storeType rhs)
{
	if (lhs == "INTEGER" && rhs == INTEGER)
	{
		return false;
	}
	else if (lhs == "BOOLEAN" && rhs == BOOLEAN)
	{
		return false;
	}
	else if (lhs == "PROG_NAME" && rhs == PROG_NAME)
	{
		return false;
	}
	else
	{
		return true;
	}
}


void CreateListingHeader()
{
	time_t now = time(NULL);
	listingFile << "STAGE2: R Jones, D Parrish  " << ctime(&now) << endl;
	listingFile << "LINE NO.              SOURCE STATEMENT" << endl;
	//line numbers and source statements should be aligned under the headings
}

void Parser()
{
	NextChar();
	//charac must be initialized to the first character of the source file
	if(NextToken() != "program")
		errorCatch("keyword \"program\" expected");
	//a call to NextToken() has two effects
	// (1) the variable, token, is assigned the value of the next token
	// (2) the next token is read from the source file in order to make
	// the assignment. The value returned by NextToken() is also
	// the next token.
	Prog();
	//parser implements the grammar rules, calling first rule
}

void CreateListingTrailer()
{
	listingFile << endl << "COMPILATION TERMINATED      " << errorBus << " ERRORS ENCOUNTERED" << endl;
}

char NextChar() //returns the next character or end of file marker
{
	if(charPos > 0)
		prev = charac;
	sourceFile.get(charac);
	if (!sourceFile.good())
	{
		charac = END_OF_FILE; //use a special character to designate end of file
	}
	else
	{
		if (lineCount == 0)
		{
			listingFile << endl << setw(5) << ++lineCount << "|" << charac;
		}
		else if (charac == '\n') 
		{
			if (newLine)
			{
				newLine = false;
				listingFile << setw(5) << ++lineCount << "|";
			}
			listingFile << endl;
			newLine = true;
		}
		else 
		{
			if (newLine)
			{
				newLine = false;
				listingFile << setw(5) << ++lineCount << "|";
			}
			listingFile << charac;
		}
	}
	
	charPos++;
	
	return charac;
}

string NextToken() //returns the next token or end of file marker
{
	token = "";
	char x;
	while (token == "")
	{
		if (charac == '{') //process comment
		{
			x = NextChar();
			while (x != '}')
			{
				if (charac==END_OF_FILE)
					errorCatch("unexpected end of file");
				else
					x = NextChar();
			}
			NextChar();
		}
		else if (charac == '}') errorCatch("'}' cannot begin token");
		else if (isspace(charac)) NextChar();
		else if (charac == ':')
		{
			token = charac;
			NextChar();
			if (charac == '=')
			{
				token += charac;
				NextChar();
			}
		}
		else if (charac == '<')
		{
			token = charac;
			if (NextChar() == '=')
			{
				token += charac;
				NextChar();
			}
			else if (charac == '>')
			{
				token += charac;
				NextChar();
			}
			
		}
		else if (charac == '>')
		{
			token = charac;
			if (NextChar() == '=')
			{
				token += charac;
				NextChar();
			}
		}
		else if (charac == '=' || charac == ';' || charac == ',' || charac == '.' || charac == '+' || charac == '-' || charac == '*' || charac == '(' || charac == ')')
		{
			token = charac;
			NextChar();
		}
		else if (isalpha(charac))
		{
			token = charac;
			char x = NextChar();
			while (isalpha(x) || isdigit(x) || x == '_')
			{
				token+=x;
				x = NextChar();
			}
			if (token.at(token.length()-1) == '_')
				errorCatch("'_' cannot end token");
		}
		else if(isdigit(charac)) 
		{
			token = charac;
			while (isdigit(NextChar())) token+=charac;
		}
		else if (NextChar() == END_OF_FILE)
			token = "$";
		else
			errorCatch("illegal symbol");
	}
	return token;
}

void Prog() //token should be "program"
{
	if (token != "program")
		errorCatch("keyword \"program\" expected");
	ProgStmt();
	if (token == "const") Consts();
	if (token == "var") Vars();
	if (token != "begin")
		errorCatch("keyword \"begin\" expected");
	BeginEndStmt();
	if (token != "$")
		errorCatch("no text may follow \"end\"");
}

void ProgStmt() //token should be "program"
{
	string x;
	if (token != "program")
	errorCatch("keyword \"program\" expected");
	x = NextToken();
	if (isKeyword(token))
		errorCatch("program name expected");
	Code("program", token);
	if (NextToken() != ";")
	errorCatch("semicolon expected");
	NextToken();
	Insert(x,PROG_NAME,CONSTANT,x,NO,0);
}

void Consts() //token should be "const"
{
	if (token != "const")
		errorCatch("keyword \"const\" expected");
	NextToken();
	if (isKeyword(token))
		errorCatch("non-keyword identifier must follow \"const\"");
	ConstStmts();
}

void ConstStmts() //token should be NON_KEY_ID
{
	string x,y;
	if (isKeyword(token))
		errorCatch("non-keyword identifier expected");
	x = token;
	if (NextToken() != "=")
		errorCatch("error: \"=\" expected");
	y = NextToken();
	if (y != "+" && y != "-" && y != "not" && y != "true" && y != "false" && WhichType(y) != INTEGER && WhichType(y) != BOOLEAN && !isKeyword(y))
		errorCatch("token to right of \"=\" illegal");
	if (y == "+" || y == "-")
	{
		if(!isInteger(NextToken()))
			errorCatch("integer expected after sign");
		y = y + token;
	}
	if (y == "not")
	{
		if(!isBoolean(NextToken()))
			errorCatch("boolean expected after not");
		if(token == "true")
			y = "false";
		else
			y = "true";
	}
	if (NextToken() != ";")
		errorCatch("semicolon expected");
	Insert(x,WhichType(y),CONSTANT,WhichValue(y),YES,1);
	y = NextToken();
	if (y != "begin" && y != "var" && isKeyword(y))
		errorCatch("non-keyword identifier,\"begin\", or \"var\" expected");
	if (!isKeyword(token))
	{
		ConstStmts();
	}
}

void Vars() //token should be "var"
{
	if (token != "var")
		errorCatch("keyword \"var\" expected");
	string y = NextToken();
	if (isKeyword(y))
		errorCatch("non-keyword identifier must follow \"var\"");
	VarStmts();
}

void VarStmts() //token should be NON_KEY_ID
{
	string x,y;
	y = token;
	if (isKeyword(y))
		errorCatch("non-keyword identifier expected");
	x = Ids();
	if (token != ":")
		errorCatch("\":\" expected");
	if(NextToken() != "integer" && token != "boolean")
		errorCatch("illegal type follows \":\"");
	y = token;
	if(NextToken() != ";")
		errorCatch("semicolon expected");
	Insert(x, WhichType(y),VARIABLE,"1",YES,1);
	y = NextToken();
	if (y != "begin" && isKeyword(y))
		errorCatch("non-keyword identifier or \"begin\" expected");
	if (!isKeyword(token))
		VarStmts();
}

void BeginEndStmt() //token should be "begin"
{
	depthCounter++;
	if (token != "begin")
		errorCatch("keyword \"begin\" expected");
	if (NextToken() != "end")
		EXEC_STMTS();
	if (token != "end")
		errorCatch("keyword \"end\" expected");
	else if (depthCounter == 0)
		Code(token);
	NextToken();
	if (depthCounter == 0 && token != ".")
		errorCatch("period expected when terminating outermost BEGIN_END block");
	else if (depthCounter > 0 && token != ";")
		errorCatch("semicolon expected when terminating interior BEGIN_END blocks");
	depthCounter--;
	NextToken();
}

void EXEC_STMTS() //token should be NON_KEY_ID, "read", or "write"
{
	if (token != "read" && token != "write" && token != "if" && token != "while" && token != "begin" && token != "repeat" && token != "else" && token != "until" && token != ";" && isKeyword(token))
		errorCatch("non-keyword identifier, \"read\", or \"write\" expected");
	EXEC_STMT();
	if (token != "end" && token != "until")
	{
		EXEC_STMTS();
	}
}

void EXEC_STMT() //token should be NON_KEY_ID, "read", or "write"
{
	// cout << token << endl;
	
	if (token == "read")
	{
		READ_STMT();
		NextToken();
	}
	else if (token == "write")
	{
		WRITE_STMT();
		NextToken();
	}
	else if (token == "if")
	{
		IF_STMT();
	}
	else if (token == "while")
	{
		WHILE_STMT();
	}
	else if (token == "repeat")
		REPEAT_STMT();
	else if (token == ";")
		NULL_STMT();
	else if (!isKeyword(token))
	{
		ASSIGN_STMT();
		NextToken();
	}
	else if (token == "begin")
	{
		BeginEndStmt();
	}
	else
		errorCatch("non-keyword identifier,  \";\", \"read\", \"write\", \"if\"  \"while\", \"repeat\", or \"begin\" expected to begin an executable statement");
}

void READ_STMT() //token should be "read"
{
	uint i = 0;
	string x, name, newName;
	bool varFound = false;
	if (token != "read")
		errorCatch("\"read\" expected");
	x = READ_LIST();
	NextToken();
	for (uint j = 0; j < x.length(); j++)
	{
		if(x[j] != ',')
		{
			name += x[j];
		}
		else if(x[j] == ',')
		{
			if (symbolTable.size() != 0)
			{		
				while (i < symbolTable.size())
				{
					newName = symbolTable[i].externalName;
					if (name == newName)
					{
						varFound = true;
						name = symbolTable[i].internalName;
						break;
					}
					i++;
					if (i == symbolTable.size() && !varFound)
					{
						errorCatch("undefined variable used in operation \"read\"");
					}
				}
			}
			else
			{
				errorCatch("undefined variable used in operation \"read\"");
			}
			Code("read", name);
			name = "";
		}
	}
	if (symbolTable.size() != 0)
	{		
		while (i < symbolTable.size())
		{
			newName = symbolTable[i].externalName;
			if (name == newName)
			{
				varFound = true;
				name = symbolTable[i].internalName;
				break;
			}
			i++;
			if (i == symbolTable.size() && !varFound)
			{
				errorCatch("undefined variable used in operation \"read\"");
			}
		}
	}
	else
	{
		errorCatch("undefined variable used in operation \"read\"");
	}
	Code("read", name);
	if (token != ";")
		errorCatch("semicolon expected");
}

string READ_LIST()
{
	string varsToRead;
	if (NextToken() != "(")
		errorCatch("'(' expected after \"read\"");
	NextToken();
	varsToRead = Ids();
	if (token != ")")
		errorCatch("')' expected after list of read variables");
	return varsToRead;
}

void WRITE_STMT() //token should be "write"
{
	uint i = 0;
	string x, name, newName;
	bool varFound = false;
	if (token != "write")
		errorCatch("\"write\" expected");
	x = WRITE_LIST();
	for (uint j = 0; j < x.length(); j++)
	{
		if(x[j] != ',')
		{
			name += x[j];
		}
		else if(x[j] == ',')
		{
			if (symbolTable.size() != 0)
			{		
				while (i < symbolTable.size())
				{
					newName = symbolTable[i].externalName;
					if (name == newName)
					{
						varFound = true;
						name = symbolTable[i].internalName;
						break;
					}
					i++;
					if (i == symbolTable.size() && !varFound)
					{
						errorCatch("undefined variable used in operation \"write\"");
					}
				}
			}
			else
			{
				errorCatch("undefined variable used in operation \"write\"");
			}
			Code("write", name);
			name = "";
		}	
	}
	if (symbolTable.size() != 0)
	{		
		while (i < symbolTable.size())
		{
			newName = symbolTable[i].externalName;
			if (name == newName)
			{
				varFound = true;
				name = symbolTable[i].internalName;
				break;
			}
			i++;
			if (i == symbolTable.size() && !varFound)
			{
				errorCatch("undefined variable used in operation \"write\"");
			}
		}
	}
	else
	{
		errorCatch("undefined variable used in operation \"write\"");
	}
	Code("write", name);
	name = "";
	NextToken();
	if (token != ";")
		errorCatch("semicolon expected");
}

string WRITE_LIST()
{
	string varsToWrite;
	if (NextToken() != "(")
		errorCatch("'(' expected after \"write\"");
	NextToken();
	varsToWrite = Ids();
	if (token != ")")
		errorCatch("')' expected after list of write variables");
	return varsToWrite;
}

void ASSIGN_STMT() //token should be NON_KEY_ID
{
	string x = "", newName = "";
	uint i = 0;
	bool varFound = false;
	string c = "", o1 = "", o2 = "";
	if (isKeyword(token))
	{
		errorCatch("non-keyword identifier expected");
	}
	x = token;
	if (symbolTable.size() != 0)
	{		
		while (i < symbolTable.size())
		{
			newName = symbolTable[i].externalName;
			if (x == newName)
			{
				varFound = true;
				x = symbolTable[i].internalName;
				break;
			}
			i++;
			if (i == symbolTable.size() && !varFound)
			{
				errorCatch("undefined variable used in operation");
			}
		}
	}
	else
	{
		errorCatch("undefined variable used in operation");
	}
	PushOperand(x);
	if (NextToken() != ":=")
		errorCatch("\":=\" expected");
	PushOperator(token);
	NextToken();
	EXPRESS();
	c = PopOperator();
	o1 = PopOperand();
	o2 = PopOperand();
	Code(c, o1, o2);
	if (token != ";")
		errorCatch("semicolon expected");
}

void EXPRESS() //token should be "not", "true", "false", "(", "+", "-", INTEGER, or NON_KEY_ID
{
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && WhichType(token) != INTEGER && isKeyword(token))
		errorCatch("expected \"not\", \"true\", \"false\", '(', '+', '-', non-keyword identifier, or an integer after operator");
	TERM();
	if (token != "<>" && token != "=" && token != "<=" && token != ">=" && token != "<" && token != ">" && token != ")" && token != ";" && token != "then" && token != "do")
		errorCatch("expected relational operator or semicolon after operand");
	EXPRESSES();
}

void EXPRESSES()
{
	if (token != "<>" && token != "=" && token != "<=" && token != ">=" && token != "<" 
	    && token != ">" && token != ")" && token != ";" && token != "then" && token != "do")
		errorCatch("expected relational operator or semicolon after operand");
	if (token == "<>" || token == "=" || token == "<=" || token == ">=" || token == "<" || token == ">")
		REL_OP();
	else
		return;
	NextToken();
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && WhichType(token) != INTEGER && isKeyword(token))
		errorCatch("expected \"not\", \"true\", \"false\", '(', '+', '-', non-keyword identifier, or an integer after operator");
	TERM();
	Code(PopOperator(), PopOperand(), PopOperand());
	if (token != "<>" && token != "=" && token != "<=" && token != ">=" && token != "<" 
	    && token != ">" && token != ")" && token != ";" && token != "then" && token != "do")
	{
		errorCatch("expected relational operator or semicolon after operand");
	}
	EXPRESSES();
}

void TERM() //token should be "not", "true", "false", "(", "+", "-", INTEGER, or NON_KEY_ID
{
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && WhichType(token) != INTEGER && isKeyword(token))
		errorCatch("expected \"not\", \"true\", \"false\", '(', '+', '-', non-keyword identifier, or an integer after operator");
	FACTOR();
	if (token != "-" && token != "+" && token != "or" && token != "<>" && token != "=" && token != "<=" && token != ">=" && token != "<" 
	    && token != ">" && token != ")" && token != ";" && token != "then" && token != "do")
	{
		errorCatch("expected additive or relational operator or semicolon after operand");
	}
	TERMS();	
}

void TERMS() //token should be an operator or semicolon
{
	string c = "", o1 = "", o2 = "";
	if (token != "-" && token != "+" && token != "or" && token != "<>" && token != "=" && token != "<=" && token != ">=" && token != "<" 
	    && token != ">" && token != ")" && token != ";" && token != "then" && token != "do")
		errorCatch("expected additive or relational operator or semicolon after operand");
	if (token == "+" || token == "-" || token == "or")
		ADD_LEVEL_OP();
	else
		return;
	NextToken();
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && WhichType(token) != INTEGER && isKeyword(token))
		errorCatch("expected \"not\", \"true\", \"false\", '(', '+', '-', non-keyword identifier, or an integer after operator");
	FACTOR();
	c = PopOperator();
	o1 = PopOperand();
	o2 = PopOperand();
	Code(c, o1, o2);
	if (token != "-" && token != "+" && token != "or" && token != "<>" && token != "=" && token != "<=" && token != ">=" && token != "<" 
	    && token != ">" && token != ")" && token != ";" && token != "then" && token != "do")
		errorCatch("expected additive or relational operator or semicolon after operand");
	TERMS();
}

void FACTOR() //token should be "not", "true", "false", "(", "+", "-", INTEGER, or NON_KEY_ID
{
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && WhichType(token) != INTEGER && isKeyword(token))
		errorCatch("expected \"not\", \"true\", \"false\", '(', '+', '-', non-keyword identifier, or an integer after operator");
	PART();
	NextToken();
	if (token != "*" && token != "div" && token != "mod" && token != "and" && token != "<>" && token != "=" && token != "<=" && token != ">=" 
	    && token != "<" && token != ">" && token != ")" && token != ";" && token != "-" && token != "+" && token != "or" && token != "then" && token != "do")
		errorCatch("expected multiplicative, additive, or relational operator or semicolon after operand");
	FACTORS();
	return;
}

void FACTORS() //token should be "*", "div", "mod", "and" to operate or "<>, =, <=, >=, <, >, ), ;, -, +, or" to epsilon move up
{
	string c = "", o1 = "", o2 = "";
	if (token != "*" && token != "div" && token != "mod" && token != "and" && token != "<>" && token != "=" && token != "<=" && token != ">=" 
	    && token != "<" && token != ">" && token != ")" && token != ";" && token != "-" && token != "+" && token != "or" && token != "then" && token != "do")
		errorCatch("expected multiplicative, additive, or relational operator or semicolon after operand");
	if (token == "*" || token == "div" || token == "mod" || token == "and")
		MULT_LEVEL_OP();
	else
		return;
	NextToken();
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && WhichType(token) != INTEGER && isKeyword(token))
		errorCatch("expected \"not\", \"true\", \"false\", '(', '+', '-', non-keyword identifier, or an integer after operator");
	PART();
	c = PopOperator();
	o1 = PopOperand();
	o2 = PopOperand();
	Code(c, o1, o2);
	NextToken();
	if (token != "*" && token != "div" && token != "mod" && token != "and" && token != "<>" && token != "=" && token != "<=" && token != ">=" 
	    && token != "<" && token != ">" && token != ")" && token != ";" && token != "-" && token != "+" && token != "or" && token != "then" && token != "do")
		{
			errorCatch("expected multiplicative, additive, or relational operator or semicolon after operand");
		}
	FACTORS();
}

void PART() //token should be "not", "true", "false", "(", "+", "-", INTEGER, or NON_KEY_ID
{
	uint i = 0;
	bool varFound = false;
	string x = "", newName = "", unaryOperand = "", temp = "";
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && WhichType(token) != INTEGER && isKeyword(token))
		errorCatch("expected \"not\", \"true\", \"false\", '(', '+', '-', non-keyword identifier, or an integer after operator");
	
	if (token == "not")
	{
		NextToken();
		unaryOperand = token;
		if(token == "(")
		{
			NextToken();
			EXPRESS();
			unaryOperand = PopOperand();
		}
		
		if (isBoolean(unaryOperand))
		{
			if (unaryOperand == "false")
			{
				if (symbolTable.size() != 0)
				{	
					i = 0;
					while (i < symbolTable.size())
					{
						if ("1" == symbolTable[i].value && symbolTable[i].dataType == BOOLEAN && symbolTable[i].mode == CONSTANT)
						{
							varFound = true;
							x = symbolTable[i].internalName;
							break;
						}
						i++;
						if (i == symbolTable.size() && !varFound)
						{
								Insert("true", BOOLEAN, CONSTANT, "1", YES, 1);
								x = symbolTable[symbolTable.size()-1].internalName;
						}
					}
				}
			}
			
			else if (unaryOperand == "true")
			{
				if (symbolTable.size() != 0)
				{	
					i = 0;
					while (i < symbolTable.size())
					{
						if ("0" == symbolTable[i].value && symbolTable[i].dataType == BOOLEAN && symbolTable[i].mode == CONSTANT)
						{
							varFound = true;
							x = symbolTable[i].internalName;
							break;
						}
						i++;
						if (i == symbolTable.size() && !varFound)
						{
							Insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
							x = symbolTable[symbolTable.size()-1].internalName;
						}
					}
				}
			}
			PushOperand(x);
		}
		
		else if (!isKeyword(unaryOperand))
		{
			x = unaryOperand;
			if (symbolTable.size() != 0)
			{		
				while (i < symbolTable.size())
				{
					newName = symbolTable[i].externalName;
					if (x == newName)
					{
						varFound = true;
						x = symbolTable[i].internalName;
						break;
					}
					i++;
					if (i == symbolTable.size() && !varFound)
					{
						errorCatch("undefined variable used in operation");
					}
				}
			}
			else
			{
				errorCatch("undefined variable used in operation");
			}
			
			PushOperand(x);
			Code("not", PopOperand());
		}
	}
		
	else if (token == "+")
	{
		NextToken();
		unaryOperand = token;
		if (token == "(")
		{
			NextToken();
			EXPRESS();
			unaryOperand = PopOperand();
		}
		
		if (isInteger(unaryOperand))
		{
			if (symbolTable.size() != 0)
			{	
				i = 0;	
				while (i < symbolTable.size())
				{
					if (unaryOperand == symbolTable[i].value && symbolTable[i].dataType == INTEGER && symbolTable[i].mode == CONSTANT)
					{
						varFound = true;
						x = symbolTable[i].internalName;
						break;
					}
					i++;
					if (i == symbolTable.size() && !varFound)
					{
						Insert(unaryOperand, INTEGER, CONSTANT, unaryOperand, YES, 1);
						x = symbolTable[(symbolTable.size() - 1)].internalName;
					}
				}
			}
			
			PushOperand(x);
		}
		
		else if (!isKeyword(unaryOperand))
		{
			x = unaryOperand;
			if (symbolTable.size() != 0)
			{		
				while (i < symbolTable.size())
				{
					newName = symbolTable[i].externalName;
					if (x == newName)
					{
						varFound = true;
						x = symbolTable[i].internalName;
						break;
					}
					i++;
					if (i == symbolTable.size() && !varFound)
					{
						errorCatch("undefined variable used in operation");
					}
				}
			}
			else
			{
				errorCatch("undefined variable used in operation");
			}
			PushOperand(x);
		}
	}
		/*
			expect ( EXPRESS )
					INTEGER
				or	NON_KEY_ID
		*/
	else if (token == "-")
	{
		NextToken();
		unaryOperand = token;
		if (token == "(")
		{
			NextToken();
			EXPRESS();
			unaryOperand = symbolTable[getTablePos(PopOperand())].value;
			PushOperand(unaryOperand);
		}
		else if (isInteger(unaryOperand))
		{
			
			x = "-";
			x += unaryOperand;
			if (symbolTable.size() != 0)
			{		
				i = 0;
				while (i < symbolTable.size())
				{
					if (x == symbolTable[i].value && symbolTable[i].dataType == INTEGER && symbolTable[i].mode == CONSTANT)
					{
						varFound = true;
						x = symbolTable[i].internalName;
						break;
					}
					i++;
				}
				if (i == symbolTable.size() && !varFound)
				{
					Insert(x, INTEGER, CONSTANT, x, YES, 1);
					temp = x;
					x = symbolTable[(symbolTable.size() - 1)].internalName;
				}
			}
			PushOperand(x);
		}
		
		else if (!isKeyword(unaryOperand))
		{
			x = unaryOperand;
			if (symbolTable.size() != 0)
			{		
				while (i < symbolTable.size())
				{
					newName = symbolTable[i].externalName;
					if (x == newName)
					{
						varFound = true;
						x = symbolTable[i].internalName;
						break;
					}
					i++;
					if (i == symbolTable.size() && !varFound)
					{
						errorCatch("undefined variable used in operation");
					}
				}
			}
			else
			{
				errorCatch("undefined variable used in operation");
			}
			PushOperand(x);
			Code("neg", PopOperand());
		}
	}
		
	else if (token == "(")
	{
		NextToken();
		EXPRESS();
	}
	
	else if (isInteger(token))
	{
		i = 0;
		if (symbolTable.size() != 0)
		{		
			while (i < symbolTable.size())
			{
				if (token == symbolTable[i].value && symbolTable[i].dataType == INTEGER && symbolTable[i].mode == CONSTANT)
				{
					varFound = true;
					x = symbolTable[i].internalName;
					break;
				}
				i++;
				if (i == symbolTable.size() && !varFound)
				{
					Insert(token, INTEGER, CONSTANT, token, YES, 1);
					x = symbolTable[getTablePos(token)].internalName;
				}
			}
		}
		PushOperand(x);
	}
	
	else if (isBoolean(token))
	{
		if (token == "true")
		{
			if (symbolTable.size() != 0)
			{	
				i = 0;
				while (i < symbolTable.size())
				{
					if (symbolTable[i].value == "1" && symbolTable[i].dataType == BOOLEAN && symbolTable[i].mode == CONSTANT)
					{
						varFound = true;
						x = symbolTable[i].internalName;
						break;
					}
					i++;
					if (i == symbolTable.size() && !varFound)
					{
						Insert("true", BOOLEAN, CONSTANT, "1", YES, 1);
						x = symbolTable[symbolTable.size()-1].internalName;
					}
				}
			}
		}
		
		else if (token == "false")
		{
			if (symbolTable.size() != 0)
			{	
				i = 0;
				while (i < symbolTable.size())
				{
					if (symbolTable[i].value == "0" && symbolTable[i].dataType == BOOLEAN && symbolTable[i].mode == CONSTANT)
					{
						varFound = true;
						x = symbolTable[i].internalName;
						break;
					}
					i++;
					if (i == symbolTable.size() && !varFound)
					{
						Insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
						x = symbolTable[symbolTable.size()-1].internalName;
					}
				}
			}
		}
		PushOperand(x);
	}
	else if (!isKeyword(token))
	{
		x = token;
		if (symbolTable.size() != 0)
		{		
			while (i < symbolTable.size())
			{
				newName = symbolTable[i].externalName;
				if (x == newName)
				{
					varFound = true;
					x = symbolTable[i].internalName;
					break;
				}
				i++;
				if (i == symbolTable.size() && !varFound)
				{
					errorCatch("undefined variable used in operation");
				}
			}
		}
		else
		{
			errorCatch("undefined variable used in operation");
		}
		PushOperand(x);
	}
}

void MULT_LEVEL_OP()
{
	PushOperator(token);
}

void ADD_LEVEL_OP()
{
	PushOperator(token);
}

void REL_OP()
{
	PushOperator(token);
}

void IF_STMT() //token should be "if"
{
	if (token != "if")
	{
		errorCatch("Compiler Error:  \"if\" expected inside of IF_STMT");
	}
	
	// cout << token << endl;
	NextToken();
	EXPRESS();
	// cout << token << endl;
	
	if (token != "then")
	{
		errorCatch("keyword \"then\" expected to follow an \"if\" test");
	}
	
	Code("then", PopOperand());
	NextToken();
	EXEC_STMT();
	ELSE_PT();
}

void ELSE_PT()
{
	if (token == "else")
	{
		Code("else", PopOperand());
		NextToken();
		EXEC_STMT();	
	}
	
	EmitPostIfCode(PopOperand());
}

void WHILE_STMT()
{
	if (token != "while")
		errorCatch("Compiler Error: expected \"while\" inside of WHILE_STMT");
	Code("while");
	NextToken();
	EXPRESS();
	if (token != "do")
	{
		errorCatch("keyword \"do\" expected to follow a \"while\" test");
	}
	
	Code("do", PopOperand());
	NextToken();
	EXEC_STMT();
	
	EmitPostWhileCode(PopOperand(), PopOperand());
}

void REPEAT_STMT()
{
	if (token != "repeat")
		errorCatch("Compiler Error: expected \"repeat\" inside of REPEAT_STMT");
	Code("repeat");
	NextToken();
	EXEC_STMTS();
	if (token != "until")
	{
		errorCatch("keyword \"until\" expected to follow a \"repeat\" test");
	}
	
	NextToken();
	EXPRESS();
	Code("until", PopOperand(), PopOperand());
	
	if (token != ";")
		errorCatch("semicolon expected");
}

void NULL_STMT()
{
	NextToken();
}


string Ids() //token should be NON_KEY_ID
{
	string temp,tempString;
	if (isKeyword(token))
		errorCatch("non-keyword identifier expected");
	tempString = token;
	temp = token;
	if(NextToken() == ",")
	{
		NextToken();
		if (isKeyword(token))
			errorCatch("non-keyword identifier expected");
		tempString = temp + "," + Ids();
	}
	return tempString;
}

void Insert(string externalName,storeType inType, modes inMode, string inValue, allocation inAlloc, int inUnits)
 //create symbol table entry for each identifier in list of external names
 //Multiply inserted names are illegal
{
	string name;
	string newName;
	uint i=0;
	if (externalName != "true" && externalName != "false" && isKeyword(externalName))
		errorCatch("name cannot be defined as a key word");
		
	for (uint j = 0; j < externalName.length(); j++)
	{
		if(externalName[j] != ',')
		{
			name += externalName[j];
		}
		else if(externalName[j] == ',')
		{
			Insert(name, inType, inMode, inValue, inAlloc, inUnits);
			name = "";
		}
	}
	if (symbolTable.size() != 0)
	{		
		while (i < symbolTable.size())
		{
			newName = symbolTable[i].externalName;
			if (externalName == newName)
			{
				errorCatch("multiple name definition");
			}
			i++;
		}
	}
	//create table entry
	entry newElement;
	if (isupper(externalName[0]))
	{
		newElement.internalName = name[0];
		if (name.size() >= 2)
		{
			newElement.internalName += name[1];
		}
		if (name.size() >= 3)
		{
			newElement.internalName += name[2];
		}
		if (name.size() >= 4)
		{
			newElement.internalName += name[3];
		}
	}
	else
	{
		newElement.internalName = GenInternalName(inType);
	}
	
	newElement.externalName = name;
	newElement.dataType = inType;
	newElement.mode = inMode;
	newElement.value = inValue;
	newElement.alloc = inAlloc;
	newElement.units = inUnits;
	
	symbolTable.push_back(newElement);
}

string GenInternalName(storeType type)
{
	string newName;
	
	if (type == INTEGER)
	{
		newName = "I";
		if (intCount <=9)
			newName += intCount + 48;
		else
		{
			newName += ((intCount /10) + 48);
			newName += ((intCount %10) + 48);
		}		
		
		intCount++;
	}
	
	else if (type == BOOLEAN)
	{
		newName = "B";
		if (boolCount <=9)
			newName += boolCount + 48;
		else
		{
			newName += ((boolCount /10) + 48);
			newName += ((boolCount %10) + 48);
		}		
		
		boolCount++;
	}
	
	else if (type == PROG_NAME)
	{
		newName = "P";
		if (intCount <=9)
			newName += progCount + 48;
		else
		{
			newName += ((progCount /10) + 48);
			newName += ((progCount %10) + 48);
		}		
		
		progCount++;
	}
	
	else if (type == UNKNOWN)
	{
		newName = GetTemp();
	}
	
	else
	{
		errorCatch("invalid type name");
	}
	
	return newName;
}

storeType WhichType(string name)
{
	if (isBoolean(name))
	{
		return BOOLEAN;
	}
	else if (isInteger(name))
	{
		return INTEGER;
	}
	else
	{
		for(uint itr = 0; itr < symbolTable.size(); ++itr)
		{
			if(name == symbolTable[itr].externalName)
			{
				return symbolTable[itr].dataType;
			}
			else if (name == symbolTable[itr].internalName)
			{
				return symbolTable[itr].dataType;
			}
		}
		
		errorCatch("Reference to undefined constant");
	}
	return BOOLEAN; //default never caught due to error-forced exit
}

string WhichValue(string name)
{
	if (isInteger(name))
	{
		return name;
	}
	else if (isBoolean(name))
	{
		return (name == "true" ? "1" : "0");
	}
	else
	{
		for(uint itr = 0; itr < symbolTable.size(); ++itr)
		{
			if(name == symbolTable[itr].externalName)
			{
				return symbolTable[itr].value;
			}
		}
		errorCatch("Reference to undefined constant");
	}
	return "foo bar"; //default never caught due to error-forced exit
}

bool isInteger(string maybeInt)
{
	if (maybeInt == "integer") return true;
	uint i = 0;
	if (maybeInt[0]=='-' || maybeInt[0]=='+' || maybeInt[0] == 'I')
		i++;
	while (i < maybeInt.length())
	{
		if(!isdigit(maybeInt[i]))
			return false;
		i++;
	}
	
	return true;
}

bool isBoolean(string maybeBool)
{
	if(maybeBool == "true" || maybeBool == "false" || maybeBool == "boolean")
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool isKeyword(string maybeID)
{
	if (maybeID == "program" || maybeID == "const" || maybeID == "var" || maybeID == "integer" 
	    || maybeID == "boolean" || maybeID == "begin" || maybeID == "end" || maybeID == "true"
		|| maybeID == "false" || maybeID == "not" || maybeID == "mod" || maybeID == "div"
		|| maybeID == "and" || maybeID == "or" || maybeID == "read" || maybeID == "write"
		|| maybeID == "if" || maybeID == "then" || maybeID == "else" || maybeID == "repeat"
		|| maybeID == "while" || maybeID == "do" || maybeID == "until")
	{
		return true;	
	}
	
	return false;
}

bool isTemp(string maybeTemp)
{
	if(maybeTemp[0] == 'T' && isdigit(maybeTemp[1]))
		return true;
	return false;
}

void PrintSymbolTable()
{
	bool minusPopped = false;
	bool printed = false;
	for(uint i = 1; i < symbolTable.size(); i++)
	{
		printed = false;
		if (!(isTemp(symbolTable[i].externalName) && symbolTable[i].alloc == NO))
		{
			objectFile << left << setw(6) << symbolTable[i].internalName << setw(4) << (symbolTable[i].mode == 1 ? "DEC" : "BSS");
			for (uint j = 4; j > 0; j--)
			{
				if (!printed)
				{
					if (!minusPopped)
					{
						switch (j)
						{
							case 4:		{
											if (symbolTable[i].value[0] == '-')
											{
												objectFile << "-";
												minusPopped = true;
											}
											else if (symbolTable[i].value.size() == 4)
											{
												objectFile << symbolTable[i].value[0] << symbolTable[i].value[1] << symbolTable[i].value[2] << symbolTable[i].value[3];
												printed = true;
											}
											else
												objectFile << "0";
											break;
										}
								
							case 3: 	{
											if ( symbolTable[i].value.size() == 3)
											{
												objectFile << symbolTable[i].value[0] << symbolTable[i].value[1] << symbolTable[i].value[2];
												printed = true;
											}
											else 
												objectFile << "0";
											break;
										}				
							case 2: 	{
											if ( symbolTable[i].value.size() == 2)
											{
												objectFile << symbolTable[i].value[0] << symbolTable[i].value[1];
												printed = true;
											}
											else 
												objectFile << "0";
											break;
										}
							case 1:     {
											if ( symbolTable[i].value.size() == 1)
											{
												objectFile << symbolTable[i].value[0];
												printed = true;
											}
											else 
												objectFile << "0";
											break;
										}
							default:	{
											printed = true;
											break;
										}
						}
					}
					else 
					{
						switch (j-1)
						{
							case 3: 	{
											if ( symbolTable[i].value.size() == 4)
											{
												objectFile << "0" << symbolTable[i].value[1] << symbolTable[i].value[2] << symbolTable[i].value[3];
												printed = true;
											}
											else 
												objectFile << "0";
											break;
										}				
							case 2: 	{
											if ( symbolTable[i].value.size() == 3)
											{
												objectFile << "0" << symbolTable[i].value[1] << symbolTable[i].value[2];
												printed = true;
											}
											else 
												objectFile << "0";
											break;
										}
							case 1:     {
											if ( symbolTable[i].value.size() == 2)
											{
												objectFile << "0" << symbolTable[i].value[1];
												printed = true;
											}
											else 
												objectFile << "0";
											break;
										}
							case 0:   	{
											if ( symbolTable[i].value.size() == 1)
											{
												objectFile << "0";
												printed = true;
											}
											else 
												objectFile << "0";
											break;
										}
							default:	{
											printed = true;
											break;
										}
						}
					}
				}
			}
			objectFile << setfill(' ') << "     " << symbolTable[i].externalName << endl;
			if (minusPopped)
			{
				minusPopped = false;
			}
		}
	}
	
}

int getTablePos(string InternalName)
{
	for (uint i = 0; i < symbolTable.size(); i++)
	{
		if (symbolTable[i].internalName == InternalName)
		{
			return i;
		}
	}
	return 0;
}

void Code(string operatorio, string operand1, string operand2)
{
	if (operatorio == "program")
		objectFile << "STRT  NOP          " << operand1 << " - R. Jones & D. Parrish\n";
	else if (operatorio == "end")
	{
		objectFile << "      HLT          \n";
		PrintSymbolTable();
		objectFile << "      END STRT     \n";
	}
	else if (operatorio == "read")
	{
		objectFile << "      RDI " << left << setw(9) << operand1 << "read(" << symbolTable[getTablePos(operand1)].externalName << ")\n";
	}
	else if (operatorio == "write")
	{
		objectFile << "      PRI " << left << setw(9) << operand1 << "write(" << symbolTable[getTablePos(operand1)].externalName << ")\n";
	}
	else if (operatorio == "+")
	{
		EmitAdditionCode(operand1, operand2);
	}
	else if (operatorio == "-")
	{
		EmitSubtractionCode(operand1, operand2);
	}
	else if (operatorio == "neg")
	{
		EmitNegationCode(operand1);
	}
	else if (operatorio == "pos")
	{
		EmitSpanishInquisitionCode(operand1);  //unary plus ... because no one expects it.
	}
	else if (operatorio == "not")
	{
		EmitNotCode(operand1);
	}
	else if (operatorio == "div")
	{
		EmitDivisionCode(operand1, operand2);
	}
	else if (operatorio == "*")
	{
		EmitMultiplicationCode(operand1, operand2);
	}
	else if (operatorio == "mod")
	{
		EmitModuloCode(operand1, operand2);
	}
	else if (operatorio == "and")
	{
		EmitAndCode(operand1, operand2);
	}
	else if (operatorio == "or")
	{
		EmitOrCode(operand1, operand2);
	}
	else if (operatorio == "=")
	{
		EmitEqualsCode(operand1, operand2);
	}
	else if (operatorio == "<")
	{
		EmitLessThanCode(operand1, operand2);
	}
	else if (operatorio == "<=")
	{
		EmitLessThanEqualCode(operand1, operand2);
	}
	else if (operatorio == ">")
	{
		EmitGreaterThanCode(operand1, operand2);
	}
	else if (operatorio == ">=")
	{
		EmitGreaterThanEqualCode(operand1, operand2);
	}
	else if (operatorio == "<>")
	{
		EmitNotEqualCode(operand1, operand2);
	}
	else if (operatorio == ":=")
	{
		EmitAssignCode(operand1, operand2);
	}
	else if (operatorio == "then")
	{
		EmitThenCode(operand1);
	}
	else if (operatorio == "else")
	{
		EmitElseCode(operand1);
	}
	else if (operatorio == "while")
	{
		EmitWhileCode();
	}
	else if (operatorio == "do")
	{
		EmitDoCode(operand1);
	}
	else if (operatorio == "repeat")
	{
		EmitRepeatCode();
	}
	else if (operatorio == "until")
	{
		EmitUntilCode(operand1, operand2);
	}
	else
	{
		errorCatch("undefined operation");
	}
}

void EmitAdditionCode(string operand1,string operand2) //add operand1 to operand2
{
	if (WhichType(operand1) != INTEGER || WhichType(operand2) != INTEGER)
		errorCatch("illegal type");
	if (regContents != operand1 && regContents != operand2 && isTemp(regContents))
	{
		objectFile << "      STA " << left << setw(9) << regContents << "deassign AReg\n";
		symbolTable[getTablePos(regContents)].alloc = YES;
		symbolTable[getTablePos(regContents)].units = 1;
		regContents = "";
	}
	if (regContents != operand1 && regContents != operand2 && !isTemp(regContents))
		regContents = "";
	if (regContents != operand1 && regContents != operand2)
	{
		objectFile << "      LDA " << left << setw(9) << operand2 << '\n';
		regContents = operand2;
	}
	if (regContents == operand1)
	{
		objectFile << "      IAD " << left << setw(9) << operand2 << symbolTable[getTablePos(operand2)].externalName << " + "	<< symbolTable[getTablePos(operand1)].externalName << '\n';
	}
	else
	{
		objectFile << "      IAD " << left << setw(9) << operand1 << symbolTable[getTablePos(operand2)].externalName << " + "	<< symbolTable[getTablePos(operand1)].externalName << '\n';
	}
	
	if(isTemp(operand2))
		FreeTemp();
	if(isTemp(operand1))
		FreeTemp();
	//A Register = next available temporary name and change type of its symbol table entry to integer
	regContents = GetTemp();
	symbolTable[getTablePos(regContents)].dataType = INTEGER;
	//push the name of the result onto operandStk
	PushOperand(regContents);
}

void EmitSubtractionCode(string operand1,string operand2) //subtract operand1 from operand2 (op2 - op1)
{
	if ((WhichType(operand1) != INTEGER || WhichType(operand2) != INTEGER))
		errorCatch("illegal type");
	if (regContents != operand1 && regContents != operand2 && isTemp(regContents))
	{
		objectFile << "      STA " << left << setw(9) << regContents << "deassign AReg\n";
		symbolTable[getTablePos(regContents)].alloc = YES;
		symbolTable[getTablePos(regContents)].units = 1;
		regContents = "";
	}
	if (regContents != operand1 && regContents != operand2 && !isTemp(regContents))
		regContents = "";
	if (regContents == operand1)
	{
		objectFile << "      STA " << left << setw(9) << operand1 << "deassign AReg\n";
		symbolTable[getTablePos(regContents)].alloc = YES;
		symbolTable[getTablePos(regContents)].units = 1;
		regContents = "";
	}
	if (regContents != operand1 && regContents != operand2)
	{
		objectFile << "      LDA " << left << setw(9) << operand2 << '\n';
		regContents = operand2;
	}
	
	objectFile << "      ISB " << left << setw(9) << operand1 << symbolTable[getTablePos(operand2)].externalName << " - " << symbolTable[getTablePos(operand1)].externalName << '\n';
	if(isTemp(operand2))
		FreeTemp();
	if(isTemp(operand1))
		FreeTemp();
	//A Register = next available temporary name and change type of its symbol table entry to integer
	regContents = GetTemp();
	symbolTable[getTablePos(regContents)].dataType = INTEGER;
	//push the name of the result onto operandStk
	PushOperand(regContents);
}

void EmitNegationCode(string operand1) //negate operand1
{
	if (WhichType(operand1) != INTEGER)
		errorCatch("illegal type");
	if (regContents == operand1 && isTemp(regContents))
	{
		objectFile << "      STA " << left << setw(9) << regContents << "deassign AReg\n";
		symbolTable[getTablePos(regContents)].alloc = YES;
		symbolTable[getTablePos(regContents)].units = 1;
		regContents = "";
	}
	if (regContents != operand1 && !isTemp(regContents))
		regContents = "";
	objectFile << "      LDA ZERO\n";
	regContents = "ZERO";
	if (!zeroInTable)
		Insert("ZERO", INTEGER, CONSTANT, 0, YES, 1);
	objectFile << "      ISB " << left << setw(9) << operand1 << "- " << symbolTable[getTablePos(operand1)].externalName << '\n';
	if(isTemp(operand1))
		FreeTemp();
	//A Register = next available temporary name and change type of its symbol table entry to integer
	regContents = GetTemp();
	symbolTable[getTablePos(regContents)].dataType = INTEGER;
	//push the name of the result onto operandStk
	PushOperand(regContents);
}

void EmitSpanishInquisitionCode(string operand1) //operator unary plus
{
	if (WhichType(operand1) != INTEGER)
		errorCatch("illegal type");
}

void EmitNotCode(string operand1) //invert operand1
{
	if(WhichType(operand1) != BOOLEAN)
		errorCatch("illegal type");
	if (regContents != operand1)
		objectFile << "      LDA " << left << setw(9) << operand1 << '\n';
	string label = nextLabel();
	objectFile << "      AZJ " << left << setw(9) << label << "not " << symbolTable[getTablePos(operand1)].externalName << '\n';
	objectFile << "      LDA " << left << setw(9) << "FALS" << "\n";
	objectFile << "      UNJ " << left << setw(4) << label << setw(5) << "+1" << "\n";
	objectFile << left << setw(6) << label << "LDA " << setw(9) << "TRUE" << "\n";
	if(isTemp(operand1))
		FreeTemp();
	regContents = GetTemp();
	if(!falseInTable)
	{
		falseInTable = true;
		Insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);
	}
	if(!trueInTable)
	{
		trueInTable = true;
		Insert("TRUE", BOOLEAN, CONSTANT, "1", YES, 1);
	}
	
	
	symbolTable[getTablePos(regContents)].dataType = BOOLEAN;
	//push the name of the result onto operandStk
	PushOperand(regContents);
}

void EmitMultiplicationCode(string operand1,string operand2) //multiply operand2 by operand1
{
 if ((WhichType(operand1) != INTEGER || WhichType(operand2) != INTEGER))
	errorCatch("illegal type");
 if (regContents != operand1 && regContents != operand2 && isTemp(regContents))
	{
		objectFile << "      STA " << left << setw(9) << regContents << "deassign AReg\n";
		symbolTable[getTablePos(regContents)].alloc = YES;
		symbolTable[getTablePos(regContents)].units = 1;
		regContents = "";
	}
 if (regContents != operand1 && regContents != operand2 && !isTemp(regContents))
		regContents = "";
 if (regContents != operand1 && regContents != operand2)
	{
		objectFile << "      LDA " << left << setw(9) << operand2 << '\n';
		regContents = operand2;
	}
 if (regContents == operand1)
 {
	 objectFile << "      IMU " << left << setw(9) << operand2 << symbolTable[getTablePos(operand2)].externalName << " * " << symbolTable[getTablePos(operand1)].externalName << '\n';
 }
 else
 {
	objectFile << "      IMU " << left << setw(9) << operand1 << symbolTable[getTablePos(operand2)].externalName << " * " << symbolTable[getTablePos(operand1)].externalName << '\n';
 }
 if(isTemp(operand2))
	FreeTemp();
 if(isTemp(operand1))
	FreeTemp();
 //A Register = next available temporary name and change type of its symbol table entry to integer
 regContents = GetTemp();
 symbolTable[getTablePos(regContents)].dataType = INTEGER;
 //push the name of the result onto operandStk
 PushOperand(regContents);
}

void EmitDivisionCode(string operand1,string operand2) //divide operand2 by operand1
{
 if (WhichType(operand1) != INTEGER || WhichType(operand2) != INTEGER)
	errorCatch("illegal type");
 if (regContents != operand1 && regContents != operand2 && isTemp(regContents))
	{
		objectFile << "      STA " << left << setw(9) << regContents << "deassign AReg\n";
		symbolTable[getTablePos(regContents)].alloc = YES;
		symbolTable[getTablePos(regContents)].units = 1;
		regContents = "";
	}
 if (regContents != operand1 && regContents != operand2 && !isTemp(regContents))
		regContents = "";
	
 if (regContents == operand1)
	{
		objectFile << "      STA " << left << setw(9) << operand1 << "deassign AReg\n";
		symbolTable[getTablePos(regContents)].alloc = YES;
		symbolTable[getTablePos(regContents)].units = 1;
		regContents = "";
	}
 if (regContents != operand1 && regContents != operand2)
	{
		objectFile << "      LDA " << left << setw(9) << operand2 << '\n';
		regContents = operand2;
	}
 objectFile << "      IDV " << left << setw(9) << operand1 << symbolTable[getTablePos(operand2)].externalName << " div " << symbolTable[getTablePos(operand1)].externalName << '\n';
 if(isTemp(operand2))
	FreeTemp();
 if(isTemp(operand1))
	FreeTemp();
 //A Register = next available temporary name and change type of its symbol table entry to integer
 regContents = GetTemp();
 symbolTable[getTablePos(regContents)].dataType = INTEGER;
 //push the name of the result onto operandStk
 PushOperand(regContents);
}

void EmitModuloCode(string operand1, string operand2)
{
	if (WhichType(operand1) != INTEGER || WhichType(operand2) != INTEGER)
	{
		errorCatch("illegal type");
	}
	if (regContents != operand1 && regContents != operand2 && isTemp(regContents))
	{
		objectFile << "      STA " << left << setw(9) << regContents << "deassign AReg\n";
		symbolTable[getTablePos(regContents)].alloc = YES;
		symbolTable[getTablePos(regContents)].units = 1;
		regContents = "";
	}
	if (regContents != operand1 && regContents != operand2 && !isTemp(regContents))
			regContents = "";
	// if (regContents == operand1)
	// {
		// objectFile << "      STA " << left << setw(9) << operand1 << "\n";
		// regContents = "";
	// }
	if (regContents != operand2)
	{
		objectFile << "      LDA " << left << setw(9) << operand2 << '\n';
		regContents = operand2;
	}
	objectFile << "      IDV " << left << setw(9) << operand1 << symbolTable[getTablePos(operand2)].externalName << " mod " << symbolTable[getTablePos(operand1)].externalName << '\n';
	if(isTemp(operand2))
		FreeTemp();
	if(isTemp(operand1))
		FreeTemp();
	//A Register = next available temporary name and change type of its symbol table entry to integer
	regContents = GetTemp();
	symbolTable[getTablePos(regContents)].dataType = INTEGER;
	objectFile << "      STQ " << regContents << "       store remainder in memory\n";
	symbolTable[getTablePos(regContents)].alloc = YES;
	symbolTable[getTablePos(regContents)].units = 1;
	objectFile << "      LDA " << regContents << "       load remainder from memory\n";
	//push the name of the result onto operandStk
	PushOperand(regContents);
}

void EmitAndCode(string operand1,string operand2) //"and" operand1 to operand2
{
 if (WhichType(operand1) != BOOLEAN || WhichType(operand1) != BOOLEAN)
	errorCatch("illegal type");
 if (regContents != operand1 && regContents != operand2 && isTemp(regContents))
	{
		objectFile << "      STA " << left << setw(9) << regContents << "deassign AReg\n";
		symbolTable[getTablePos(regContents)].alloc = YES;
		symbolTable[getTablePos(regContents)].units = 1;
		regContents = "";
	}
 if (regContents != operand1 && regContents != operand2 && !isTemp(regContents))
		regContents = "";
 if (regContents != operand1 && regContents != operand2)
	{
		objectFile << "      LDA " << left << setw(9) << operand2 << '\n';
		regContents = operand2;
	}
 if (regContents == operand1)
 {
	 objectFile << "      IMU " << left << setw(9) << operand2 << symbolTable[getTablePos(operand2)].externalName << " and " << symbolTable[getTablePos(operand1)].externalName << '\n';
 }
 else
 {
	objectFile << "      IMU " << left << setw(9) << operand1 << symbolTable[getTablePos(operand2)].externalName << " and " << symbolTable[getTablePos(operand1)].externalName << '\n';
 }
 if(isTemp(operand2))
	FreeTemp();
 if(isTemp(operand1))
	FreeTemp();
 //A Register = next available temporary name and change type of its symbol table entry to integer
 regContents = GetTemp();
 symbolTable[getTablePos(regContents)].dataType = BOOLEAN;
 //push the name of the result onto operandStk
 PushOperand(regContents);
}

void EmitOrCode(string operand1,string operand2) //"or" operand1 to operand2
{
 if (WhichType(operand1) != BOOLEAN || WhichType(operand1) != BOOLEAN)
	errorCatch("illegal type");
 if (regContents != operand1 && regContents != operand2 && isTemp(regContents))
	{
		objectFile << "      STA " << left << setw(9) << regContents << "deassign AReg\n";
		symbolTable[getTablePos(regContents)].alloc = YES;
		symbolTable[getTablePos(regContents)].units = 1;
		regContents = "";
	}
 if (regContents != operand1 && regContents != operand2 && !isTemp(regContents))
		regContents = "";
 if (regContents != operand1 && regContents != operand2)
	{
		objectFile << "      LDA " << left << setw(9) << operand2 << '\n';
		regContents = operand2;
	}
 if (regContents == operand1)
	{
		objectFile << "      IAD " << left << setw(9) << operand2 << symbolTable[getTablePos(operand2)].externalName << " or "	<< symbolTable[getTablePos(operand1)].externalName << '\n';
	}
 else
	{
		objectFile << "      IAD " << left << setw(9) << operand1 << symbolTable[getTablePos(operand2)].externalName << " or "	<< symbolTable[getTablePos(operand1)].externalName << '\n';
	}
	string label = nextLabel();
	objectFile << "      AZJ " << left << setw(4) << label << setw(5) << "+1" << '\n';
	objectFile << left << setw(6) << label << "LDA " << setw(9) << "TRUE" << "\n";
 //insert TRUE in symbol table with value 1 and external name TRUE
 if(!trueInTable)
 {
	 trueInTable = true;
	 Insert("TRUE", BOOLEAN, CONSTANT, "1", YES, 1);
 }
 //insert FALSE in symbol table with value 0 and external name FALSE
 if(!falseInTable)
 {
	 falseInTable = true;
	Insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);
 }
 //deassign all temporaries involved and free those names for reuse;
 if(isTemp(operand2))
	FreeTemp();
 if(isTemp(operand1))
	FreeTemp();
 //A Register = next available temporary name and change type of its symbol table entry to boolean
 regContents = GetTemp();
 symbolTable[getTablePos(regContents)].dataType = BOOLEAN;
 //push the name of the result onto operandStk
 PushOperand(regContents);
}

void EmitEqualsCode(string operand1,string operand2) //test whether operand2 equals operand1
{
 if (WhichType(operand1) != WhichType(operand2))
	errorCatch("incompatible types");
 if (regContents != operand1 && regContents != operand2 && isTemp(regContents))
	{
		objectFile << "      STA " << left << setw(9) << regContents << "deassign AReg\n";
		symbolTable[getTablePos(regContents)].alloc = YES;
		symbolTable[getTablePos(regContents)].units = 1;
		regContents = "";
	}
 if (regContents != operand1 && regContents != operand2 && !isTemp(regContents))
		regContents = "";
 if (regContents != operand1 && regContents != operand2)
	{
		objectFile << "      LDA " << left << setw(9) << operand2 << '\n';
		regContents = operand2;
	}
 //emit code to perform a register-memory subtraction with A Register holding the result;
  if (regContents == operand1)
 {
	 objectFile << "      ISB " << left << setw(9) << operand2 << symbolTable[getTablePos(operand2)].externalName << " = " << symbolTable[getTablePos(operand1)].externalName << '\n';
 }
 else
 {
	objectFile << "      ISB " << left << setw(9) << operand1 << symbolTable[getTablePos(operand2)].externalName << " = " << symbolTable[getTablePos(operand1)].externalName << '\n';
 }
 //emit code to perform an AZJ to the next available Ln
 string label = nextLabel();
 objectFile << "      AZJ " << left << setw(9) << label << '\n';
 //emit code to do a register-memory load FALS
 objectFile << "      LDA " << left << setw(9) << "FALS" << '\n';
 //insert FALS in symbol table with value 0 and external name false
 if(!falseInTable)
 {
	 falseInTable = true;
	Insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);
 }
 //emit code to perform a UNJ to the acquired label Ln +1
 objectFile << "      UNJ " << left << setw(4) << label << setw(5) << "+1" << '\n';
 //emit code to label the next instruction with the acquired label Ln
		//and do a register-memory load TRUE
 objectFile << left << setw(4) << label << "  LDA " << left << setw(9) << "TRUE" << '\n';
 //insert TRUE in symbol table with value 1 and external name true
 if(!trueInTable)
 {
	 trueInTable = true;
	 Insert("TRUE", BOOLEAN, CONSTANT, "1", YES, 1);
 }
 //deassign all temporaries involved and free those names for reuse;
 if(isTemp(operand2))
	FreeTemp();
 if(isTemp(operand1))
	FreeTemp();
 //A Register = next available temporary name and change type of its symbol table entry to boolean
 regContents = GetTemp();
 symbolTable[getTablePos(regContents)].dataType = BOOLEAN;
 //push the name of the result onto operandStk
 PushOperand(regContents);
}

void EmitLessThanCode(string operand1, string operand2)
{
 if (WhichType(operand1) != WhichType(operand2))
	errorCatch("incompatible types");
 if (regContents != operand1 && regContents != operand2 && isTemp(regContents))
	{
		objectFile << "      STA " << left << setw(9) << regContents << "deassign AReg\n";
		symbolTable[getTablePos(regContents)].alloc = YES;
		symbolTable[getTablePos(regContents)].units = 1;
		regContents = "";
	}
 if (regContents != operand1 && regContents != operand2 && !isTemp(regContents))
		regContents = "";
 if (regContents != operand1 && regContents != operand2)
	{
		objectFile << "      LDA " << left << setw(9) << operand2 << '\n';
		regContents = operand2;
	}
 //emit code to perform a register-memory subtraction with A Register holding the result;
 objectFile << "      ISB " << left << setw(9) << operand1 << symbolTable[getTablePos(operand2)].externalName << " < " << symbolTable[getTablePos(operand1)].externalName << '\n';
 //emit code to perform an AZJ to the next available Ln
 string label = nextLabel();
 objectFile << "      AMJ " << left << setw(9) << label << '\n';
 //emit code to do a register-memory load FALS
 objectFile << "      LDA " << left << setw(9) << "FALS" << "\n";
 if(!falseInTable)
 {
	 falseInTable = true;
	Insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);
 }
 //emit code to perform a UNJ to the acquired label Ln +1
 objectFile << "      UNJ " << left << setw(4) << label << "+1   \n";
 //emit code to label the next instruction with the acquired label Ln
		//and do a register-memory load TRUE
 objectFile << left << setw(4) << label << "  LDA " << left << setw(9) << "TRUE" << "\n";
 if(!trueInTable)
 {
	 trueInTable = true;
	 Insert("TRUE", BOOLEAN, CONSTANT, "1", YES, 1);
 }
 //deassign all temporaries involved and free those names for reuse;
 if(isTemp(operand2))
	FreeTemp();
 if(isTemp(operand1))
	FreeTemp();
 //A Register = next available temporary name and change type of its symbol table entry to boolean
 regContents = GetTemp();
 symbolTable[getTablePos(regContents)].dataType = BOOLEAN;
 //push the name of the result onto operandStk
 PushOperand(regContents);
}

void EmitLessThanEqualCode(string operand1, string operand2)
{
 if (WhichType(operand1) != WhichType(operand2))
	errorCatch("incompatible types");
 if (regContents != operand1 && regContents != operand2 && isTemp(regContents))
	{
		objectFile << "      STA " << left << setw(9) << regContents << "deassign AReg\n";
		symbolTable[getTablePos(regContents)].alloc = YES;
		symbolTable[getTablePos(regContents)].units = 1;
		regContents = "";
	}
 if (regContents != operand1 && regContents != operand2 && !isTemp(regContents))
		regContents = "";
 if (regContents != operand1 && regContents != operand2)
	{
		objectFile << "      LDA " << left << setw(9) << operand2 << '\n';
		regContents = operand2;
	}
 //emit code to perform a register-memory subtraction with A Register holding the result;
 objectFile << "      ISB " << left << setw(9) << operand1 << symbolTable[getTablePos(operand2)].externalName << " <= " << symbolTable[getTablePos(operand1)].externalName << '\n';
 //emit code to perform an AZJ to the next available Ln
 string label = nextLabel();
 objectFile << "      AMJ " << left << setw(9) << label << '\n';
 objectFile << "      AZJ " << left << setw(9) << label << '\n';
 //emit code to do a register-memory load FALS
 objectFile << "      LDA " << left << setw(9) << "FALS" << "\n";
 if(!falseInTable)
 {
	 falseInTable = true;
	Insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);
 }
 //emit code to perform a UNJ to the acquired label Ln +1
 objectFile << "      UNJ " << left << setw(4) << label << "+1   \n";
 //emit code to label the next instruction with the acquired label Ln
		//and do a register-memory load TRUE
 objectFile << left << setw(4) << label << "  LDA " << left << setw(9) << "TRUE" << "\n";
 if(!trueInTable)
 {
	 trueInTable = true;
	 Insert("TRUE", BOOLEAN, CONSTANT, "1", YES, 1);
 }
 //deassign all temporaries involved and free those names for reuse;
 if(isTemp(operand2))
	FreeTemp();
 if(isTemp(operand1))
	FreeTemp();
 //A Register = next available temporary name and change type of its symbol table entry to boolean
 regContents = GetTemp();
 symbolTable[getTablePos(regContents)].dataType = BOOLEAN;
 //push the name of the result onto operandStk
 PushOperand(regContents);
}

void EmitGreaterThanCode(string operand1, string operand2)
{
 if (WhichType(operand1) != WhichType(operand2))
	errorCatch("incompatible types");
 if (regContents != operand1 && regContents != operand2 && isTemp(regContents))
	{
		objectFile << "      STA " << left << setw(9) << regContents << "deassign AReg\n";
		symbolTable[getTablePos(regContents)].alloc = YES;
		symbolTable[getTablePos(regContents)].units = 1;
		regContents = "";
	}
 if (regContents != operand1 && regContents != operand2 && !isTemp(regContents))
		regContents = "";
 if (regContents != operand1 && regContents != operand2)
	{
		objectFile << "      LDA " << left << setw(9) << operand2 << '\n';
		regContents = operand2;
	}
 //emit code to perform a register-memory subtraction with A Register holding the result;
 objectFile << "      ISB " << left << setw(9) << operand1 << symbolTable[getTablePos(operand2)].externalName << " > " << symbolTable[getTablePos(operand1)].externalName << '\n';
 //emit code to perform an AZJ to the next available Ln
 string label = nextLabel();
 objectFile << "      AMJ " << left << setw(9) << label << '\n';
 objectFile << "      AZJ " << left << setw(9) << label << '\n';
 //emit code to do a register-memory load FALS
 objectFile << "      LDA " << left << setw(9) << "TRUE" << "\n";
 if(!trueInTable)
 {
	 trueInTable = true;
	 Insert("TRUE", BOOLEAN, CONSTANT, "1", YES, 1);
 }
 //emit code to perform a UNJ to the acquired label Ln +1
 objectFile << "      UNJ " << left << setw(4) << label << "+1   \n";
 //emit code to label the next instruction with the acquired label Ln
		//and do a register-memory load TRUE
 objectFile << left << setw(4) << label << "  LDA " << left << setw(9) << "FALS" << "\n";
 if(!falseInTable)
 {
	 falseInTable = true;
	Insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);
 }
 //deassign all temporaries involved and free those names for reuse;
 if(isTemp(operand2))
	FreeTemp();
 if(isTemp(operand1))
	FreeTemp();
 //A Register = next available temporary name and change type of its symbol table entry to boolean
 regContents = GetTemp();
 symbolTable[getTablePos(regContents)].dataType = BOOLEAN;
 //push the name of the result onto operandStk
 PushOperand(regContents);
}

void EmitGreaterThanEqualCode(string operand1, string operand2)
{
 if (WhichType(operand1) != WhichType(operand2))
	errorCatch("incompatible types");
 if (regContents != operand1 && regContents != operand2 && isTemp(regContents))
	{
		objectFile << "      STA " << left << setw(9) << regContents << "deassign AReg\n";
		symbolTable[getTablePos(regContents)].alloc = YES;
		symbolTable[getTablePos(regContents)].units = 1;
		regContents = "";
	}
 if (regContents != operand1 && regContents != operand2 && !isTemp(regContents))
		regContents = "";
 if (regContents != operand1 && regContents != operand2)
	{
		objectFile << "      LDA " << left << setw(9) << operand2 << '\n';
		regContents = operand2;
	}
 //emit code to perform a register-memory subtraction with A Register holding the result;
 objectFile << "      ISB " << left << setw(9) << operand1 << symbolTable[getTablePos(operand2)].externalName << " >= " << symbolTable[getTablePos(operand1)].externalName << '\n';
 //emit code to perform an AZJ to the next available Ln
 string label = nextLabel();
 objectFile << "      AMJ " << left << setw(9) << label << '\n';
 //emit code to do a register-memory load FALS
 objectFile << "      LDA " << left << setw(9) << "TRUE" << "\n";
 if(!trueInTable)
 {
	 trueInTable = true;
	 Insert("TRUE", BOOLEAN, CONSTANT, "1", YES, 1);
 }
 //emit code to perform a UNJ to the acquired label Ln +1
 objectFile << "      UNJ " << left << setw(4) << label << "+1   \n";
 //emit code to label the next instruction with the acquired label Ln
		//and do a register-memory load TRUE
 objectFile << left << setw(4) << label << "  LDA " << left << setw(9) << "FALS" << "\n";
 if(!falseInTable)
 {
	 falseInTable = true;
	Insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);
 }
 //deassign all temporaries involved and free those names for reuse;
 if(isTemp(operand2))
	FreeTemp();
 if(isTemp(operand1))
	FreeTemp();
 //A Register = next available temporary name and change type of its symbol table entry to boolean
 regContents = GetTemp();
 symbolTable[getTablePos(regContents)].dataType = BOOLEAN;
 //push the name of the result onto operandStk
 PushOperand(regContents);
}

void EmitNotEqualCode(string operand1, string operand2)
{
 if (WhichType(operand1) != WhichType(operand2))
	errorCatch("incompatible types");
 if (regContents != operand1 && regContents != operand2 && isTemp(regContents))
	{
		objectFile << "      STA " << left << setw(9) << regContents << "deassign AReg\n";
		symbolTable[getTablePos(regContents)].alloc = YES;
		symbolTable[getTablePos(regContents)].units = 1;
		regContents = "";
	}
 if (regContents != operand1 && regContents != operand2 && !isTemp(regContents))
		regContents = "";
 if (regContents != operand1 && regContents != operand2)
	{
		objectFile << "      LDA " << left << setw(9) << operand2 << '\n';
		regContents = operand2;
	}
 //emit code to perform a register-memory subtraction with A Register holding the result;
 objectFile << "      ISB " << left << setw(9) << operand1 << symbolTable[getTablePos(operand2)].externalName << " <> " << symbolTable[getTablePos(operand1)].externalName << '\n';
 //emit code to perform an AZJ to the next available Ln
 string label = nextLabel();
 objectFile << "      AZJ " << left << setw(4) <<  label << setw(5) << "+1" << '\n';
 //emit code to label the next instruction with the acquired label Ln
		//and do a register-memory load TRUE
 objectFile << left << setw(4) << label << "  LDA " << left << setw(9) << "TRUE" << "\n";
 if(!trueInTable)
 {
	 trueInTable = true;
	 Insert("TRUE", BOOLEAN, CONSTANT, "1", YES, 1);
 }
 //deassign all temporaries involved and free those names for reuse;
 if(isTemp(operand2))
	FreeTemp();
 if(isTemp(operand1))
	FreeTemp();
 //A Register = next available temporary name and change type of its symbol table entry to boolean
 regContents = GetTemp();
 symbolTable[getTablePos(regContents)].dataType = BOOLEAN;
 //push the name of the result onto operandStk
 PushOperand(regContents);
}

void EmitAssignCode(string operand1,string operand2) //assign the value of operand1 to operand2
{
 if (WhichType(operand1) != WhichType(operand2))
	errorCatch("incompatible type");
 if (symbolTable[getTablePos(operand2)].mode != VARIABLE)
	errorCatch("symbol on left-hand side of assignment must have a storage mode of VARIABLE");
 if (operand1 == operand2) return;
 if (regContents != operand1)
	objectFile << "      LDA " << left << setw(9) << operand1 << '\n';
 // emit code to store the contents of that register into the memory location pointed to by
 // operand2
 objectFile << "      STA " << left << setw(9) << operand2 << symbolTable[getTablePos(operand2)].externalName << " := " << symbolTable[getTablePos(operand1)].externalName << "\n";
 // deassign operand1;
 regContents = operand2;
 // if operand1 is a temp then free its name for reuse;
 if(isTemp(operand1))
	FreeTemp();
 //operand2 can never be a temporary since it is to the left of ':='
}

void EmitThenCode(string operand)
{
	if (WhichType(operand) != BOOLEAN)
		errorCatch("conditional statements must be of type BOOLEAN");
	string tempLabel;
	//assign next label to tempLabel
	tempLabel = nextLabel();
	//emit instruction to set the condition code depending on the value of operand;
	if (regContents != operand)
	{
		objectFile << "      LDA " << left << setw(9) << operand << "\n";
	}
	//emit instruction to branch to tempLabel if the condition code indicates operand is zero
	//(false);
	objectFile << "      AZJ " << left << setw(9) << tempLabel << "if false jump to " << tempLabel << "\n";
	//push tempLabel onto operandStk so that it can be referenced when EmitElseCode() or
	//EmitPostIfCode() is called;
	PushOperand(tempLabel);
	//if operand is a temp then
	if(isTemp(operand))
	{
		//free operand's name for reuse;
		FreeTemp();
	}
	//deassign operands from all registers

	regContents = "";

}

void EmitElseCode(string operand)
{
	string tempLabel;
	tempLabel = nextLabel();
	// emit instruction to branch unconditionally to tempLabel;
	objectFile << "      UNJ " << left << setw(4) << tempLabel << "     jump to end if\n";
	// emit instruction to label this point of object code with the argument operand;
	objectFile << left << setw(4) << operand << "  NOP          else\n";
	PushOperand(tempLabel);

	regContents = "";

}

void EmitPostIfCode(string operand)
{
	objectFile << left << setw(4) << operand << "  NOP          end if\n";
	regContents = "";
}

void EmitWhileCode()
{
	string tempLabel;
	tempLabel = nextLabel();
	objectFile << left << setw(4) << tempLabel << "  NOP          while\n";
	PushOperand(tempLabel);
	
	regContents = "";
}

void EmitDoCode(string operand)
{
	if (WhichType(operand) != BOOLEAN)
		errorCatch("conditional statements must be of type BOOLEAN");
	string tempLabel;
	tempLabel = nextLabel();
	// emit instruction to set the condition code depending on the value of operand;
	if (regContents != operand)
	{
		objectFile << "      LDA " << left << setw(9) << operand << "\n";
	}
	// emit instruction to branch to tempLabel if the condition code indicates operand is zero
	// (false);
	objectFile << "      AZJ " << left << setw(9) << tempLabel << "do\n";
	
	PushOperand(tempLabel);
	
	//if operand is a temp then
	if(isTemp(operand))
	{
		//free operand's name for reuse;
		FreeTemp();
	}
	//deassign operands from all registers
	regContents = "";
}

void EmitPostWhileCode(string operand1, string operand2)
{
	objectFile << "      UNJ " << left << setw(9) << operand2 << "end while\n";
	//emit instruction which labels this point of the object code with the argument operand1;
	objectFile << left << setw(4) << operand1 << "  NOP          \n";
	//deassign operands from all registers
	
	regContents = "";

}

void EmitRepeatCode()
{
	string tempLabel;
	tempLabel = nextLabel();
	//emit instruction to label this point in the object code with the value of tempLabel;
	objectFile << left << setw(4) << tempLabel << "  NOP          repeat\n";
	PushOperand(tempLabel);
	
	regContents = "";
}

void EmitUntilCode(string operand1, string operand2)
{
	if (WhichType(operand1) != BOOLEAN)
		errorCatch("conditional statements must be of type BOOLEAN");
	// emit instruction to set the condition code depending on the value of operand1;
	if (regContents != operand1)
	{
		objectFile << "      LDA " << left << setw(9) << operand1 << "\n";
	}
	// emit instruction to branch to the value of operand2 if the condition code indicates operand is
	// zero (false);
	objectFile << "      AZJ " << left << setw(9) << operand2 << "until\n";
	
	//if operand is a temp then
	if(isTemp(operand1))
	{
		//free operand's name for reuse;
		FreeTemp();
	}
	//deassign operands from all registers
	regContents = "";
}

void FreeTemp()
{
 currentTempNo--;
 if (currentTempNo < -1)
	errorCatch("compiler error, currentTempNo should be >= –1");
}

string GetTemp()
{
 string temp;
 currentTempNo++;
 temp = "T";
 if(currentTempNo <= 9)
	temp += (currentTempNo + 48);
 else
 {
	temp += ((currentTempNo / 10) + 48);
	temp += ((currentTempNo % 10) + 48);
 }	
 if (currentTempNo > maxTempNo)
 {
	Insert(temp, UNKNOWN, VARIABLE, "1", NO, 0);
	maxTempNo++;
 }
 return temp;
}

string nextLabel()
{
	string newName;
	labelCount++;
	newName = "L";
	if (labelCount <=9)
		newName += labelCount + 48;
	else
	{
		newName += ((labelCount /10) + 48);
		newName += ((labelCount %10) + 48);
	}		
	
	return newName;
}

void PushOperator(string name) //push name onto operatorStk
{
	operatorStk.push(name);
}

void PushOperand(string name) //push name onto operandStk
 //if name is a literal, also create a symbol table entry for it
{
	// bool varFound = false;
	// string x = "";
	// uint i = 0;
	// if (WhichType(name) == INTEGER)
	// {
		// i = 0;
		// if (symbolTable.size() != 0)
		// {		
			// while (i < symbolTable.size())
			// {
				// if (name == symbolTable[i].internalName && symbolTable[i].dataType == INTEGER && symbolTable[i].mode == CONSTANT)
				// {
					// varFound = true;
					// x = symbolTable[i].internalName;
					// break;
				// }
				// i++;
				// if (i == symbolTable.size() && !varFound)
				// {
					// Insert(name, INTEGER, CONSTANT, name, YES, 1);
					// x = symbolTable[getTablePos(name)].internalName;
				// }
			// }
		// }
	// }
	
	// else if (WhichType(name) == BOOLEAN)
	// {
		// if (name == "true")
		// {
			// if (symbolTable.size() != 0)
			// {	
				// i = 0;
				// while (i < symbolTable.size())
				// {
					// if ("1" == symbolTable[i].value && symbolTable[i].dataType == BOOLEAN && symbolTable[i].mode == CONSTANT)
					// {
						// varFound = true;
						// x = symbolTable[i].internalName;
						// break;
					// }
					// i++;
					// if (i == symbolTable.size() && !varFound)
					// {
						// Insert("TRUE", BOOLEAN, CONSTANT, "1", YES, 1);
						// x = symbolTable[getTablePos("TRUE")].internalName;
					// }
				// }
			// }
		// }
		
		// else if (name == "false")
		// {
			// if (symbolTable.size() != 0)
			// {	
				// i = 0;
				// while (i < symbolTable.size())
				// {
					// if ("0" == symbolTable[i].value && symbolTable[i].dataType == BOOLEAN && symbolTable[i].mode == CONSTANT)
					// {
						// varFound = true;
						// x = symbolTable[i].internalName;
						// break;
					// }
					// i++;
					// if (i == symbolTable.size() && !varFound)
					// {
						// Insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);
						// x = symbolTable[getTablePos("FALSE")].internalName;
					// }
				// }
			// }
		// }
	// }
	operandStk.push(name);
}

string PopOperator() //pop name from operatorStk
{
	string topOp;
	if (!operatorStk.empty())
	{
		topOp = operatorStk.top();
		operatorStk.pop();
		return topOp;
	}
	else errorCatch("operator stack underflow");
	return "foo bar";  //default case never reached due to errorCatch exit
}

string PopOperand() //pop name from operandStk
{
	string topOp;
	if (!operandStk.empty())
	{
		topOp = operandStk.top();
		operandStk.pop();
		return topOp;
	}
	else errorCatch("operand stack underflow");
	return "foo bar";  //default case never reached due to errorCatch exit
}

void errorCatch(string errorString)
{
	listingFile <<"\nError: Line" << lineCount << ": " << errorString << endl << endl;
	
	// cout << token << endl;
	
	errorBus = true;
	
	CreateListingTrailer();
	
	//PrintSymbolTable();
	
	// for(uint i = 0; i < symbolTable.size(); i++)
	// {
		// objectFile << symbolTable[i].externalName << setw((17-symbolTable[i].externalName.size())+2) << symbolTable[i].internalName << setw(13)
				   // << (symbolTable[i].dataType == 2 ? "PROG_NAME" : (symbolTable[i].dataType == 1 ? "BOOLEAN" : "INTEGER")) << setw(10) 
				   // << (symbolTable[i].mode == 1 ? "CONSTANT" : "VARIABLE") << setw(17) 
				   // << (symbolTable[i].value == "true" ? "1" : (symbolTable[i].value == "false" ? "0" : symbolTable[i].value)) << setw(5) 
				   // << (symbolTable[i].alloc == 1 ? "NO" : "YES") << setw(3) << symbolTable[i].units << endl;
	// }
	
	sourceFile.close();
	listingFile.close();
	objectFile.close();
	exit(0);
}
