

/*  sqlparser.c  */  


/*
 *      SQL parser. performs a recursive descent parse 
 *      to evaluate an expression permissible
 *      within the following grammar:
 *
 *      expr    :       stmt SEMI
 *      stmt    :       select from ( where ) 
 *      select  :       SELECT ( STAR | field_list ) 
 *      star    :       *
 *      field_list  :   field_name ( COMMA field_name ) 
 *      field_name  :   identifier   
 *      from        :   FROM table_name  
 *      table_name  :   identifier              
 *      where_stmt  :   WHERE ( cond_stmt ) 
 *      cond_stmt   :   value OP value ( AND | OR cond_stmt )   
 *      value       :   field_name | int | string  
 *      semi        :   ;   
 * 
 */
 
 
/*   #include "sql.h"    */

 
char *nxtch;     /* Parser scan pointer */
 
  
/*
 *  macros:
 *  ungetch - Put back the last character examined.
 *  getch   - return the next character from expr string.
*/ 

#define ungetch()   nxtch--
#define getch()     *nxtch++

 
int expr(char *expbuf)
{

  



}

 
/* stmt : select from ( where ) */ 
int stmt()
{


     

}
 
 
 
/* select : SELECT ( STAR | field_list )  */
int select() 
{




}


/* star :  *  */  
int star() 
{ 
    
    
} 


int field_list() 
{ 
    
    
    
    
} 


    
int field_name() 
{ 
    
    

    
}     


int from() 
{ 
    
    
} 


int table_name() 
{ 
    
    
    
} 



int where_stmt() 
{ 
    
    
    
} 


    
int cond_stmt() 
{ 
    
    
    
}  



int value() 
{ 
    
    
    
} 


int semi() 
{ 
    
    
} 




int main(int argc, char *argv[]) { 
  if (argc < 2) error(usage);
  if (*argv[1] == '-') { debug = 1; --argc; ++argv; }
  if (argc < 2) error(usage);
  openinfile(argv[1]);
  init();
  if (!PROGRAM()) error("Syntax error");
  fclose(inf);
  
  return 0;   
  
}




    
    
         
            

