
# `src/server_io.cpp:160`:`Class TIOJClient`
- the `websocket` client
- `OnMessage` will receive the submission code

# `src/server_io.cpp:367`:`DealOneSubmission`
- set for the `submission` basic data, also the `specjudge type`



# `tioj-judge/include/tioj/submission.h`
- save for the basic infomation of submission

# `tioj-judge/src/tioj/tasks.cpp`
- compile the code

## `tasks.cpp:227:RunScoring`
**hilight**
- run the `special judge` code
1. `SPECJUDGE_OLD`
2. `SPECJUDGE_NEW`
3. `SPECJUDGETYPE::NORMAL`

# `tioj-judge/src/tioj/sandbox_exec.h`
- run the code

## `sandbox_exec.cpp:18-23`
- `dup2(a,b)` will redirect the pipe`a` to pip`b`
- `1` stands for stdout
- `0` stands for stdin
- the father process can send the input write at `outpipe[1]`
and receive the data at `inpipe[0]`, 
the child process can send the output at `inpipe[1]` and receive the input at `output[0]`

# `tioj-judge/src/tioj/submission.cpp`
## `submission.cpp:376`:`FinalizeExcute`
- for compile and output the `Verdict`

## `sumbmission.cpp:777`:`DispatchTask`
- for dispatch the judge process to 4 subprogress
1. `SetupCompile`
2. `SetupExecute`
3. `SetupScoring` : for specail judge
4. `SetupSummary`


## `sumbmission.cpp:442`:`SetupScoring`

- set for scoring 

## `submission.cpp:575`:`FinalizeScoring`
**hilight**
- for `Old` and `New` specail judge result

**submission.cpp:596** is our target


## `submission.cpp:261`:`FinalizeCompile`
- set for compile the code

## `submission.cpp:209`:`SetupCompile`
- set up the env of the task




## `submission:793`:`int handle`
- using run `task` to run 4 of
1. `compile`
2. `execute`
3. `scoring`
4. `summary`
task,

## `submission:850` : `PushSubmission`
- the main process for the `submission`
- the source 




# need to change

[yes] 1. `/usr/local/include/tioj/submission.h:19` -> add new`ENUM_SPECJUDGE_TYPE`

[yes] 2. `src/tioj/tasks.cpp:227` -> add new command sent into `sub.specjudge_type`
    - sending `argv` into testlib

[yes] 3. `src/tioj/submission.cpp:457` -> jump out when `TLE/MLE` depends on the specjudge type

[yes] 4. `src/tioj/submission.cpp:595` -> reading the return value of specjudge type

[yes] 5. `src/tioj/submission.cpp:934` -> add the type of specjudge


# `tioj`

## `/home/william/tioj/app/channels/fetch_channel.rb`

- send the data

## `/home/william/tioj/app/channels/application_cable/connection.rb`

- connect to the `judge` and can be identify by user, contest, judge_server


## `app/model/problem.rb:51`

- add `specjudge type` in `model`

## `app/helpers/problem_helper.rb:78` : `specjudge_type_desc_map`

- 

