/*
std::experimental::when_any()针对多个future,此函数创建一个新的future，并与他们
有所关联，若其中一个就绪，此新的future也会随之就绪。
他返回std::experimental::when_any_result<>而产生的内部实例，改实例由一个序列和一个索引值
组成，其中序列包含传入的全体future，索引值表明哪个就绪
*/

    std::experimental::future<FinalResult>
    find_and_process_value(std::vector<MyData> &data)
    {
      unsigned const concurrency = std::thread::hardware_concurrency();
      unsigned const num_tasks = (concurrency > 0)? concurrency : 2；
      std::vector<std::experimental::future<MyData *>> results;
      auto const chunk_size = (data.size() + num_tasks - 1) / num_tasks;
      auto chunk_begin = data.begin();
      std::shared_ptr<std::atomic<bool>> done_flag = 
          std::make_shared<std::atomic<bool>>(false);
      for (unsigned i = 0; i < num_tasks; ++i){ // 1
        auto chunk_end = 
          (i < (num_tasks - 1)? chunk_begin + chunk_size : data.end());
        results.push_back(spawn_async([=]{ // 2
          for (auto entry = chunk_begin;
               !*done_flag && (entry != chunk_end);
               ++entry){
            if (matches_find_criteria(*entry)){
              *done_flag = true;
              return &*entry;
            }
          }
          return (MyData *)nullptr;
        }));
        chunk_begin = chunk_end;
      }
      std::shared_ptr<std::experimental::promise<FinalResult>> final_result =
        std::make_shared<std::experimental::promise<FinalResult>>();
      struct DoneCheck {
        std::shared_ptr<std::experimental::promise<FinalResult>>
          final_result;
      DoneCheck(
        std::shared_ptr<std::experimental::promise<FinalResult>>
          final_result_)
        : final_result(std::move(final_result_)) {}
      void operator()( // 4
        std::experimental::future<std::experimental::when_any_result<
          std::vector<std::experimental::future<MyData *>>>>
            results_param) {
        auto results = results_param.get();
        MyData *const ready_result =
          results.futures[results.index].get(); // 5
        if (ready_result)
          final_result->set_value( // 6
            process_found_value(*ready_result));
        else {
          results.futures.erase(
            results.futures.begin() + results.index); // 7
          if (!results.futures.empty()) {
            std::experimental::when_any( // 8
              results.futures.begin(), results.futures.end())
              .then(std::move(*this));
          } else {
            final_result->set_exception(
              std::make_exception_ptr( // 9
                std::runtime_error(“Not found”)));
          }
        }
      };
      std::experimental::when_any(results.begin(), results.end())
      .then(DoneCheck(final_result)); // 3
      return final_result->get_future(); // 10
    }