//future对象可以在线程间传递，使得一个计算任务依赖于另一个任务的计算结果，却不必显示访问共享数据


#include<list>
#include<algorithm>
#include<future>
#include<iostream>


template<typename T>
std::list<T> parallel_quick_sort(std::list<T> input){
    if(input.empty()) return input;
    std::list<T> result;

    //将input的首元素剪切到result中作为用于交换的基准元素
    result.splice(result.begin(),input,input.begin());
    T const& pivot = *result.begin();

    //按照比基准元素大还是小将链表分为两部分，分界点为divide_point
    auto divide_point = std::partition(input.begin(),input.end(),[&](T const& t){
        return t < pivot;
    });

    //将input中的前半部分剪切出来作为lower_part
    std::list<T> lower_part;
    lower_part.splice(lower_part.end(),input,input.begin(),divide_point);

    //开启一个异步线程对lower_part进行排序
    std::future<std::list<T>> new_lower(
        std::async(&parallel_quick_sort<T>,std::move(lower_part))
    );

    //本线程对higher_part进行排序
    auto new_higher(
        parallel_quick_sort(std::move(input))
    );

    //将两部分排序结果拼接到result中
    result.splice(result.end(),new_higher);
    result.splice(result.begin(),new_lower.get());
    return result;
}


int main(){
    std::list<int> ls {3,5,8,2,4,1};
    std::list<int> res = parallel_quick_sort(ls);

    for(auto& num:res){
        std::cout << num << std::endl;
    }

    return 0;
}