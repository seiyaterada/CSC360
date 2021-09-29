# README for Vac_Station Simulation

This file first describes what vaccine.c does for part-a and then describes meetup.c and meetup.h <br>
for part-b.

## Vaccination Problem

### Introduction

<p>vaccine.c is a program which simulates a vaccination centre with one regestration desk (`reg_desk`) and multiple vaccination stations (`vac_station`). <br>
The simulation was programmed with the uses of pthreads, mutexes, and conditiont variables. No semaphores were used. Through the process of this part of the assignment,<br>
 I was able to get a better understanding of how to use synchronization tools to solve problems, specifically using mutexes and condition variables.</p>

### reg_desk

 <p>`reg_desk` is where people initially come to register for a vaccination session. It will read in an input file consisiting of `person_id`,<br>
 `arrival_time`, and `service_time`. All of this information is added to the queue as an instance of `Person` and `Person_Info` with a first come first serve basis.<br>
 These people added to the queue are then taken to the `vac_station`'s for vaccination. The `reg_desk` also broadcasts to all the `vac_station`'s once<br>
 registration of all the people are done via a `pthread_cond_broadcast` and sets the `is_vac_completed` parameter to `true`. The enqueueing is done while<br> holding the mutex so that there are no conflicts with the other threads trying to use the queue at the same time.</p>

### vac_station

 <p>`vac_station` is where people that have been enqueued come get vaccinated. The `vac_station` are running in a loop constantly looking for new people to vaccinate if they are currently not performing any tasks.<br>
 The simulation of the vaccination is done by first locking the mutex so that the queue cannot be accessed by any other threads, then dequeueing the person being vaccinated.<br>
 The mutex is then unlocked an `usleep` is used to simulate the service time, then the vaccination is complete and looks for another person to vaccinate. <br>
 If there are no more people in the queue to vaccinate, the `vac_station` will terminate its station. We make sure that before exiting a `vac_station`<br>
  at any time, we signal the condition variable to make sure that the other `vac_station`'s are not in a deadlock waiting for the mutex. </p>

### main

<p>In the main function, all the arguments are validated and then the variables being used in the program is initialized. Once this is done, it will create the thread for the `reg_desk` and then create<br> 
however many threads which was given by the input, for the number of `vac_stations`. Then, `pthread_join` is called on the `vac_station` threads in order to wait until all the `vac_station`'s<br>
are done vaccinating before exiting the program.

## References

[1] GradientGradient 1, CrowmanCrowman 23k55 gold badges3838 silver badges5454 bronze badges, szxszx 5, P.PP.P 107k1818 gold badges154154 silver badges210210 bronze badges, JerskaJerska 11.6k44 gold badges3131 silver badges5050 bronze badges, and SchmurfySchmurfy 1, “pthread_create and passing an integer as the last argument,” Stack Overflow, 01-May-1962. [Online]. Available: https://stackoverflow.com/questions/19232957/pthread-create-and-passing-an-integer-as-the-last-argument. [Accessed: 01-Jul-2021].<br>
[2]“How to create a simple thread in C,” Educative. [Online]. Available: https://www.educative.io/edpresso/how-to-create-a-simple-thread-in-c. [Accessed: 01-Jul-2021].<br>
[3]“The Little Book of Semaphores,” Green Tea Press. [Online]. Available: https://greenteapress.com/wp/semaphores/. [Accessed: 01-Jul-2021].
