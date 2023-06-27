//使用多个线程分别对数组中的数据进行处理，最后将结果汇总输出

#include<future>
#include<vector>
#include<iostream>



//1.采用std::async从多个future中收集结果
auto process_data(std::vector<int>& vec){
    std::vector<std::future<int>> results;
    for(int& num:vec){
        results.push_back(
            std::async([num](){return num-2;})
        );
    }
    
    return std::async([&](){
        std::vector<int> output;
        for(auto& f:results){
            //使用此方式会导致但凡有分项结果得出，就会在该处反复唤醒，
            //但是如果还有其他任务未得出结果，又会再次休眠，导致过多的上下文切换
            //带来额外的开销
            output.push_back(f.get());
        }
        return output;
    });
}


int main(){
    std::vector<int> test_data{1,2,3,4,5,6,7};
    auto output = process_data(test_data).get();
    
    return 0;
}