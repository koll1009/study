
#include <stdio.h>
#include "test.h"

using namespace std;
class class1{
public:
    explicit class1(int a):a_(a){}
    virtual bool test() { return true;}
private:
    int a_;
};

class class2{
public:
    explicit class2(int b):b_(b){}
    virtual bool test1() { return true;}
private:
    int b_;
};

class class3:public class1, public class2
{
public: 
    class3(int a, int b,int c):class1(a),class2(b), c_(c){}
    string toString()
    {
        return "bool";
    }  
private:
    int c_;
};
int main(int argc, char **argv)
{
    func1(4);
    int a = 4;
    func1(a);
    int& b = a;
    func1(b);
    auto test = [](void) ->int {return 4;};
    func1(test());
    class3 t(1,2,3);
    printf("%s\n",t.toString().c_str());
    return 0;
}