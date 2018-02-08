# 421FinalProgram

This project required us to design and implement a scanner, parser, and translator for Japanese romaji into english.

SCANNER
For the scanner, we had to develop DFA's for nearly all Japanese syllables. Pre-defined words are hard coded into a dictionary array and provide a basis for most basic Japanese sentences. If a word is found to be in this array it is assigned the relevant type found next to the word in the array. If it is not found in the array but is a valid word it is assigned type WORD1. If the word has an uppercase letter as the last character in the word it is WORD2, or a verb root. If the word is not made of Japanese syllables it is marked as ERROR.

PARSER
For the parser, we added additional functionality to the scanner. Primarily, the use of the scanner function results to parse predefined sentence structures to check for grammar accuracy. These sentences were in BNF form allowing us to scan for valid pieces of a sentence in order, and identify parts that did not follow the grammar. The grammar only contains very simple Japanese sentence structure. Two types of syntax errors can occur. First, syntaxerror1 indicates that the parser found the wrong type when stepping through a sentence. Second, syntaxerror2 occurs when a switch statement reaches default (should never happen). If a syntax error is reached, the user has the option to end there, among other option that will be detailed later in the extra options section. If no errors are found, the parser will end on a period and continue to the next sentence or EOFM.

TRANSLATOR
The translator also adds to the previous parser code. Two new functions are added, genEword and gen. genEword searches through a hard coded dictionary and returns the equivalent english word. This only occurs for WORD1 and WORD2, which are words that are already defined in the Japanese dictionary. gen outputs the results of the parser and the translator
