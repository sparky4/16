?!
This  is a quick C++ tutorial, first in C++ you can use C files, you make a C  file and give it extension .c (if no header), next to use it in your  C++ you include it as follows
#extern "C" {
     #include "myfile.c"
}
I don't remember if it's # for extern anyway...


In  C++ and well, C too, you use macros and stuff and do things to the  compiler does not goof, one typical construction is "if (something) has  been defined, then add this code, and after that code end if" for  example it looks like this:
#ifdef WIN32
     #include <windows.h>
#endif

And you get different commands too like #ifndef which means if it is not yet defined, and #else, and #define  which defines something, and so on. So another useful construction you  can use for C++ to avoid stuff like confusing the compiler into  including code more than once is by saying, well if you haven't included  it yet, then include it please, or rather, if you haven't defined my  flag,...
...then  define my flag, and then add my code, and then close that block. So  next time it looks at the same file it goes, oh I already have that one,  ok next!
You do it like this
#ifndef _FILENAME_H_
#define _FILENAME_H_
//code here
#endif
The  _FILENAME_H_ is just supposed to be something unique, so usually when  you write out a filename with caps and underscore you are guaranteed a  unique flag that wont mess up your code. Remember #define defines something AT COMPILE TIME, not runtime.


In  C++ you really, really, really have to separate your code more than in  maybe C. But the same rules apply, you have to alert the compiler of a  function before you implement the function. For example:
void myfunc();
int main()
{
     myfunc();
     return 0;
}
void myfunc()
{
    printf("huehuehuehue");
}
In  C++ you do this also, this is for the compiler otherwise it doesn't  know what the hell you are doing you moron. But it is also for yourself  and other people. This is because you might want to change exactly how  your functions do what you want them to do, but you don't necessarily  want to change that you have a function that is supposed to do something  important. That is why you declare...
...everything  as "in the best situation I have this function, it takes these  arguments and returns this to me" and separate that from how you  actually do it.
The  declarations all go in header files with an extension .h and are  included in all the files that use the code, including the file that  implements the functions. The file that implements the functions has  extension .cpp and you never include this file anywhere. NEVER EVER!   Because these are your implementations, not your descriptions. You only  include the headers, in their respective .cpp...
...file,  hopefully with the SAME NAME except for extension, and include only the  header also in any file that uses that code too.
The header (.h)  is the file you want to start with the #ifndef, #define, and end with #endif,  stuff. Not the implementation file (.cpp), if it's only in the header  it works find, if it's in the .cpp too then you will probably break it  at compile time.
A  class is like a struct, or object. Think of them like defining your own  types, with complex properties/members, and functions/methods. An  example of using classes, already made could be this:
#include "cheetamen.h"
int main()
{
     Cheetamen cheeta_one; //one cheetamen
     Cheetamen cheeta_two; //two cheetamen
     cheeta_one.happy();
     cheeta_one.nr = 57;
     cheeta_two.sad();
     cheeta_two.nr = 12;
     printf("Cheetaman %d is very %s", cheeta_one.nr, cheeta_one.get_state());
     printf("Cheetaman %d is very %s", cheeta_two.nr, cheeta_two.get_state());
     return 0;
}
And that would return
Cheetaman 57 is very happy
Cheetaman 12 is very sad
Something like that, depending on what get state does.
You define instances of a class with new, and get free up memory with delete.
Cheetaman *cheeta;
cheeta = new Cheetaman(1, "happy");
cheeta = new Cheetaman();
delete(cheeta);
You  now have a memory leak you baka!!  Ok remember to delete everything  after using it, or save a pointer to it somewhere you can find it again  later.
So  a class is like a struct but instead it is called class, with access  modifiers, public so anything can be accessed, protected it can be  messed with, and private it can only be affected internally by the class  itself. This is important. If you do this:
cheeta.nr = 7;
/*Then  you set the nr member to 7, but if nr is private you can't, so you need  a function part of Cheetamen to update it. The reason for this is  because you can tell other people what to do, you can only ask them to  do something.*/
Add classes to a namespace.
namespace hereisaname {
 // all functions and stuff in the name space
}
You  can declare namespace in other files, it just adds them to the  namespace, this way you can have a lot of similar functions and stuff in  different namespaces. It qualifies their unique names to (namespace  name)::(member name) so it works like a prefix, I guess?  Anyway moving  on.
class Cheetaman {
public:
    //this is a class
    Cheetaman(); //default constructor, this is always called, unless you use another constructor
     Cheetaman(int x, int y); //overloaded constructor, this is called  when you put two ints in, so this is called instead of default.
    void happy();
    void sad();
    const char * get_state();
     int nr;
    ~Cheetaman();
private:
    const char *state;
};
That  is your declaration of the class, it has two constructors, a default  and one overloaded, and some functions, and variables. When you use a  class you can use the dot operator to use the members .  like this:
Cheetaman cheeta;
cheeta.happy(); //note the dot...
For  your implementation (.cpp) you use the include the (.h) and use the  namespace too, and then define them statically as members I guess you  could say. So Classname::membername like this:
#include "cheetaman.h"
namespace Cheetas {
     Cheetaman::Cheetaman()
     {
       //default, set all the standard values for member variables and init code here
      }
      Cheetaman::Cheetaman(int x, int y)
      {
             //do extra stuff with x y
      }
     
     void Cheetaman::happy()
     {
      this->state = new //well I forget the specifics for const char types but you get the point...
      this->state = "happy";
     }
     
     const char* Cheetaman::get_state()
     {
            return this->state; //you return that state member of the  actual class, you see?  So it can change (happy/sad) but you always  return the correct one.
     }
     //and so on and so on.
     Cheetaman::~Cheetaman()
     {
          //deconstructor, delete all memory allocated here
      }
}
this-> is used in the class as . is use outside of the class.
And that is more or less the gist of it. Oh yeah and you can inline assembly too, and use boolean types (bool) and so on. 
sparky4
sparky4!!!!!
sparky4: put in a file and upload it!
sparky4: put in a file and upload it!
sparky4: put in a file and upload it!
sparky4 no I am lazy
http://boards.4chan.org/jp/res/9468991
sparky4
sparky4 >/jp/ confirmed for having the hottest white girls
sparky4 I knew I should had tried to become a /jp/ idol.
sparky4: http://piratepad.net/INvoPrHMSN paste here
sparky4: mlet me get some drugs~
^
|
recursive chatlog
