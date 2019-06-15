main: inference.o matrix_op.o param_load.o frontend.o bm.o txt2label.o questions.o
	g++ -o main inference.o matrix_op.o param_load.o frontend.o bm.o txt2label.o questions.o
matrix_op.o: matrix_op.cpp matrix_op.h
	g++ -c matrix_op.cpp -std=c++11
questions.o: questions.cpp questions.h
	g++ -c questions.cpp -std=c++11
param_load.o: param_load.cpp param_load.h
	g++ -c param_load.cpp -std=c++11
bm.o: bm.cpp bm.h param_load.h
	g++ -c bm.cpp -std=c++11
frontend.o: frontend.cpp frontend.h matrix_op.h bm.h
	g++ -c frontend.cpp -std=c++11
inference.o: inference.cpp matrix_op.h param_load.h frontend.h
	g++ -c inference.cpp -std=c++11
txt2label.o: txt2label.cpp txt2label.h questions.h questions.cpp
	g++ -c txt2label.cpp -std=c++11
clean:
	rm *.o
