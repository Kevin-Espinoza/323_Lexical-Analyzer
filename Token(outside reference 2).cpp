
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;
#include "Token.h"



enum State_t { STATE_BEGIN, END_STATE, COMMENT_FOUND1, COMMENT_FOUND2, IDENTIFIER_FOUND, CHAR_FOUND1, CHAR_FOUND2,
	STRING_FOUND1, STRING_FOUND2, STRING_ESCAPE,  INTEGER_FOUND, STRING_FOUND, OPERATOR_FOUND };

static map<string, Keyword_t> KeywordMap;
static map<string,Keyword_t>::iterator it;

State_t        LexState; 

void InitKeywordMap()
{
	KeywordMap["if"]     = LEX_IF;
	KeywordMap["else"]   = LEX_ELSE;
	KeywordMap["for"]    = LEX_FOR;
	KeywordMap["do"]     = LEX_DO;
	KeywordMap["while"]  = LEX_WHILE;
	KeywordMap["switch"] = LEX_SWITCH;
	KeywordMap["case"]   = LEX_CASE;
	KeywordMap["break"]  = LEX_BREAK;
	KeywordMap["cout"]   = LEX_COUT;
	KeywordMap["cin"]    = LEX_CIN;
};



LexErrorCode_t GetToken (string Buffer, int &Index, FILE ListingFile, token_t &token, bool testing)
{

 
	         LexState = STATE_BEGIN;
	LexErrorCode_t LexError = LEX_NO_ERROR;

	token.TokenType   = LEX_UNDEFINED;
	token.TokenString = "";
	token.IntVal = 0;

	while( Buffer[Index] == ' ')
	{
	    Index++;
	    
	}

	FILE * pFile ;							 
	 
    while( (LexState != END_STATE) && (LexError == LEX_NO_ERROR) )
	{
		char ch = Buffer[Index];
		token.TokenString += ch;

		switch(LexState)
		{
//---------------------------------------------------------------------------
		  case STATE_BEGIN:
			if(isalpha(ch))
			{
				LexState = IDENTIFIER_FOUND;
				break;
			}

			if(isdigit(ch))
			{
				LexState = INTEGER_FOUND;
				break;
			}

		    switch(ch)
			{
			  case '\\':
				LexState = COMMENT_FOUND1;
				break;

			  case '\'':
				 
				token.TokenString.pop_back();
				  LexState = CHAR_FOUND1;
				break;

			  case '\"':
				 
				token.TokenString.pop_back();
				  LexState = STRING_FOUND1;
				break;

			  case ',':
				LexState = END_STATE;
				token.TokenType = LEX_COMMA;
				break;
            case ' ':
               token.TokenString.pop_back();
			
				break;

              case '\n':
              token.TokenString.pop_back();
				 
                break;
              case '\r':
               token.TokenString.pop_back();
		
                break;


			  case ';':
				LexState = END_STATE;
				token.TokenType = LEX_SEMICOLON;
				break;

			  case '(':
				LexState = END_STATE;
				token.TokenType = LEX_LEFT_PARENTHESIS;
				break;

			  case ')':
				LexState = END_STATE;
				token.TokenType = LEX_RIGHT_PARENTHESIS;
				break;

			  case '{':
				LexState = END_STATE;
				token.TokenType = LEX_LEFT_BRACE;
				break;

			  case '}':
				LexState = END_STATE;
				token.TokenType = LEX_RIGHT_BRACE;
				break;

			  case '=':
			  case '!':
		      case '+':
			  case '-':
			  case '*':
			  case '/':
			  case '<':
			  case '<<':
			  case '>':
				LexState = OPERATOR_FOUND;
				break;
			}
			break;

//---------------------------------------------------------------------------

		case IDENTIFIER_FOUND:
			if(isalnum(ch))
				LexState = IDENTIFIER_FOUND;
			else
			{
				LexState = END_STATE;
			token.TokenString.pop_back();
				 
				it = KeywordMap.find(token.TokenString);
				if(it == KeywordMap.end())
					token.TokenType = LEX_IDENTIFIER;
				else
				{	 
					token.TokenType = LEX_KEYWORD;
					 token.KeywordID = (Keyword_t)it->second;
				}
			}
			break;

//---------------------------------------------------------------------------

		case INTEGER_FOUND:
			if(isdigit(ch))
				LexState = INTEGER_FOUND;
			else
			{
				LexState = END_STATE;
				token.TokenString.pop_back();
			 
				token.TokenType = LEX_INTEGER;
			}
			break;

//---------------------------------------------------------------------------

		case COMMENT_FOUND1:
			if(ch == '\\')
			{
				token.TokenType = LEX_LINE_COMMENT;
				LexState = END_STATE;
				break;
			}

			else
			{
				if(ch == '*')
				{
					token.TokenType = LEX_BEGIN_COMMENT;
					LexState = END_STATE;
					break;
				}
				else
					return LEX_UNKNOWN_CHAR;
			}
			break;

//---------------------------------------------------------------------------

		case CHAR_FOUND1:
			if(ch == '\'')
			{
				LexState = END_STATE;
				token.TokenString.pop_back();
				 
				token.CharVal = '\0';
				token.TokenType = LEX_CHAR;
			}
			else
			{
                if(isprint(ch))
				    LexState = CHAR_FOUND2;
			    else
					return LEX_UNKNOWN_CHAR;
			};
			break;

		case CHAR_FOUND2:
			if(ch == '\'')
			{
				LexState = END_STATE;
				token.TokenString.pop_back();
			
				token.TokenType = LEX_CHAR;
			}
			break;

//---------------------------------------------------------------------------

		case STRING_FOUND1:
			if(ch == '\"')
			{
				LexState = END_STATE;
				token.TokenString.pop_back();
				token.TokenType = LEX_STRING;
				break;
			}


			if(ch == '\\')
			{
				LexState = STRING_ESCAPE;
				token.TokenString.pop_back();
				
				break;
			}


            if(!isprint(ch))
                 LEX_UNKNOWN_CHAR;
			break;

		case STRING_FOUND2:
			if(ch == '\"')
			{
				LexState = END_STATE;
				token.TokenString.pop_back();
			
				token.TokenType = LEX_STRING;
			}
			break;

			if(ch == '\\')
			{
				LexState = STRING_ESCAPE;
				token.TokenString.pop_back();
				
			}
			break;

            if(isprint(ch))
				LexState = CHAR_FOUND2;
			else
				return LEX_UNKNOWN_CHAR;
			break;

		case STRING_ESCAPE:
			if(ch == 'n')
			{
				token.TokenString.pop_back();
			 
				LexState = STRING_FOUND1;
				token.TokenString += '\n';
			}
			else
				return LEX_ILLEGAL_ESCAPE_CHAR;
			break;

//---------------------------------------------------------------------------
		case OPERATOR_FOUND:
		    switch(token.TokenString[0])
			{
			  case '!':
                if(ch == '=')
				{
					LexState = END_STATE;
					token.TokenType = LEX_OPERATOR;
					token.OprID = LEX_NE;
				}
				else
				{
					LexState = END_STATE;
					token.TokenType = LEX_OPERATOR;
					token.OprID = LEX_NOT;
				}
				break;

		      case '+':
				if(ch == '=')
				{
					LexState = END_STATE;
					token.TokenType = LEX_OPERATOR;
					token.OprID = LEX_INC;
				}
				else
				{
					LexState = END_STATE;
					token.TokenType = LEX_OPERATOR;
					token.OprID = LEX_ADD;
				}
				break;

			  case '-':
                if(ch == '=')
				{
                    LexState = END_STATE;
					token.TokenType = LEX_OPERATOR;
					token.OprID = LEX_DEC;
				}
				else
				{
			
				
					 if(isdigit(ch))
					 {
					
								LexState = END_STATE;
								token.TokenString.pop_back();
						        token.TokenType = LEX_OPERATOR;
						        token.OprID = LEX_NEG;
								Index--;
						
					}
                    else
                    {
                        LexState = END_STATE;
                        token.TokenType = LEX_OPERATOR;
                        token.OprID = LEX_SUB;
                    }
				}
				break;

			  case '*':

					LexState = END_STATE;
					token.TokenType = LEX_OPERATOR;
					token.OprID = LEX_MUL;

				break;

			  case '/':
				   
				   if(ch == '*')
                       {

								    LexState = COMMENT_FOUND2;
                                    token.TokenType = LEX_BEGIN_COMMENT;
                                    GetEndOfComment(Buffer, Index, ListingFile, token);
                        }
                     else
							 {
								 if(ch == '/')
								   {
								
									   
									   LexState = COMMENT_FOUND1;
									   token.TokenType = LEX_LINE_COMMENT;
									   GetEndOfComment(Buffer, Index, ListingFile, token);
 
									 }
								 else
								   {
										
									   LexState = END_STATE;
									  token.TokenType = LEX_OPERATOR;
									  token.OprID = LEX_DIV;
								   }
							  }
				
				break;
				

			  case '=':
				if(ch == '=')
				{
					LexState = END_STATE;
					token.TokenType = LEX_OPERATOR;
					token.OprID = LEX_EQ;
				}
				else
				{
					LexState = END_STATE;
					token.TokenType = LEX_ASSIGNMENT;
					token.OprID = LEX_NO_OPR;
				}
				break;

			  case '<':
                    if(ch == '=')
				{
					LexState = END_STATE;
					token.TokenType = LEX_OPERATOR;
					token.OprID = LEX_LE;
				}
				else
				{
                    if(ch == '<')
                    {
                        LexState = END_STATE;
                        token.TokenType = LEX_OPERATOR;
                        token.OprID = LEX_COUT_OPR;
                    }
                    else
                    {
                        LexState = END_STATE;
                        token.TokenType = LEX_OPERATOR;
                        token.OprID = LEX_LT;
                    }
				}
				break;

			  case '>':
                    if(ch == '=')
				{
					LexState = END_STATE;
					token.TokenType = LEX_OPERATOR;
					token.OprID = LEX_GE;
				}
				else
				{
                    if(ch == '>')
                    {
                        LexState = END_STATE;
                        token.TokenType = LEX_OPERATOR;
                        token.OprID = LEX_CIN_OPR;
                    }
                    else
                    {
                        LexState = END_STATE;
                        token.TokenType = LEX_OPERATOR;
                        token.OprID = LEX_GT;
                    }
				}
				break;
			}
			break;
		}  // End while
//---------------------------------------------------------------------------
		Index++;
		
   }
   
   string str;
     switch(token.TokenType)
                {
                    case LEX_OPERATOR :
                           	
							str =  token.TokenString + "              \t "+ TokenTypes_t_str[token.TokenType] + "              \t " + Operator_t_str[token.OprID]  + "\n";
                            break;
                     case LEX_KEYWORD :
                          
							str = token.TokenString + "              \t "+TokenTypes_t_str[token.TokenType] + "              \t " + Keyword_t_str[token.KeywordID]+" \n" ;
                            break;
					 case LEX_ASSIGNMENT :
						 str =  token.TokenString + "              \t "+ TokenTypes_t_str[token.TokenType] + "              \t " + Operator_t_str[token.OprID]  + "\n";
						 break ;
					 case LEX_BEGIN_COMMENT: break;
					 case LEX_LINE_COMMENT : break;
					 case LEX_END_COMMENT  : break;
                   
					 default :
						
                          
							str =  token.TokenString+ "              \t " + TokenTypes_t_str[token.TokenType] + " \n" ;
                            break;
                }
   pFile = fopen ("myfile.txt","a");

   fputs (str.c_str(),pFile); fclose (pFile);

	return LEX_NO_ERROR;
};


LexErrorCode_t GetEndOfComment (string Buffer, int &Index, FILE &ListingFile, token_t &token, bool testing)
{
	FILE * pFile ; 		
	pFile = fopen ("myfile.txt","a");
   string str;
   switch(token.TokenType)
   {
     case LEX_BEGIN_COMMENT : str =  token.TokenString + "              \t "+ TokenTypes_t_str[token.TokenType]  +"\n";
	 case LEX_LINE_COMMENT  : str =  token.TokenString + "              \t "+ TokenTypes_t_str[token.TokenType]  +"\n";
 
   }
    fputs (str.c_str(), pFile); 
    /////////////////////////////////////////////
	switch(LexState)
	{
	case  COMMENT_FOUND1 :
		while (Buffer[Index]!='\n')
			Index++;
		break;
	case  COMMENT_FOUND2 :
		do{
			while (Buffer[Index++]!='*');
		}
		while(Buffer[Index]!='/');token.TokenType=LEX_END_COMMENT;str =  "*/              \t "+ TokenTypes_t_str[token.TokenType]  +"\n"; fputs (str.c_str(), pFile);

		break;
	}

	fclose (pFile);
	LexState= END_STATE;

	return LEX_NO_ERROR;

};
