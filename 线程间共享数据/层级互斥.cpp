#include<iostream>
#include<mutex>
#include<climits>
/*
对于thread_local，官方解释是：

thread_local 只对声明于命名空间作用域的对象、声明于块作用域的对象以及静态数据成员允许。
它指示对象拥有线程存储期。它能与 static 或 extern 结合，
以分别指定内部或外部链接（除了静态数据成员始终拥有外部链接），但附加的 static 不影响存储期。

线程存储期：对象的存储在线程开始时分配，而在线程结束时解分配。
每个线程拥有其自身的对象实例。唯有声明为 thread_local 的对象拥有此存储期。
thread_local 能与 static 或 extern 一同出现，以调整链接。

*/

class hierarchical_mutex{
    std::mutex internal_mutex;
    unsigned long const hierarchy_value;
    unsigned long previous_hierarchy_value;

    //当前线程的层级编号
    static thread_local unsigned long this_thread_hierarchy_value;

    void check_for_hierarchy_violation(){
        if(this_thread_hierarchy_value <= hierarchy_value){
            throw std::logic_error("mutex hierarchy violated");
        }
    }

    void update_hierarchy_value(){
        previous_hierarchy_value = this_thread_hierarchy_value;
        this_thread_hierarchy_value = hierarchy_value;
    }

public:
    explicit hierarchical_mutex(unsigned long value):
        hierarchy_value(value),previous_hierarchy_value(0){};
        
    void lock(){
        check_for_hierarchy_violation();
        //内部的互斥加锁成功后则可以更新层级
        internal_mutex.lock();
        update_hierarchy_value();
    }

    void unlock(){
        if(this_thread_hierarchy_value != hierarchy_value)
            throw std::logic_error("mutex hierarchy violated");
        //如果冲突，复原
        this_thread_hierarchy_value = previous_hierarchy_value;
        internal_mutex.unlock();
    }

    bool try_lock(){
        check_for_hierarchy_violation();
        if(!internal_mutex.try_lock()) return false;
        update_hierarchy_value();
        return true;
    }
};

thread_local unsigned long hierarchical_mutex::this_thread_hierarchy_value(ULONG_MAX);