#include "queue.h"

void QueueMonitor::init()
{
    head = NULL;

    sem_init(&mutexSem, 0, 1);
    sem_init(&nextSem, 0, 0);
    nextCount = 0;

    sem_init(&conditionNonEmptySem, 0, 0);
    conditionNonEmptyCount = 0;
}

void QueueMonitor::destroy()
{
    sem_destroy(&mutexSem);
    sem_destroy(&conditionNonEmptySem);
}

void QueueMonitor::pushBack(StudentNode* curr)
{
    curr -> next = NULL;
    curr -> prev = NULL;

    if(head == NULL)
    {
        head = curr;
    }
    else
    {
        StudentNode* temp = head;
        while(temp -> next != NULL)
        {
            temp = temp -> next;
        }
        temp -> next = curr;
        curr -> prev = temp;
    }
}
    
StudentNode * QueueMonitor::popFront()
{
    StudentNode* temp;
    if(head != NULL)
    {
        temp = head;
        head = head -> next;
        if(head != NULL)
        {
            head -> prev = NULL;
        }
    }
    return temp;
}

void QueueMonitor::conditionWait(sem_t &conditionSem, int &conditionCount)
{
    conditionCount++;
    if (nextCount > 0)
        sem_post(&nextSem);
    else
        sem_post(&mutexSem);
    sem_wait(&conditionSem);
    conditionCount--;
}

void QueueMonitor::conditionPost(sem_t &conditionSem, int &conditionCount)
{
    if (conditionCount > 0)
    {
        nextCount++;
        sem_post(&conditionSem);
        sem_wait(&nextSem);
        nextCount--;
    }
}

StudentNode * QueueMonitor::getStudent()
{
    sem_wait(&mutexSem);
    while(head == NULL)
    {
        conditionWait(conditionNonEmptySem, conditionNonEmptyCount);
    }

    StudentNode *temp = popFront();

    if(head != NULL)
    {
        conditionPost(conditionNonEmptySem, conditionNonEmptyCount);
    }

    if (nextCount > 0)
        sem_post(&nextSem);
    else
        sem_post(&mutexSem);

    return temp;
}

void QueueMonitor::addStudentToQueue(StudentNode* currStudent)
{
    sem_wait(&mutexSem);

    pushBack(currStudent);

    conditionPost(conditionNonEmptySem, conditionNonEmptyCount);
    
    if (nextCount > 0)
        sem_post(&nextSem);
    else
        sem_post(&mutexSem);
}

void QueueMonitor::getNumberOfStudents(string filename)
{
    string line;
    ifstream tempFile;

    tempFile.open(filename);
    getline(tempFile, line);
    while(getline(tempFile, line))
    {
        numberOfStudents++;
    }
}