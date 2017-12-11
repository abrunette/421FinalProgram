/*****************translator.cpp**************/
//Authors: Aaron Brunette, Paul Rowe, Erik Leung
//Last updated: 2017/12/09
//Compiled with g++
//Written on vim, visual studio, github
//Purpose: Translate romanji into english while catching
// spelling and gramatical errors.
//
//	 	MIT License
/*****************************************/

#include <iostream>		//i/o
#include <cstdio>		//printf, i/o
#include <cstdlib>		//exit()
#include <fstream>		//fstream
#include <string>		//string
#include <time.h>		//time
#include <unordered_map>	//english lexicon
using namespace std;

//=================================================
// File translator.cpp written by Group Number: 03
//=================================================

//token vars
enum token_type { //type creation
	ERROR, WORD1, WORD2, PERIOD, 
	VERB, VERBNEG, VERBPAST, VERBPASTNEG, 
	IS, WAS, OBJECT, SUBJECT, DESTINATION, 
	PRONOUN, CONNECTOR, EOFM };

//enum to string
string conversion[16] = {
"ERROR", "WORD1", "WORD2", "PERIOD", 
"VERB", "VERBNEG", "VERBPAST", "VERBPASTNEG", 
"IS", "WAS", "OBJECT", "SUBJECT", "DESTINATION", 
"PRONOUN", "CONNECTOR", "EOFM" };

//Global variables
token_type saved_token;			//next token from scanner, default is empty
string saved_lexeme;			//next word from scanner, default empty
bool token_available = false;		//flag, default is unavailable
string saved_E_word;			//hold saved english word for gen
string uInput;				//user input name of file to open

//global files
fstream toParse;			//global stream to parse file
fstream errors;				//global stream to collect error messages in file
fstream translated;			//hold translated words

//extra feature variables
char sTrace = 'n';			//scanner trace off by default
char pTrace = 'n';			//parser trace off by default
char wantMatch = 'n';			//display matches if parser trace is off
char errorCorr = 'n';			//error correction off by default
char errorOutput = 'n';			//error output off by default
char removeErrors = 'n';		//for making a brand new errors file

//Translator function prototypes
void getEword();			//get english word
void gen(string);			//output translation

//japanese word lexicon
const int arraySize = 38;
string reservedwords[arraySize] = {
"masu", "VERB", "masen", "VERBNEG", "mashita", "VERBPAST",
"masendeshita", "VERBPASTNEG", "desu", "IS", "deshita", "WAS",
"o", "OBJECT", "wa", "SUBJECT", "ni", "DESTINATION",
"watashi", "PRONOUN", "anata", "PRONOUN", "kare", "PRONOUN",
"kanojo", "PRONOUN", "sore", "PRONOUN", "mata", "CONNECTOR",
"soshite", "CONNECTOR", "shikashi", "CONNECTOR",
"dakara", "CONNECTOR", "eofm", "EOFM" };

//english lexicon
unordered_map <string, string> dict = {
                {"watashi", "I/me"},
                {"anata", "you"},
                {"kare", "he/him"},
                {"kanojo", "she/her"},
                {"sore", "it"},
                {"mata", "also"},
                {"soshite", "Then"},
                {"shikashi", "however"},
                {"dakara", "Therefore"},
                {"ikI", "go"},
                {"tabeE", "eat"},
                {"shiE", "do" },
                {"kiE", "come"},
                {"tatakaI", "fight"},
                {"benkyou", "study"},
                {"kore", "this"},
                {"nai", "not"},
                {"seito", "student"},
                {"nakI", "cry"},
                {"yorokobI", "rejoice"},
                {"toire", "toilet"},
                {"kanashii", "sad"},
                {"agE", "give"},
                {"gohan", "meal"},
                {"sensei", "teacher"},
                {"tesuto", "test"},
		{"sekinin", "responsibility"},
};

//parser function prototypes
void story();
void sentence();
void statement1();
void statement2();
void statement3();
void noun();
void verb();
void be();
void tense();

//General function prototypes
token_type next_token();	//go to next token
bool match(token_type);		//find and remove from text
void checking();		//ask user if they want extra features

//Syntax error function prototypes
int foundError = 0;
void syntaxerror1(token_type);		//mismatch, error checking
void syntaxerror2(string);		//switch default

