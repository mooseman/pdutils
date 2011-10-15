-- toysql.hs   
-- This code is aimed at parsing phrases like the 
-- following -  
-- select * from mytable where city = "Sydney"; 
-- select foo, bar from mytable where var1 > 10; 
-- select baz from mytable; 
-- This code is released to the public domain.  
-- "Share and enjoy....."  ;)  

module Main where 

import Text.ParserCombinators.Parsec

sqlStmt = do{ createStmt
            ; selectStmt 
            ; fromStmt
            ; (whereStmt) 
            ; semicolon  
            }  

createStmt = do{ CREATE 
               ; tablename 
               ; AS
               }  
                
tablename = identifier  


selectStmt = do{ SELECT 
               ; varStmt 
               } 
               
varStmt = star <|> singlevar <|> varlist 
star = '*'  
singlevar = identifier 
varlist = sepBy singlevar (char ',')   

fromStmt = do{ FROM 
             ; tableStmt 
             }  
             
tableStmt = singleTablestmt <|> multiTablestmt 
singleTablestmt = identifier 
multiTablestmt = sepBy identifier (char ',')  

whereStmt = do{ WHERE 
              ; condStmt 
              }  
              
condStmt = multiCondstmt <|> singleCondstmt 
singleCondstmt = singleCondstmtnoparens <|> singleCondstmtwithparens 

singleCondstmtnoparens = do{ 
                   ; singlevar 
                   ; OP 
                   ; value                    
                   }  

singleCondstmtwithparens = do( 
                   ;  char '(' 
                   ;  singleCondstmtnoparens 
                   ;  char ')'  
                   
multiCondstmt = sepBy singleCondstmt ( AND <|> OR )  

OP = ( '=' <|> '<' <|> '>' <|> "<=" <|> ">=" <|> "!=" )  
semicolon = char ';'

parseSQL :: String -> Either ParseError [[String]]
parseSQL input = parse sqlFile "(unknown)" input


