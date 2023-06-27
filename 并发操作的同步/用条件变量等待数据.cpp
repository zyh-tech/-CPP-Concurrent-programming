#include<mutex>
#include<queue>
#include<condition_variable>

std::mutex mut;
std::queue<int> data_queue;
std::condition_variable data_cond;

void data_preparation_thread(){
    while(more_data_to_prepare()){
        int data = prepare_data();{
            std::lock_guard<std::mutex> lk(mut);
            data_queue.push(data);
        }
        data_cond.notify_one();
    }
}

void data_processing_thread(){
    while(true){
        std::unique_lock<std::mutex> lk(mut);

        //wait检查条件是否成立，如果成立就返回，继续向下运行
        //否则就解锁，进入阻塞状态等待
        //当其他线程调用notify_one/notify_all时，重新获取锁，再次检查条件
        data_cond.wait(lk,[]{return !data_queue.empty();});
        int data = data_queue.front();
        data_queue.pop();
        lk.unlock();
        process(data);
        if(is_last_data(data)) break;
    }
}