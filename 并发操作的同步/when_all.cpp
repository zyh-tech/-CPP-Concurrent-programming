/*
使用std::experimental::when_all()函数等待future1全部就绪，
然后使用then编排后续函数，而非std::async,

*/

    std::experimental::future<FinalResult> process_data(
      std::vector<MyData>& vec)
    {
      size_t const chunk_size = whatever;
      std::vector<std::experimental::future<ChunkResult>> results;
      for (auto begin = vec.begin(), end = vec.end(); beg != end){
        size_t const remaining_size = end - begin;
        size_t const this_chunk_size = std::min(remaining_size, chunk_size);
        results.push_back(
          spawn_async(
            process_chunk, begin, begin+this_chunk_size));
        begin += this_chunk_size;
      }
      return std::experimental::when_all(
        results.begin(), results.end()).then( // 1
          [](std::future<std::vector<std::experimental::future<ChunkResult>>> ready_results){
            std::vector<std::experimental::future<ChunkResult>> all_results = ready_results.get();
            std::vector<ChunkResult> v;
            v.reserve(all_results.size());
            for (auto& f: all_results){
              v.push_back(f.get()); // 2
            }
            return gather_results(v);
          });
    }

/*
std::experimental::when_any()针对多个future,此函数创建一个新的future，并与他们
有所关联，若其中一个就绪，此新的future也会随之就绪。
他返回std::experimental::when_any_result<>而产生的内部实例，改实例由一个序列和一个索引值
组成，其中序列包含传入的全体future，索引值表明哪个就绪
*/