//Scanner function prototypes
void scanner(token_type&, string&);
void dictionary(token_type&, string);	//checks words against reservedwords array
bool startstate(string);
bool vowels(string, int&);
bool consonants(string, int&);
bool sRoot(string, int&);
bool zRoot(string, int&);
bool jRoot(string, int&);
bool tRoot(string, int&);
bool dRoot(string, int&);
bool cRoot(string, int&);
bool wRoot(string, int&);
bool yRoot(string, int&);

//Driver, opens files and runs parser
//Done by: Aaron
int main()
{
	printf("\nEnter the name of the file you want to translate: ");
	getline(cin, uInput);	
	
	//new file every time
	translated.open("translated.txt", fstream::out | fstream::trunc);
	toParse.open(uInput.c_str());		//open test file	

	if(!toParse.is_open())	//check if file opened correctly
	{
		printf("Error opening file. Ending program...\n");
		exit(EXIT_FAILURE);
	}		

	story();		//begin parse		

	toParse.close();
	translated.close();

	return 0;
}

//************************Translator functions****************************//

//Searches the lexicon for the english equivalent
//Done by: Erik Leung
void getEword()
{
	auto search = dict.find(saved_lexeme);
            if (search != dict.end())
            {
                saved_E_word = search->second;
		return;
            }
            saved_E_word = saved_lexeme;	
}

//Outputs the internal representation to the screen and translation file
//Done by: Paul
void gen(string theRep)
{
	if(theRep == "TENSE")
	{
		cout << theRep << ": " << conversion[saved_token] << endl;
		translated << theRep << ": " << conversion[saved_token] << endl;
	}
	else
	{
		cout << theRep << ": " << saved_E_word << endl;
		translated << theRep << ": " << saved_E_word << endl;
	}
}

//*************************End of translator functions***********************//

/*********************Parser Functions************************/

//<story> -> <sentence> { <sentence> }
//Done by: Erik Leung
void story()
{
	checking();	//check what features the user wants

	if (pTrace == 'y')
		printf("Processing <story>\n");

	sentence();
	translated << endl;
	while(next_token() != EOFM)
	{	
		sentence();
		translated << endl;
	}

	if(pTrace == 'y')
		printf("\nSuccessfully parsed <story>\n");

	if(!foundError)
		errors << "No errors found.\n";

	if(errors.is_open())
	{
		errors << "-----------------------------------------------------------" << endl << endl;
		errors.close();
	}
}

//<sentence> -> [CONNECTOR #getEword# #gen#] <noun> #genEword# SUBJECT #gen# <statement1>
//Done by: Erik Leung
void sentence()
{
	if (pTrace == 'y')
		printf("\n====== Processing <s> ======\n");
	
	if(next_token() == CONNECTOR)	
	{
		match(CONNECTOR);
		getEword();
		gen("CONNECTOR");
	}
	switch(next_token())
	{
		case WORD1: case PRONOUN:
			noun();
			getEword();
			match(SUBJECT);
			gen("ACTOR");
			statement1();
			break;
		default:
			syntaxerror2("<s>");
			return;
	}
}

//<statement1> -> <verb> #genEword# #gen# <tense> #gen# PERIOD | <noun> #genEword# <statement2>
//Done by: Erik Leung
void statement1()
{
	if(pTrace == 'y')
		printf("Processing <afterSubject>\n");

	switch(next_token())
	{
		case WORD2:
		 	verb();
			getEword();
			gen("ACTION");
			tense();
			gen("TENSE");
			match(PERIOD);
			printf("\n");
			break;
		case WORD1: case PRONOUN:
			noun();
			getEword();	
			statement2();
			break;
		default:
			syntaxerror2("<afterSubject>");
			return;
	}
}

//<statement2> -> #gen# <be> #gen# PERIOD | DESTINATION #gen# <verb> #genEword# #gen# <tense> #gen# PERIOD | OBJECT #gen# <statement3>
//Done by: Erik Leung
void statement2()
{
	if(pTrace == 'y')
		printf("Processing <afterNoun>\n");

	switch(next_token())
	{
		case IS: case WAS:
			gen("DESCRIPTION");
			be();
			gen("TENSE");
			match(PERIOD);
			printf("\n");
			break;
		case DESTINATION:
			match(DESTINATION);
			gen("TO");
			verb();
			getEword();
			gen("ACTION");
			tense();
			gen("TENSE");
			match(PERIOD);
			printf("\n");
			break;
		case OBJECT:
			match(OBJECT);
			gen("OBJECT");
			statement3();
			break;
		default:
			syntaxerror2("<afterNoun>");
			return;
	}
}

