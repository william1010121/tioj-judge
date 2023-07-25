#include "example_problem.h"
#include "utils.h"

#include <gtest/gtest-matchers.h>

namespace {

constexpr long kTime = 1655000000;

}

TEST_F(ExampleProblem, SpecjudgeOldProblemOneSubmission) {
  kMaxParallel = 2;
  SetUp(2, 3);
  AssertVerdictReporter reporter(Verdict::AC);
  sub.reporter = reporter.GetReporter();
  sub.judge_between_stages = true;
  long id = SetupSubmission(sub, 5, Compiler::GCC_CPP_17, kTime, true, R"(#include <cstdio>
int main(){ puts("what"); })", SpecjudgeType::SPECJUDGE_OLD, R"(#include <cstdio>
#include "nlohmann/json.hpp"
int main(){ puts("0"); })");
  PushSubmission(std::move(sub));
  WorkLoop(false);
  TeardownSubmission(id);
}

TEST_F(ExampleProblem, SpecjudgeOldSetResult) {
  kMaxParallel = 2;
  SetUp(2, 3);
  AssertVerdictReporter reporter(Verdict::TLE);
  sub.reporter = reporter.GetReporter();
  auto orig_score = sub.reporter.ReportScoringResult;
  sub.reporter.ReportScoringResult = [&](auto& sub, auto& res, int subtask, int stage){
    orig_score(sub, res, subtask, stage);
    ASSERT_EQ(res.td_results[subtask].score, 1234567);
  };
  long id = SetupSubmission(sub, 5, Compiler::GCC_CPP_17, kTime, true, R"(#include <cstdio>
int main(){ puts("what"); })", SpecjudgeType::SPECJUDGE_OLD, R"(#include <cstdio>
int main(){ puts("0 SPECJUDGE_OVERRIDE_VERDICT TLE SPECJUDGE_OVERRIDE_SCORE 1.234567"); })");
  PushSubmission(std::move(sub));
  WorkLoop(false);
  TeardownSubmission(id);
}

TEST_F(ExampleProblem, SpecjudgeNewProblemOneSubmission) {
  kMaxParallel = 2;
  SetUp(3, 3);
  AssertVerdictReporter reporter(Verdict::AC);
  sub.reporter = reporter.GetReporter();
  auto orig_score = sub.reporter.ReportScoringResult;
  sub.reporter.ReportScoringResult = [&](auto& sub, auto& res, int subtask, int stage){
    orig_score(sub, res, subtask, stage);
    ASSERT_EQ(res.td_results[subtask].score, 1234567);
  };
  long id = SetupSubmission(sub, 6, Compiler::GCC_CPP_17, kTime, true, R"(#include <cstdio>
int main(){ puts("what"); })", SpecjudgeType::SPECJUDGE_NEW, R"(#include <iostream>
#include "nlohmann/json.hpp"
int main(){ std::cout << nlohmann::json{{"verdict", "AC"}, {"score", "1.234567"}}; })");
  PushSubmission(std::move(sub));
  WorkLoop(false);
  TeardownSubmission(id);
}

TEST_F(ExampleProblem, Multistage) {
  kMaxParallel = 2;
  SetUp(4, 2);
  AssertVerdictReporter reporter(Verdict::AC);
  sub.reporter = reporter.GetReporter();
  sub.stages = 3;
  // stage 0 +1, stage 2 -1
  long id = SetupSubmission(sub, 7, Compiler::GCC_CPP_17, kTime, false, R"(#include <cstdio>
int main(int argc, char** argv){ int a; scanf("%d",&a); printf("%d", a+argv[1][0]-'1'); })");
  PushSubmission(std::move(sub));
  WorkLoop(false);
  TeardownSubmission(id);
}

