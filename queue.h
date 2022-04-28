#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
#include <pthread.h>  
#include <string> 
#include <unistd.h>   
#include <semaphore.h>
#include <fstream>
#include <sstream>
#include<chrono>
#include <sys/mman.h>
using namespace std;
using namespace std::chrono;

/*
Struct: StudentNode
Purpose: Represents a student node
Variables: int for the ID
           int the amoung of time till the student is done eating
           int for the time it will take the student to eat
           int for the students total wait time

           two time points for the arrival time and the sit time
           student pointers to represent the previous and next node
Functions: N/A
*/
struct StudentNode
{
    int id;
    int burstTime;
    int eatingTimeLeft;
    
    time_point<high_resolution_clock> arrivalTime;

    StudentNode* next;
    StudentNode* prev;
};

/*
Struct: QueueMonitor
Purpose: Monitor for the waiter with helper functions
Variables: 3 semaphores to control the flow of the list
           student pointer 
           int representing the number of students
           int representing the number of students served
Functions: init
            Purpose: Initialize the semaphores and the linked list

           destroy
            Purpose: To deallocate the semaphores that we initialized

           pushBack
            Purpose: Adds a node to the end of the list

           popFront
            Purpose: Gets the first node of a list

           conditionWait
            Purpose: Waits for a condition to be signaled

           conditionPost
            Purpose: Signals that the condiiton is free

           getStudent
            Purpose: Gets the next student in line
            
           addStudentToQueue
            Purpose: Adds a student to the line

           getNumberOfStudents
            Purpose: Gets the amount of students in line

            **CODE BASED OFF OF ALEX'S A4_Boilerplate.cpp**
*/
struct QueueMonitor
{
    StudentNode* head;

    int numberOfStudents = 0;
    int studentsServed = 0;

    sem_t mutexSem;
    sem_t nextSem;
    int nextCount;

    sem_t conditionNonEmptySem;
    int conditionNonEmptyCount;

    void init();

    void destroy();

    void pushBack(StudentNode* curr);
     
    StudentNode * popFront();

    void conditionWait(sem_t &conditionSem, int &conditionCount);

    void conditionPost(sem_t &conditionSem, int &conditionCount);

    StudentNode *getStudent();

    void addStudentToQueue(StudentNode* currStudent);

    void getNumberOfStudents(string filename);
};

#endif