//<statement3> -> <verb> #genEword# #gen# <tense> PERIOD | <noun> #genEword# DESTINATION #gen# <verb> #getEword# #gen# <tense> #gen# PERIOD
//Done by: Erik Leung
void statement3()
{
	if (pTrace == 'y')
		printf("Processing <afterObject>\n");

	switch(next_token())
	{
		case WORD2:
			verb();
			getEword();
			gen("ACTION");
			tense();
			gen("TENSE");
			match(PERIOD);
			printf("\n");
			break;
		case WORD1: case PRONOUN:
			noun();
			getEword();
			match(DESTINATION);
			gen("TO");
			verb();
			getEword();
			gen("ACTION");
			tense();
			gen("TENSE");
			match(PERIOD);
			printf("\n");
			break;
		default:
			syntaxerror2("<afterObject>");
			return;
	}
}

//<noun> -> WORD1 | PRONOUN
//Done by: Erik Leung
void noun()
{
	if (pTrace == 'y')
		printf("Processing <noun>\n");

	switch(next_token())
	{
		case WORD1:
			match(WORD1);
			break;
		case PRONOUN:
			match(PRONOUN);
			break;
		default:
			syntaxerror2("<noun>");
			return;
	}
}

//<verb> -> WORD2
//Done by: Erik Leung
void verb()
{
	if (pTrace == 'y')
		printf("Processing <verb>\n");

	switch(next_token())
	{
		case WORD2:
			match(WORD2);
			break;
		default:
			syntaxerror2("<verb>");
			return;
	}
}

//<be> -> IS | WAS
//Done by: Erik Leung
void be()
{
	if (pTrace == 'y')
		printf("Processing <be>\n");

	switch(next_token())
	{
		case IS:
			match(IS);
			break;
		case WAS:
			match(WAS);
			break;
		default:
			syntaxerror2("<be>");
			return;
	}
}

//<tense> -> VERBPAST | VERBPASTNEG | VERB | VERBNEG
//Done by: Erik Leung
void tense()
{
	if (pTrace == 'y')
		printf("Processing <tense>\n");

	switch(next_token())
	{
		case VERBPAST:
			match(VERBPAST);
			break;
		case VERB:
			match(VERB);
			break;
		case VERBPASTNEG:
			match(VERBPASTNEG);
			break;
		case VERBNEG:
			match(VERBNEG);
			break;
		default:
			syntaxerror2("<tense>");
			return;
	}
}

//Matches types to check for correct grammar
//Done by: Aaron
bool match(token_type thetype)
{
		if (next_token() != thetype)	//mismatch
		{
			syntaxerror1(thetype);
			//only retries if user chose to correct error
			scanner(saved_token, saved_lexeme);
			match(thetype);
		}
		else //matched
		{
			if(pTrace == 'y' || wantMatch == 'y')
				printf("Matched %s\n", conversion[thetype].c_str());
				//cout << "Matched " << conversion[thetype] << endl;

			token_available = false;	//remove token
			return true;
		}
}

//Grabs the next word from the input file
//Done by: Aaron
token_type next_token()
{
	if (!token_available)	//no saved token
	{
		scanner(saved_token, saved_lexeme);	//finds next token and lexeme
		token_available = true;			//has saved token
	}
	return saved_token;
}

