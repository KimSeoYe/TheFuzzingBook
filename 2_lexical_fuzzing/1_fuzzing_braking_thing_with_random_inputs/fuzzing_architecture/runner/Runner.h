#ifndef RUNNER
#define RUNNER

#define PASS "PASS"
#define FAIL "FAIL"
#define UNRESOLVED "UNRESOLVED"

/**
 * problem.
 * Fuzzer와 RandomFuzzer는 모두 Fuzzer 구조체를 사용한다. (RandomFuzzer도 Fuzzer 구조체를 사용한다.)
 * Fuzzer와 RandomFuzzer의 run은 각각 Runner.run과 ProgramRunner.run을 사용한다.
 * 현재 Runner와 ProgramRunner의 멤버 변수가 달라서, Runner와 ProgramRunner를 따로 선언해둔 상태이다.
 * >> Fuzzer.run의 function pointer의 타입이 다르므로, RandomFuzzer가 Fuzzer 구조체를 그대로 사용할 수 없다.
 * 
 * ProgramRunner.run_process는 run에서 실행하기 위한 함수이므로 구조체에서 빼고,
 * Runner의 멤버 변수와 ProgramRunner의 멤버 변수를 통일해서
 * ProgramRunnerInit이 Runner 구조체를 initialize하도록 만드는 것은 어떨까?
 * >> risk : Runner 구조체 안에, Runner에서는 사용하지 않는 멤버 변수들이 들어갈 수 있다.
 * 
 * or, 
 * RandomFuzzerInit이 Fuzzer 구조체를 사용하도록 하지 말고,
 * RandomFuzzer 구조체를 따로 만들어서 사용하도록 하는게 나을 것 같다.
 * 
 * runner.h 하나에 Runner, PrintRunner, ProgramRunner를 모두 넣어두는 건 어떨까?
*/

typedef struct Runner {
    int input_size ;
    char * input ;
    char * outcome ;

    void (* run) (struct Runner *, char *, int) ;
    void (* free_input) (struct Runner) ;
} Runner ;

void run (Runner * self, char * input, int input_size) ;

void free_input (Runner runner) ;

void RunnerInit (Runner * runner) ;  // naming..?

#endif