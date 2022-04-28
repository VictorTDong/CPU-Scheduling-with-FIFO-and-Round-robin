/*
Purpose: Cpu scheduling with round robin
Known bugs: None
Limitations:
Usage: To make and run code
            Case 1:
            make all
            ./round-robin <Filename>
                     
       To clean files
            make clean
*/

#include "queue.h"

int* timeQuantum;
struct QueueMonitor queue;
sem_t genMutex;
sem_t preemptMutex;

/*
Function: addStudentFunction
Purpose: Add students to the queue
Method: Reads a file and adds the student based on the time they come to the queue which places them in line to recieve a table
Input: void pointer that points to a string with the file name
Output: N/A

*/
void *addStudentFunction(void * arg)
{
    ifstream file;
    string line;

    string filename = *static_cast<string*>(arg);

    file.open(filename);

    getline(file,line);
    stringstream ss(line);
    ss >> *timeQuantum;

    int id = 1;
    int timestamp;
    int eatingTimeLeft;

    while(getline(file,line))
    {
        stringstream ss(line);
        ss >> timestamp >> eatingTimeLeft;

        StudentNode *currStudent = new StudentNode();

        currStudent -> id = id;
        currStudent -> eatingTimeLeft = eatingTimeLeft;
        currStudent -> burstTime = eatingTimeLeft;

        sleep(timestamp);
        
        printf("Arrive %i \n", id);
        sem_wait(&preemptMutex);
        currStudent -> arrivalTime = high_resolution_clock::now();
        queue.addStudentToQueue(currStudent);

        id++;
    }

    return 0;
}

/*
Function: seatStudentFunction
Purpose: Seats students at availible tables
Method: Waits until a student is in the queue then gets them the next availble table based on their arrival in the queue. A student is only given a certain time slot to eat and if
        they arent finished with eating, they will be thrown back into the line and will have to wait until the next availible table to keep eating. Once a student finishes eating,
        their table will be freed for the next availble person
Input: void pointer
Output: N/A

*/
void *seatStudentFunction(void *arg)
{
    while(queue.studentsServed < queue.numberOfStudents)
    {
        sem_wait(&genMutex);
        int currTimeSitting = 0;
        bool preempted = false;
        if(queue.studentsServed < queue.numberOfStudents)
        {
            StudentNode *currStudent = queue.getStudent();
            auto sit = high_resolution_clock::now();
            printf("Sit %i \n", currStudent -> id);
            sem_post(&genMutex);
            if(currStudent -> id > 0)
            {
                queue.studentsServed++; 
                while(!preempted && currStudent -> eatingTimeLeft > 0)
                {   
                    sleep(1);
                    currTimeSitting += 1;
                    currStudent -> eatingTimeLeft -= 1;        
                    if(currTimeSitting == *timeQuantum && currStudent -> eatingTimeLeft > 0)
                    {
                        queue.addStudentToQueue(currStudent);
                        preempted = true;
                        printf("Preempt %i \n", currStudent -> id);
                        queue.numberOfStudents++;
                        sem_post(&preemptMutex);
                    }
                    else
                    {
                        sem_post(&preemptMutex);
                    }
                }
                if(!preempted && currStudent -> eatingTimeLeft == 0)
                {
                    auto leave = high_resolution_clock::now();
                    int turnaroundTime = duration_cast<seconds>(leave - currStudent -> arrivalTime + 500ms).count();
                    printf("Leave %i Turnaround %i Wait %i\n", currStudent -> id, turnaroundTime, turnaroundTime - currStudent -> burstTime);
                }
            }
        }
    }
    sem_post(&genMutex);
    return 0;
}


int main(int argc, char *argv[])
{
    if (argc != 2) 
    {
        printf("usage: ./round-robin <File name>\n");
        return 0;
    }
    
    string filename = argv[1];
        
    queue.getNumberOfStudents(filename);

    sem_init(&genMutex, 1, 1);
    sem_init(&preemptMutex, 1, 1);

    timeQuantum = (int*)mmap(NULL, sizeof(int*), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    queue.init();

    pthread_t producerID, consumerID1, consumerID2, consumerID3, consumerID4;

    pthread_create(&producerID, NULL, addStudentFunction, &filename);
    pthread_create(&consumerID1, NULL, seatStudentFunction, NULL);
    pthread_create(&consumerID2, NULL, seatStudentFunction, NULL);
    pthread_create(&consumerID3, NULL, seatStudentFunction, NULL);
    pthread_create(&consumerID4, NULL, seatStudentFunction, NULL);

    pthread_join(producerID, NULL);
    pthread_join(consumerID1, NULL);
    pthread_join(consumerID2, NULL);
    pthread_join(consumerID3, NULL);
    pthread_join(consumerID4, NULL);

	return 1;
}