//Asks the user what extra features they want
//Done by: Aaron
void checking()
{
	printf("\nWould you like to trace the scanner (y/n)? ");
	cin >> sTrace;

	printf("Would you like to trace the parser (y/n)? ");
	cin >> pTrace;

	if(pTrace != 'y')
	{
		printf("Would you like to show matched types (y/n)? ");
		cin >> wantMatch;
	}

	printf("Would you like to try to correct syntax errors (y/n)? ");
	cin >> errorCorr;

	printf("Would you like error messages to be output to errors.txt (y/n)? ");
	cin >> errorOutput;

	if(errorOutput == 'y')
	{
		printf("Would you like to start with a new errors.txt file (y/n)? ");
		cin >> removeErrors;
	
		if(removeErrors == 'y')	//new file
			errors.open("errors.txt", fstream::out | fstream::trunc);
		else			//add to end of file
			errors.open("errors.txt", fstream::out | fstream::app);
	}	

	printf("\n");

	//time variables for errors.txt and translation.txt
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	//output time into output files
	errors << uInput.c_str() << "\t" << asctime(timeinfo) << endl;
	translated << uInput.c_str() << "\t" << asctime(timeinfo) << endl;
}

//Syntax Errors

//when match() function does not match
//Done by: Paul
void syntaxerror1(token_type thetype)
{
	foundError = 1;
	//output to screen and error.txt file
	//cout << "SYNTAX ERROR: expected " << conversion[thetype] << " but found " << saved_lexeme << endl;
	printf("SYNTAX ERROR: expected %s but found %s\n", conversion[thetype].c_str(), saved_lexeme.c_str());
	
	if(errors.is_open())
		errors << "SYNTAX ERROR: expected " << conversion[thetype] << " but found " << saved_lexeme << endl;
	
	if (errorCorr == 'y')	//starts error checking if enabled by user
	{
		//cout << "Instead of "<< saved_lexeme << " try (eofm to exit): ";
		printf("Instead of %s try (eofm to exit): ", saved_lexeme.c_str());
		cin >> saved_lexeme;	//user inputs replacement string to try
	}
	else	//otherwise end as usual
	{
		errors << "-----------------------------------------------------------" << endl << endl;
		exit(EXIT_FAILURE);	//end program
	}
}

//Done by: Paul
void syntaxerror2(string pFunction)	//when a switch statement goes to default
{
	foundError = 1;
	//output to screen and error.txt file
	//cout << "SYNTAX ERROR: unexpected " << saved_lexeme << " found in " << pFunction << endl;
	printf("SYNTAX ERROR: unexpected %s found in %s\n", saved_lexeme.c_str(), pFunction.c_str());

	if(errors.is_open())
		errors << "SYNTAX ERROR: unexpected " << saved_lexeme << " found in " << pFunction << endl;

	errors << "-----------------------------------------------------------" << endl << endl;	
	exit(EXIT_FAILURE);	//always ends program
}

/*****************************End of Parser Functions***************************/



/*****************************Scanner Functions*********************************/

//Scanner processes only one word each time it is called
//Done by: Aaron & Erik
void scanner(token_type& a, string& w)
{
	bool result = true;	//default, word is assumed valid	

	if(!token_available)	//if error checking is enabled there will already be a word to be checked
		toParse >> w;  //read word

	if(sTrace == 'y')
		cout << "Scanner called using word: " << w << endl;
	
	if(w == "eofm")	//eof found
	{
		a = EOFM;
		return;
	}
	else if(w == ".")	//period calls period DFA
	{
		a = PERIOD;
		return;
	}
	
	result = startstate(w);		//enter DFA

	if (result == false)	//result of DFA is false, word is invalid
	{
		printf("Lexical error: %s is not a valid token\n", w.c_str());
		a = ERROR;
	}
	else	//word is valid
	{
		if (isupper(w[w.size() - 1]))	//check last character in word for Uppercase
			a = WORD2;	//verb
		else				//not uppercase so default WORD1, check against reservedwords array
		{
			a = WORD1;	//default WORD1, check if reserved
			dictionary(a, w);
		}
	}
	if(sTrace == 'y')
		printf("Scanner saving token as: %s\n", conversion[a].c_str());
}

//Searches through the reservedwords array to find type
//Done by: Aaron & Erik
void dictionary(token_type &a, string w)
{
	if(sTrace == 'y')
		printf("Searching dictionary for %s\n", w.c_str());

	//while reservedwords can read in
	for (int i = 0; i < arraySize; i++)
	{
		if (reservedwords[i] == w)	//if reservedword == givenword
		{
			i++;	//move to type
			for(int step = 0; step < 16; step++)	//step through conversion array to find string
			{
				if(conversion[step] == reservedwords[i])	//if reservedtype == giventype
				{
					a = static_cast<token_type>(step);	//set type to array position, convert int to enum
					return;
				} 
			} return;
		}i++;	//move to next word
	} return;
}



