/*
Purpose: Cpu scheduling with first in first out
Known bugs: None
Limitations:
Usage: To make and run code
            Case 1:
            make all
            ./fifo <Filename>
                     
       To clean files
            make clean
*/
#include "queue.h"

struct QueueMonitor queue;
sem_t genMutex;

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

        sleep(timestamp);

        printf("Arrive %i \n", id);
        currStudent -> arrivalTime = high_resolution_clock::now();
        queue.addStudentToQueue(currStudent);
        id++;
     }
     return 0;
}

/*
Function: seatStudentFunction
Purpose: Seats students at availible tables
Method: Waits until a student is in the queue then gets them the next availble table based on their arrival in the queue. Once a student finishes eating,
        they are removed from the table.
Input: void pointer
Output: N/A

*/
void *seatStudentFunction(void *arg)
{
    StudentNode *currStudent;
    
    while(queue.studentsServed < queue.numberOfStudents)
    {   
        sem_wait(&genMutex);
        if(queue.studentsServed < queue.numberOfStudents)
        {
            currStudent = queue.getStudent();
            sem_post(&genMutex); 
            queue.studentsServed++;
            auto sit = high_resolution_clock::now();
            printf("Sit %i \n", currStudent -> id);
            while(currStudent -> eatingTimeLeft != 0)
            {
                sleep(1);
                currStudent -> eatingTimeLeft -= 1;
            }
            auto leave = high_resolution_clock::now();
            
            printf("Leave %i Turnaround %i Wait %i\n", currStudent -> id, duration_cast<seconds>(leave - currStudent -> arrivalTime + 500ms).count(), duration_cast<seconds>(sit - currStudent -> arrivalTime + 500ms).count());
        }
    }
    sem_post(&genMutex);
    free(currStudent);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2) 
    {
        printf("usage: ./fifo <File name>\n");
        return 0;
    }
    string filename = argv[1];
    
    queue.getNumberOfStudents(filename);

    sem_init(&genMutex, 1, 1);

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