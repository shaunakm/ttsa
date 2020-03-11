output: double_rr_sched.o
	g++ double_rr_sched.o -o output

message.o: double_rr_sched.cpp
	g++ -cpp double_rr_sched.cpp

clean:
	rm *.o output
	rm *.out