TEST_F(ExampleProblem, MultistageTL) {
  kMaxParallel = 2;
  SetUp(4, 2);
  AssertVerdictReporter reporter(Verdict::TLE);
  sub.reporter = reporter.GetReporter();
  sub.stages = 3;
  // use 0.6s per stage (TL 1s)
  long id = SetupSubmission(sub, 8, Compiler::GCC_CPP_17, kTime, false, R"(#include <ctime>
int main(int argc, char** argv){ auto a = clock(); while (clock()-a<0.6*CLOCKS_PER_SEC); })",
      SpecjudgeType::SPECJUDGE_NEW, R"(#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
int main(int argc, char**argv){
  std::ifstream fin(argv[1]); nlohmann::json data; fin >> data;
  std::cout << nlohmann::json{{"verdict", data["stats"]["original_verdict"].get<std::string>()}};
})");
  PushSubmission(std::move(sub));
  WorkLoop(false);
  TeardownSubmission(id);
}

TEST_F(ExampleProblem, MultistageSpecjudgeNew) {
  kMaxParallel = 2;
  SetUp(4, 2);
  AssertVerdictReporter reporter(Verdict::AC);
  sub.reporter = reporter.GetReporter();
  auto orig_score = sub.reporter.ReportScoringResult;
  sub.reporter.ReportScoringResult = [&](auto& sub, auto& res, int subtask, int stage){
    orig_score(sub, res, subtask, stage);
    ASSERT_EQ(stage, 2); // should run all 3 stages
  };
  sub.stages = 3;
  sub.judge_between_stages = true;
  // add stage+1 in execute; subtract it back in specjudge
  // if output nothing, default to continue
  long id = SetupSubmission(sub, 1, Compiler::GCC_CPP_17, kTime, false, R"(#include <cstdio>
int main(int argc, char** argv){ int a; scanf("%d",&a); printf("%d", a+argv[1][0]-'0'+1); })",
      SpecjudgeType::SPECJUDGE_NEW, R"(#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
int main(int argc, char**argv){
  std::ifstream fin(argv[1]); nlohmann::json data; fin >> data;
  int stage = data["current_stage"].get<int>();
  if (stage == 2) {
    int a, b;
    std::ifstream(data["user_output_file"].get<std::string>()) >> a;
    std::ifstream(data["answer_file"].get<std::string>()) >> b;
    if (a-stage-1 == b) {
      std::cout << nlohmann::json{{"verdict", "AC"}};
    }
  } else {
    int a;
    std::ifstream(data["user_output_file"].get<std::string>()) >> a;
    std::ofstream(data["user_output_file"].get<std::string>()) << (a - stage-1);
    if (stage == 1) std::cout << nlohmann::json{{"verdict", ""}};
  }
}
)");
  PushSubmission(std::move(sub));
  WorkLoop(false);
  TeardownSubmission(id);
}

TEST_F(ExampleProblem, MultistageSpecjudgeNewSkip) {
  kMaxParallel = 2;
  SetUp(4, 2);
  AssertVerdictReporter reporter(Verdict::AC);
  sub.reporter = reporter.GetReporter();
  sub.stages = 3;
  sub.judge_between_stages = true;
  auto orig_score = sub.reporter.ReportScoringResult;
  sub.reporter.ReportScoringResult = [&](auto& sub, auto& res, int subtask, int stage){
    orig_score(sub, res, subtask, stage);
    ASSERT_EQ(stage, 0); // should run only 1 stage
  };
  long id = SetupSubmission(sub, 1, Compiler::GCC_CPP_17, kTime, false, "int main(){}",
      SpecjudgeType::SPECJUDGE_NEW, R"(#include <cstdio>
int main(){ puts("{\"verdict\":\"AC\"}"); })");
  PushSubmission(std::move(sub));
  WorkLoop(false);
  TeardownSubmission(id);
}

