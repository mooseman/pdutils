


# sep_classes_update.py 
# This code is released to the public domain. 
# "Share and enjoy..... ;) "  

# A note to myself that a class in Python 
# can update another class and they do not 
# have to be related. 
# Here is an example -  

class foo(object): 
  def __init__(self): 
     self.dict = {}
     
class bar(object): 
  def __init__(self): 
     self.data = None 
  def update(self, somedict, key, val): 
     somedict.update({key: val})       


# Create instances of the classes 
a = foo() 
b = bar() 

# Use b to update the dict in a 
b.update(a.dict, "foo", 42) 
b.update(a.dict, "bar", 123) 

# Print the updated dict from a 
print a.dict 


 
