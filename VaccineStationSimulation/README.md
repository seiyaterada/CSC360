# README for Vaccine Station Simulation

The goal of this project was to write a C program implementing a solution to solve a kind of producer-consumer problem and become more familiar with developing multi-threaded systems.
This file describes what the vaccine station simulation does, while also explaining the methodology used to solve the problem.

## Vaccination Problem

### Introduction

vaccine.c is a program which simulates a vaccination centre with one regestration desk (`reg_desk`) and multiple vaccination stations (`vac_station`).
The simulation was programmed with the uses of pthreads, mutexes, and conditiont variables. No semaphores were used. Through the process of this project,
 I was able to get a better understanding of how to use synchronization tools to solve problems, specifically using mutexes and condition variables.

 ### Running the Simulation

 #### Input

 The program accepts two command-line arguments when run in the shell (i.e. the number of vaccination stations for the simulation, and a simulation-events file). <br>
 As an example, here are one of the simulation-events files:
 ![image](https://github.com/seiyaterada/CSC360/assets/54200250/0303f5ad-d922-41aa-8d99-cf747ff1bdaa) <br>
 Each line above has the format:<br>
 ```<person_id>:<arrival_time>,<service_time>```<br><br>
`person_id` uniquely identifies someone arriving for vaccination, `arrival_time` and
`service_time` are in tenths of seconds. All values are integers. <br>

#### Output

The progress of each simulation is outputted to the console. An example of a possible output is given below (note the specific vaccination stations used by each person is possibly random and may change from run to run).
![image](https://github.com/seiyaterada/CSC360/assets/54200250/27f60a15-5a1e-447b-9347-2064d7491733)


### Functions and Methodology

#### reg_desk

 `reg_desk` is where people initially come to register for a vaccination session. It will read in an input file consisiting of `person_id`,
 `arrival_time`, and `service_time`. All of this information is added to the queue as an instance of `Person` and `Person_Info` with a first come first serve basis.
 These people added to the queue are then taken to the `vac_station`'s for vaccination. The `reg_desk` also broadcasts to all the `vac_station`'s once
 registration of all the people are done via a `pthread_cond_broadcast` and sets the `is_vac_completed` parameter to `true`. The enqueueing is done while holding the mutex so that there are no conflicts with the other threads trying to use the queue at the same time.

#### vac_station

 `vac_station` is where people that have been enqueued come get vaccinated. The `vac_station` are running in a loop, constantly looking for new people to vaccinate if they are currently not performing any tasks.
 The simulation of the vaccination is done by first locking the mutex so that the queue cannot be accessed by any other threads, then dequeueing the person being vaccinated.
 The mutex is then unlocked and `usleep` is used to simulate the service time, then the vaccination is complete and looks for another person to vaccinate. 
 If there are no more people in the queue to vaccinate, the `vac_station` will terminate its station. We make sure that before exiting a `vac_station`
  at any time, we signal the condition variable to make sure that the other `vac_station`'s are not in a deadlock waiting for the mutex. 

#### main

In the main function, all the arguments are validated and then the variables being used in the program is initialized. Once this is done, it will create the thread for the `reg_desk` and then create
however many threads which was given by the input, for the number of `vac_stations`. Then, `pthread_join` is called on the `vac_station` threads in order to wait until all the `vac_station`'s
are done vaccinating before exiting the program.

## References

[1] GradientGradient 1, CrowmanCrowman 23k55 gold badges3838 silver badges5454 bronze badges, szxszx 5, P.PP.P 107k1818 gold badges154154 silver badges210210 bronze badges, JerskaJerska 11.6k44 gold badges3131 silver badges5050 bronze badges, and SchmurfySchmurfy 1, “pthread_create and passing an integer as the last argument,” Stack Overflow, 01-May-1962. [Online]. Available: https://stackoverflow.com/questions/19232957/pthread-create-and-passing-an-integer-as-the-last-argument. [Accessed: 01-Jul-2021].<br>
[2]“How to create a simple thread in C,” Educative. [Online]. Available: https://www.educative.io/edpresso/how-to-create-a-simple-thread-in-c. [Accessed: 01-Jul-2021].<br>
[3]“The Little Book of Semaphores,” Green Tea Press. [Online]. Available: https://greenteapress.com/wp/semaphores/. [Accessed: 01-Jul-2021].
