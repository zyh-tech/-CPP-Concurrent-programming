#include<iostream>
#include<thread>
using namespace std;

class background_task{
public:
    void operator()() const{
        cout << "函数对象 hello" <<endl;
    }
};

void task1(){
    cout << "函数指针 hello"<<endl;;
}


int main(){
    background_task f;
    thread my_thread1(f);
    my_thread1.join();

    thread my_thread2(task1);
    my_thread2.join();

    thread my_thread3([]{
        cout << "函数指针 hello"<<endl;;
    });    
    my_thread3.join();
    return 0;
}