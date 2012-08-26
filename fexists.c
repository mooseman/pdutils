

/*  fexists.c  */  
/*  Look for a file in the current directory. */ 
/*  Return 0 if found, and -1 if not found.  */ 
/*  This code is released to the public domain. */  
/*  "Share and enjoy....."  ;)    */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*  USAGE:                                        */  
/*  On the command line ( $ is command prompt) -  */ 
/*  $ ./fexists foo.c                             */ 
/*  OR  -                                         */  
/*  $ ./fexists "foo.c"                           */  
/*  Either method should work. They do for me.    */   


int fexists(char *fname)
{  

   FILE *fptr ;  	
	
   fptr = fopen(fname, "r") ;  
   
   if ( !fptr )  return -1 ;  /* File does not exist in dir. */ 	
	 
   fclose(fptr);  
   return 0;    /* File DOES exist in dir.  */  	

} 



int main( int argc, char *argv[]) 
{


/*  See if a file exists.  */ 
printf("Result is %d \n", fexists(argv[1]) ) ; 

return 0; 


}  


	
