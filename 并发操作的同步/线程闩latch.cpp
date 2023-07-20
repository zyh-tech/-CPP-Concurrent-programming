/*
每当等待的目标事件发生，就令latch done(thread_count)上的计数减1
*/


void foo(){
  unsigned const thread_count=...;
  latch done(thread_count); // 1
  my_data data[thread_count];
  std::vector<std::future<void> > threads;
  for(unsigned i=0;i<thread_count;++i)
    threads.push_back(std::async(std::launch::async,[&,i]{ // 2
      data[i]=make_data(i);
      done.count_down(); // 3
      do_more_stuff(); // 4
    }));
  done.wait(); // 5
  process_data(data,thread_count); // 6
} // 7