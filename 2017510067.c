#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> 
#include <time.h> 
#define Max_people 50

void *people(void *num);  //The method for people who come to the health clinic for testing
void *staff(void *no);  //The method of healthcare staffs working in the health clinic.
void wait();  //random time generation method


sem_t staff_pillow; //When the test room is emptied, the semaphore that will control the ventilation of the room by the health personnel in that test room
sem_t waitingseats[8]; //semaphore for the occupancy of 3 seats in 8 waiting rooms in the clinic
sem_t Staff[8];  // 8 health staff semaphore in the clinic
sem_t mutex;   // the semaphore that keeps the waiting room at the clinic available for testing
sem_t Peoples; // semaphore of people who come to the clinic for testing

int numberof_people; //the number of people who come to get tested at the clinic
int test=0; // If the test in the waiting room is completed, the variable value is 1, if not, the variable value is 0.
int temp3; // the number of the first person to come to the waiting room
int temp2; //the number of the second person coming to the waiting room
int people_no; // holding the number of the person coming to the clinic
int unit_no;  // the room number of the waiting room in the clinic.
int nmu=0; // it keeps track of how many people in total are entering the waiting rooms in the clinic (for control purposes).
int unit_numarasi=0; // variable that keeps which waiting room a person enters to be tested
int Stf_Number[8]; // keeps the number of the healthcare personnel in the waiting rooms in the clink. If there is a patient waiting in the waiting room (it can also be said that he is doing indexing.)
int temp_rw[Max_people]; //  keeps the values ​​of which waiting room people entered at random time.

