-- toysql.hs   

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
singleCondstmt = do{ singlevar 
                   ; OP 
                   ; value 
                   }  
                   
multiCondstmt = sepBy singleCondstmt ( AND <|> OR )  

OP = ( '=' <|> '<' <|> '>' <|> "<=" <|> ">=" <|> "!=" )  
semicolon = char ';'

parseSQL :: String -> Either ParseError [[String]]
parseSQL input = parse sqlFile "(unknown)" input