/***************************Bools******************************/

//Sentence -> vowel | consonant | s | z | j | t | d | c | w | y
//Done by: Paul
bool startstate(string w)	//also final state
{
	int charpos = 0;
	bool result = true;		//result of going through bools

	while(w[charpos] != '\0')
	{
		switch(w[charpos])
		{
			case 'a': case 'i': case 'u': case 'e': case 'o':
				result = vowels(w, charpos);
				break;
			case 'k': case 'n': case 'h': case 'm': case 'r': case 'g': case 'b': case 'p':
				result = consonants(w, charpos);
				break;
			case 's':
				result = sRoot(w, charpos);
				break;
			case 'z':
				result = zRoot(w, charpos);
				break;
			case 'j':
				result = jRoot(w, charpos);
				break;
			case 't':
				result = tRoot(w, charpos);
				break;
			case 'd':
				result = dRoot(w, charpos);
				break;
			case 'c':
				result = cRoot(w, charpos);
				break;
			case 'w':
				result = wRoot(w, charpos);
				break;
			case 'y':
				result = yRoot(w, charpos);
				break;
			default:		//invalid character
				return false;
		}
		if(w[charpos] == 'E' || w[charpos] == 'I')	//WORD2 check
			return true;
		if(result == false)				//failed in bools
			return false;
	}
	return true;
}

//vowel -> ( a | i | u | e | o )[n]
//done by: Paul & Aaron
bool vowels(string w, int& charpos)
{
	if(w[charpos + 1] == 'n')	//Vn
	{
		charpos++; charpos++;
		return true;
	}
	else
	{
		charpos++;
		return true;
	}
}

//consonant -> ( n | k | h | m | r | g | b | p )[n]
//done by: Paul & Aaron
bool consonants(string w, int& charpos)
{	
	int state = 0;
	char hold = w[charpos];
	charpos++;

	if(state == 0 && w[charpos] == hold && (hold == 'k' || hold == 'p'))	//little tsu, CC
	{
		state = 0;
		charpos++;
	}
	if(state == 0 && (w[charpos] == 'a' || w[charpos] == 'i' || w[charpos] == 'u' || w[charpos] == 'e' || w[charpos] == 'o'))	//CV
	{	
		state = 2;
		charpos++;
	}
	if(state == 0 && w[charpos] == 'y')	//Cy
	{
		state = 1;
		charpos++;
	}
	if(state == 1 && (w[charpos] == 'a' || w[charpos] == 'u' || w[charpos] == 'o')) //CyV
	{
		state = 2;
		charpos++;
	}
	if(state == 2 && w[charpos] == 'n')	//-n
	{
		charpos++;
		state = 3;
	}
  	if(state == 2 || state == 3) 
		return true;
   	else 
		return false;
}

//s -> [s]( a | i | u | o )[n] | [s]( ha | hi | hu | ho )[n]
//done by: Paul & Aaron
bool sRoot(string w, int& charpos)
{
	int state = 0;
	charpos++;
	
	if(state == 0 && w[charpos] == 's')	//little tsu, ss
	{
		state = 0;
		charpos++;
	}
	if(state == 0 && w[charpos] == 'h')	//sh
	{
		state = 1;
		charpos++;
	}
	if(state == 1 && (w[charpos] == 'a' || w[charpos] == 'i' || w[charpos] == 'u' || w[charpos] == 'o'))	//sha shi shu sho
	{
		state = 2;
		charpos++;
	}
	if(state == 0 && (w[charpos] == 'a' || w[charpos] == 'u' || w[charpos] == 'e' || w[charpos] == 'o'))	//sa su se so
	{
		state = 2;
		charpos++;
	}
	if(state == 2 && w[charpos] == 'n')	//-n
	{
		state = 3;
		charpos++;
	}
	if(state == 2 || state == 3)
		return true;
	else
		return false;
}