int main(int argc, char *argv[])
{
    int x;
    printf("The maximum capacity of the clinic is 50. Determine the number of people accordingly\n");
    scanf("%d",&x); // asks the number of people coming to the clinic from the user
    numberof_people=x;
    pthread_t btid[8];
    pthread_t tid[Max_people]; 
    int Number[Max_people];  // array that holds the number of people coming to the clinic and with the maximum capacity of the clinic.
    if (numberof_people>Max_people)
    {
        printf("The maximum number of People is %d.\n", Max_people);
        system("PAUSE");   
        return 0;
    } 

    //initialize the semaphores 
    for(int a=0;a<8;a++)
    {
        sem_init(&Staff[a],0,0); 
    }
    for(int a=0;a<8;a++)
    {
        sem_init(&waitingseats[a],0,3); 
    }  
    sem_init(&Peoples,0,numberof_people); 
    sem_init(&staff_pillow,0,0); 
    sem_init(&mutex,0,1);

     // random waiting room entrance in random time.
    for (int i =0; i <8; i++) 
    {
        Stf_Number[i] = i;
        
    } 
    int m = sizeof(Stf_Number)/ sizeof(Stf_Number[0]); 
    randomize (Stf_Number, m); 

    for(int p=0;p<8;p++)
    {
        temp_rw[p]=Stf_Number[p];
    }  

    //random people entrance in random time
    for (int i = 0; i < numberof_people; i++) {
        Number[i] = i;
    } 
    int n = sizeof(Number)/ sizeof(Number[0]); 
    randomize (Number, numberof_people); 
   
    for (int i=0;i<8;i++)
    {   
        // create healthcare staff  processes 
        pthread_create(&btid[i], NULL, staff, (void *)&i);
            
    }
          /* START CRITICAL REGION */
    for (int i=0;i<numberof_people;i++)
    { 
        // create personel processes 
        pthread_create(&tid[i], NULL, people, (void *)&Number[i]);
        wait();    
    }
    for (int i = 0; i < numberof_people; i++) {
        pthread_join(tid[i],NULL);    
    }
    
}
void *staff(void *num)   // The method I created for the healthcare staff. But it doesn't work properly due to some algorithmic errors.
{
    unit_no=*(int *)num;
    /*printf("Healthcare staff.-%d[Id:%d] Joins Shop. ",unit_no,pthread_self());
    while(1)              
    {   
        sem_wait(&Staff[unit_no]);    // the room is expected to be filled.
        sem_wait(&mutex);            //Lock mutex to protect seat changes
        if(test==1)      // the test in the waiting room is over.
        {
            printf("Healthcare staff ventilates -%d waiting room. ",unit_no);
        }
        else
        {
            sem_post(&mutex);
            sem_post(&Peoples);         
            printf("healthcare staff.-%d Wakes Up .\n",unit_no);
            sleep(1);
        }
    }
    */
}
void *people(void *num) // The method of people coming to have a test
{  
    people_no=*(int *)num; 
    //sem_wait(&mutex);
    unit_numarasi=temp_rw[nmu];
    int value; // I used it to suppress the visual appearance of the waiting chairs in the room as a result of reaching the occupancy rate.
    sem_getvalue(&waitingseats[unit_numarasi],&value);
   // sem_post(&Staff[num]); //activates the healthcare staff.
    printf("People %d is entering the hospital\n",people_no+1);
    printf("People %d is at Covid-19 Test Unit %d waiting room\n",people_no+1,unit_numarasi+1);
    printf("People %d is filling the form\n",people_no+1);
    printf("Covid-19 Test Unit %d waiting room:\n",unit_numarasi+1);
    //sem_post(&mutex); // Unlock mutex to protect seat changes
    if(value==3)  // If all the waiting chairs are empty, in that the (unit_numarası) room
    {
        sem_wait(&waitingseats[unit_numarasi]);
        printFirst(people_no+1,unit_numarasi+1);
  
    }
    else if (value==2) // If the waiting chairs are not all empty, in that the (unit_numarası) room
    {
        sem_wait(&waitingseats[unit_numarasi]);
        printSecond(people_no+1,unit_numarasi+1);
    
    }
    else if (value==1) // if all the waiting chairs are full,in that the (unit_numarası) room
    {
       temp_rw[(nmu+8)]=Stf_Number[nmu];
       sem_wait(&waitingseats[unit_numarasi]);
       printThird(people_no+1,unit_numarasi+1);
       nmu=nmu+1;
       // This cycle activates three more waiting chairs after the testing process in the room is finished and the room is empty.
       for(int y=0;y<3;y++)
       {
           sem_post(&waitingseats[unit_numarasi]);
       }
       test=1;
       printf("\n");
       //sem_wait(&mutex);
       pthread_exit(0);
       //sem_post(&mutex); 
    }

}
void printFirst(int *num,int *numstf) // A person has come to the waiting room and if there are two more vacancies, the health personnel announce
{   
    temp3=people_no+1;
    printf("[X-%d][ ][ ]\n",num);
    printf("         Healthcare Staff %d :  The last 2 people, let' s start! Please, pay attention to your social distance and hygiene use a mask\n",numstf);
}
void printSecond(int *num,int *numstf) // Two people are waiting in the waiting room and if there is a single vacant place, the health personnel announce.
{
    temp2=people_no+1;
    printf("[X-%d][X-%d][ ]\n",temp3,num);
    printf("         Healthcare Staff %d :  The last 1 people, let' s start! Please, pay attention to your social distance and hygiene use a mask\n",numstf);
}
void printThird(int *num,int *numstf) // The third person also came to the waiting room and the testing process took place and the patients in the room leave the room.
{
    printf("[X-%d][X-%d][X-%d]\n",temp3,temp2,num);
    printf("Covid-19 test Unit %d's Healthcare Staff apply the  test\n",numstf);
    printf("People %d is leaving from unit\n",temp3);
    printf("People %d is leaving from unit\n",temp2);
    printf("People %d is leaving from unit\n",num);
    printf("\n");
        
}
void wait()     /*Generates random number between 50000 to 250000*/
{
     int x = rand() % (250000 - 50000 + 1) + 50000; 
     srand(time(NULL));
     usleep(x);     //usleep halts execution in specified miliseconds
}
void swap (int *a, int *b) 
{ 
    int temp = *a; 
    *a = *b; 
    *b = temp; 
} 
void randomize ( int arr[], int n ) 
{   
    srand ( time(NULL) ); 
    for (int i = n-1; i > 0; i--) 
    { 
        int j = rand() % (i+1);  
        swap(&arr[i], &arr[j]); 
    } 
} 
