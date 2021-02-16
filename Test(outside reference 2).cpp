
    #include <fstream>
    #include <iostream>
    #include <vector>
    #include <string>
    #include "Token.h"
    using namespace std;


	string ReadFile(string filename)
	{
		string contenu ;  
		 ifstream fichier(filename, ios::in ); 
   
		 if(fichier)
		{
	       string ligne;
	     
		   while(getline(fichier, ligne)) 
			  {
                contenu +=  ligne+ '\n' ;
			  }
		   return contenu;
		}		
        else
                cerr << "Couldnt open the file" << endl;
  
  
	}
  

 int main( int argc, char * argv [] )
    {

          string FileContent;
		  string fileName;
		
		  int index =0;
		  
		  token_t tok;
		 
		  cout<<" Enter your filename <ex : Demo1.cpp > : ";
		  cin>>fileName ;
      
		 FileContent = ReadFile(fileName);
			FILE * pFile ;
	        pFile = fopen ("myfile.txt","w");
            
        
			InitKeywordMap();
		

			while(index <= FileContent.length()-1)
            {
			    try{
                
			      	GetToken(FileContent, index, *pFile, tok); 

						if(index == FileContent.length()-1)
							return 0;
			       }
			     catch(InterpExc exc){cout <<"\nError " << exc.get_err(); break;}
              
            }
			 
			fclose (pFile);
      

    }



