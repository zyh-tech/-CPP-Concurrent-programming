    result_chunk process(data_chunk);
    std::vector<data_chunk>
    divide_into_chunks(data_block data, unsigned num_threads);
    void process_data(data_source &source, data_sink &sink) {
      unsigned const concurrency = std::thread::hardware_concurrency();
      unsigned const num_threads = (concurrency > 0) ? concurrency : 2;
      std::experimental::barrier sync(num_threads);
      std::vector<joining_thread> threads(num_threads);
      std::vector<data_chunk> chunks;
      result_block result;
      for (unsigned i = 0; i < num_threads; ++i) {
        threads[i] = joining_thread([&, i] {
          while (!source.done()) { // 6
            if (!i) { // 1
              data_block current_block =
                source.get_next_data_block();
                chunks = divide_into_chunks(
                  current_block, num_threads);
            }
            sync.arrive_and_wait(); // 2
            result.set_chunk(i, num_threads, process(chunks[i])); // 3
            sync.arrive_and_wait(); // 4
            if (!i) { // 5
              sink.write_data(std::move(result));
            }
          }
        });
      }
    } // 7