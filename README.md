# CS421 Final Program

This project required us to design and implement a scanner, parser, and translator for Japanese romaji into english.

SCANNER

For the scanner, we had to develop DFA's for nearly all Japanese syllables. Pre-defined words are hard coded into a dictionary array and provide a basis for most basic Japanese sentences. If a word is found to be in this array it is assigned the relevant type found next to the word in the array. If it is not found in the array but is a valid word it is assigned type WORD1. If the word has an uppercase letter as the last character in the word it is WORD2, or a verb root. If the word is not made of Japanese syllables it is marked as ERROR. These types are passed into the relevant parser functions in order to check syntax as explained below.

PARSER

For the parser, we added additional functionality to the scanner. Primarily, the use of the scanner function results to parse predefined sentence structures to check for grammar accuracy. These sentences were in BNF form allowing us to scan for valid pieces of a sentence in order, and identify parts that did not follow the grammar. The grammar only contains very simple Japanese sentence structure. There are several parser functions representing the various layers of the BNF grammar. Story, sentence, statement 1-3 (these represent the major variations in sentence structure such as desciptions or actions), noun, verb, be, tense. Within each of these functions are calls to check if the current word being scanned matches with the expected type. The program cascades down these functions as words are confirmed to be correct. Two types of syntax errors can occur. First, syntaxerror1 indicates that the parser found the wrong type when stepping through a sentence. Second, syntaxerror2 occurs when a switch statement reaches default (should never happen). If a syntax error is reached, the user has the option to end there, among other option that will be detailed later in the extra options section. If no errors are found, the parser will end on a period and continue to the next sentence or EOFM.

TRANSLATOR

The translator also adds to the previous parser code. Two new functions are added, genEword and gen. genEword searches through a hard coded dictionary and returns the equivalent english word. This only occurs for WORD1 and WORD2, which are words that are already defined in the Japanese dictionary. gen outputs the English equivalent word if it is in the dictionary, otherwise it leaves the Japanese word. If the word is a connector or expression (such as "to be") the type is output. It should be noted that the translator function will only activate if there are no errors in both the scanner and parser. genEword and gen calls are placed within the parser functions to output the flow of the sentence. At the same time, the words output to the screen in the translator are also placed in the translator.txt file for easy access to previous translations.

OTHER FEATURES

Before the program begins the main functions, the user is asked if they would like to enable several additional functions to either simplify the output, or to display the processing at several levels.

Scanner Trace: As the scanner scans through each word, additional information is printed to the screen to indicate what the program is doing as it steps through words. The information displayed is the current word being scanned, what the word's token is, and when the scanner is searching through the dictionary to check word validity.

Parser Trace: The parser trace has the same function as the scanner trace, although it is called within parser functions. The information displayed is the step through of the BNF grammar as each new function is entered. It is also used in the token matching to print when a type has been matched.

Display Matches: This is a secondary selection of parser trace, if parser trace is turned off the user will still be asked if they want matching to be displayed. If this is turned on, then the matched notification will be displayed when a correct type is found, as above.

Error Correction: This allows the user to redefine the current word that is being scanned. If this is turned on, the user will be prompted when a syntaxerror1 is found (type does not match expected).

Error Output: Asks the user if they would like the errors output to be saved to a text file for later use.

Remove Error File: Asks the user if they would like to start with a new error output file or append to the original. This truncates the old file with the new errors or appends if they want to keep the old errors.