//z -> ( a | u | e | o )[n]
//done by: Paul & Aaron
bool zRoot(string w, int& charpos)
{
	int state = 0;
	charpos++;

	if(state == 0 && (w[charpos] == 'a' || w[charpos] == 'u' || w[charpos] == 'e' || w[charpos] == 'o'))	//za zu ze zo
	{
		state = 1;
		charpos++;
	}
	if(state == 1 && w[charpos] == 'n')	//-n
	{
		state = 2;
		charpos++;
	}
	if(state == 1 || state == 2)
		return true;
	else
		return false;
}

//j -> ( a | i | u | o )[n]
//done by: Paul & Aaron
bool jRoot(string w, int& charpos)
{
	int state = 0;
	charpos++;

	if(state == 0 && (w[charpos] == 'a' || w[charpos] == 'i' || w[charpos] == 'u' || w[charpos] == 'o'))	//ja ji ju jo
	{
		state = 1;
		charpos++;
	}
	if(state == 1 && w[charpos] == 'n')	//-n
	{
		state = 2;
		charpos++;
	}
	if(state == 1 || state == 2)
		return true;
	else 
		return false;
}

//t -> [t](su)[n] | [t]( a | e | o )[n]
//done by: Paul & Aaron
bool tRoot(string w, int& charpos)
{
	int state = 0;
	charpos++;
	
	if(state == 0 && w[charpos] == 't')	//little tsu, tt
	{
		state = 0;
		charpos++;
	}
	if(state == 0 && w[charpos] == 's')	//ts
	{
		state = 1;
		charpos++;
	}
	if(state == 1 && w[charpos] == 'u')	//tsu
	{
		state = 2;
		charpos++;
	}
	if(state == 0 && (w[charpos] == 'a' || w[charpos] == 'e' || w[charpos] == 'o'))	//ta te to
	{
		state = 2;
		charpos++;
	}	
	if(state == 2 && w[charpos] == 'n')	//-n
	{
		state = 3;
		charpos++;
	}
	if(state == 2 || state == 3)
		return true;
	else
		return false;
}

//d -> ( a | e | o)[n]
//done by: Paul & Aaron
bool dRoot(string w, int& charpos)
{
	int state = 0;
	charpos++;

	if(state == 0 && (w[charpos] == 'a' || w[charpos] == 'e' || w[charpos] == 'o'))	//da de do
	{
		state = 1;
		charpos++;
	}
	if(state == 1 && w[charpos] == 'n')	//-n
	{
		state = 2;
		charpos++;
	}
	if(state == 1 || state == 2)
		return true;
	else
		return false;
}

//c -> [c]( ha | hi | hu | ho )[n]
//done by: Paul & Aaron
bool cRoot(string w, int& charpos)
{
	int state = 0;
	charpos++;
	
	if(state == 0 && w[charpos] == 'c')	//little tsu
	{
		state = 0;
		charpos++;
	}
	if(state == 0 && w[charpos] == 'h')	//ch
	{
		state = 1;
		charpos++;
	}
	if(state == 1 && (w[charpos] == 'a' || w[charpos] == 'i' || w[charpos] == 'u' || w[charpos] == 'o'))	//cha chi chu cho
	{
		state = 2;
		charpos++;
	}
	if(state == 2 && w[charpos] == 'n')	//-n
	{
		state = 3;
		charpos++;
	}
	if(state == 2 || state == 3)
		return true;
	else
		return false;
}

//w -> a[n]
//done by: Paul & Aaron
bool wRoot(string w, int& charpos)
{
	int state = 0;
	charpos++;

	if(state == 0 && w[charpos] == 'a')	//wa
	{
		state = 1;
		charpos++;
	}
	if(state == 1 && w[charpos] == 'n')	//wan
	{
		state = 2;
		charpos++;
	}
	if(state == 1 || state == 2)
		return true;
	else
		return false;
}

//y -> ( a | u | o )[n]
//done by: Paul & Aaron
bool yRoot(string w, int& charpos)
{
	int state = 0;
	charpos++;
	
	if(state == 0 && (w[charpos] == 'a' || w[charpos] == 'u' || w[charpos] == 'o'))	//ya yu yo
	{
		state = 1;
		charpos++;
	}
	if(state == 1 && w[charpos] == 'n')	//yan yun yon
	{
		state = 2;
		charpos++;
	}
	if(state == (1 || 2))
		return true;
	else
		return false;
}

/*****************************End of Scanner Functions************************************/
