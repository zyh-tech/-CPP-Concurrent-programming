#include<iostream>
#include<thread>
using namespace std;
//线程内部具有存储空间，参数会按照默认方式复制到该处，
//然后这些副本被当成临时变量，以右值的形式传给新线程上的函数或可调用对象

void f(const int& i, const string& s){
    cout << "f运行" <<endl;
}

int main(){
    string str = "hello";
    thread t(f,3,str);
    t.join();
    return 0;