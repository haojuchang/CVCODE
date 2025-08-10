===== 1. build cases  =====
cd pp/cases
make

===== 2. run program =====
cd pp
make
make case c=$(case_num) max_thread=$(max_thread) input_size=$(input_size)

input_size is as 2's exponent