TEST_F(ExampleProblem, MultistageSpecjudgeOld) {
  kMaxParallel = 2;
  SetUp(4, 2);
  AssertVerdictReporter reporter(Verdict::AC);
  sub.reporter = reporter.GetReporter();
  sub.stages = 3;
  sub.judge_between_stages = true;
  // same as MultistageSpecjudgeNew
  // if output nothing, default to continue
  long id = SetupSubmission(sub, 1, Compiler::GCC_CPP_17, kTime, false, R"(#include <cstdio>
int main(int argc, char** argv){ int a; scanf("%d",&a); printf("%d", a+argv[1][0]-'0'+1); })",
      SpecjudgeType::SPECJUDGE_OLD, R"(#include <iostream>
#include <fstream>
#include <string>
int main(int argc, char**argv){
  std::ifstream fin(argv[1]);
  int stage = std::stoi(std::string(argv[6]));
  if (stage == 2) {
    int a, b;
    std::ifstream(argv[1]) >> a;
    std::ifstream(argv[3]) >> b;
    if (a-stage-1 == b) std::cout << 0;
  } else {
    int a;
    std::ifstream(argv[1]) >> a;
    std::ofstream(argv[1]) << (a - stage-1);
    std::cout << 0;
  }
}
)");
  PushSubmission(std::move(sub));
  WorkLoop(false);
  TeardownSubmission(id);
}

TEST_F(ExampleProblem, MultistageSpecjudgeOldWA) {
  kMaxParallel = 2;
  SetUp(4, 2);
  AssertVerdictReporter reporter(Verdict::WA);
  sub.reporter = reporter.GetReporter();
  sub.stages = 3;
  sub.judge_between_stages = true;
  auto orig_score = sub.reporter.ReportScoringResult;
  sub.reporter.ReportScoringResult = [&](auto& sub, auto& res, int subtask, int stage){
    orig_score(sub, res, subtask, stage);
    ASSERT_EQ(stage, 0); // should run only 1 stage
  };
  long id = SetupSubmission(sub, 1, Compiler::GCC_CPP_17, kTime, false, R"(#include <cstdio>
int main(int argc, char** argv){ int a; scanf("%d",&a); printf("%d", a+argv[1][0]-'0'+1); })",
      SpecjudgeType::SPECJUDGE_OLD, "int main(){}");
  PushSubmission(std::move(sub));
  WorkLoop(false);
  TeardownSubmission(id);
}

TEST_F(ExampleProblem, UserCompileFlags) {
  SetUp(4, 2);
  AssertVerdictReporter reporter(Verdict::AC);
  sub.reporter = reporter.GetReporter();
  sub.user_compile_options = "-lgmpxx -lgmp";
  long id = SetupSubmission(sub, 1, Compiler::GCC_CPP_17, kTime, true, R"(#include <iostream>
#include <gmpxx.h>
int main() {
  mpz_class a;
  std::cin >> a;
  std::cout << (a * 1234567890123456789_mpz) / 1234567890123456789_mpz;
}
  )");
  PushSubmission(std::move(sub));
  WorkLoop(false);
  TeardownSubmission(id);
}

TEST_F(ExampleProblem, UserCompileFlagsSubstitution) {
  SetUp(4, 2);
  AssertVerdictReporter reporter(Verdict::CE, false);
  sub.reporter = reporter.GetReporter();
  sub.reporter.ReportCEMessage = [](auto&, const SubmissionResult& res) {
    ASSERT_TRUE(res.ce_message.find("multiple definition of `main") != std::string::npos);
  };
  sub.user_compile_options = "$INPUT";
  long id = SetupSubmission(sub, 1, Compiler::GCC_CPP_17, kTime, true, "int main(){}");
  PushSubmission(std::move(sub));
  WorkLoop(false);
  TeardownSubmission(id);
}

TEST_F(ExampleProblem, SpecjudgeCompileFlags) {
  SetUp(4, 2);
  AssertVerdictReporter reporter(Verdict::AC);
  sub.reporter = reporter.GetReporter();
  sub.specjudge_compile_options = "-lgmpxx -lgmp";
  long id = SetupSubmission(sub, 1, Compiler::GCC_CPP_17, kTime, true, "int main(){}",
      SpecjudgeType::SPECJUDGE_OLD, R"(#include <iostream>
#include <gmpxx.h>
int main(int argc, char**argv){
  if (argc * 1234567890123456789_mpz == argc * 1234567890123456789_mpz) std::cout << 0_mpz;
})");
  PushSubmission(std::move(sub));
  WorkLoop(false);
  TeardownSubmission(id);
}
