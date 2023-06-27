#include<iostream>
#include<thread>
using namespace std;

//使用RAII机制确保主线程即使后续出现异常，主线程也会等待子线程运行至结束
class thread_guard{
    std::thread& t;
public:
    explicit thread_guard(thread& t_):t(t_){};

    ~thread_guard(){
        if(t.joinable()){
            t.join();
        }
    }
    thread_guard(thread_guard const&) = delete;
    thread_guard& operator=(thread_guard const&) = delete;
};

void task1(){
    cout << "子线程"<<endl;;
}


int main(){
    thread t(task1);
    thread_guard g(t);
    cout << "主线程"<<endl;
}
