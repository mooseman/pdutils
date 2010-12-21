

#  gapbuffer.py  
#  A gap-buffer in Python. 
#  ( See http://en.wikipedia.org/wiki/Gap_buffer ) 

#  Gap buffers are often used in text editors.   
#  They divide the text into two strings with a 
#  gap at the position of the cursor (where text 
#  is inserted).   
#  This code is released to the public domain.  
#  "Share and enjoy..."  ;)  

#  TO DO  - add insert, delete, copy and move 
#  text operations.   
#  Also add find and replace operations.  
 
 
import string  

class gapbuffer(object):  
  def __init__(self, text): 
     self.text = text  
     self.cursorpos = len(self.text) 
     self.gapsize = 5 
     self.gap = "[" + " "*self.gapsize + "]" 
     self.text = self.text[0:self.cursorpos] + self.gap         
     
     
  def display(self):  
     print self.text 
          
  def cursorpos(self): 
     return self.cursorpos 
     
  # Create a gap at cursorpos.  
  def makegap(self):  
     self.text = self.text[0:self.cursorpos] + self.gap \
       + self.text[self.cursorpos:len(self.text)]       
     
  # Remove the gap before we move it    
  def removegap(self):    
     self.text = self.text[0:self.cursorpos] \
       + self.text[self.cursorpos+len(self.gap):len(self.text)]  

  # Insert some text 
  def insert_txt(self, text):  
     self.removegap()
     self.text = self.text[0:self.cursorpos] + text + self.gap \
       + self.text[self.cursorpos:len(self.text)]   
     self.cursorpos += len(text)  
     self.makegap()  
     
  # Remove text 
  def remove_txt(self, pos):  
     pass 

  # Copy text 
  def copy_txt(self):  
     pass 
     
  # Move text 
  def move_txt(self):  
     pass     


  # Show the character immediately before the gap.     
  def cursorchar(self): 
     return self.text[self.cursorpos-1]       
           
  def movecursor(self, num): 
     self.removegap() 
     self.cursorpos += num 
     self.makegap()  
     
  def buffer1(self):  
     return self.text[0:self.cursorpos]    
  
  def buffer2(self):  
     return self.text[self.cursorpos+len(self.gap):len(self.text)]     
  

#  Test the class  
a = gapbuffer("Mary had a little lamb, its fleece was white as snow")  

# Print the text 
a.display() 

# Prints the position of the cursor.  
print a.cursorpos
  
# Move the cursor   
a.movecursor(-14)   
print a.cursorpos   
  
# Print the character at the cursor 
print a.cursorchar() 

a.display()  
  
print a.buffer1() 
print a.buffer2()   
  
# Move the cursor again   
a.movecursor(-16)   
print a.cursorpos   
  
# Print the character at the cursor 
print a.cursorchar() 

a.display()  
  
print a.buffer1() 
print a.buffer2()   
  
# Insert some text    
a.insert_txt("Foobar")  

print a.buffer1() 
print a.buffer2()       
    







