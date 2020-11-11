/*
 * Greg Brown
 * Operating Systems
 * Lab 02
 * November 2, 2020
 * Purpose: To implement semaphores and mutex locks to prevent deadlock in a
 *              program that utilizes concurrent threads
 *
 * NOTE: Unfortunately, I have only been able to get about a 60% to 75% success
 *          rate on this program and have been unable to figure out what the
 *          issue is.
 */

#include <semaphore.h>    //Included to utilize the "wait" and "post" functions
#include <pthread.h>      //Included to utilize concurrent threads
#include <stdlib.h>       //Included for standard C library
#include <iostream>       //Included for console output during debugging
#include <unistd.h>       //Included to utilize the "sleep" function

using namespace std;

void initSemaphores();    //Function to initialize semaphores
void initVariables();     //Function to initialize global variables

//Declaring all the necessary global bool, int, and semaphore variables
sem_t mutex, smokerMutex, agentSem, tobacco, paper, match;
sem_t notifyM, notifyP, notifyT;
bool isTobacco, isMatch, isPaper;
int tSmoked, pSmoked, mSmoked, mPush, tPush, pPush;

void * t_smoker(void* arg)
{
  for(int i = 0; i < 3; ++i)        //Iterates 3 times, which results in 6
  {                                 //    cigarettes because of concurrent threads
      sem_wait(&notifyT);           //Waits to be given the "ingredients" by pusher
      sem_wait(&smokerMutex);       //Waits until it can run after another finishes

      sleep((rand() % 50) / 1000);  //Delays for some time up to 50ms

      sem_post(&agentSem);          //Lets the agent know they can produce more

      sleep((rand() % 50) / 1000);  //Delays for some time up to 50ms

      sem_post(&smokerMutex);       //Releases mutex so another "smoker" can use
  }                                 //    their ingredients

  return(NULL);     //Return to show completion
}

//Another "smoker," the same as before except being called by a different semaphore
void * m_smoker(void* arg)
{
  for(int i = 0; i < 3; i++)
  {
      sem_wait(&notifyM);
      sem_wait(&smokerMutex);

      sleep((rand() % 50) / 1000);

      sem_post(&agentSem);

      sleep((rand() % 50) / 1000);

      sem_post(&smokerMutex);
  }

  return(NULL);
}

//Another "smoker," the same as before except being called by a different semaphore
void * p_smoker(void* arg)
{
  for(int i = 0; i < 3; ++i)
  {
      sem_wait(&notifyP);
      sem_wait(&smokerMutex);

      sleep((rand() % 50) / 1000);

      sem_post(&agentSem);

      sleep((rand() % 50) / 1000);

      sem_post(&smokerMutex);
  }

  return(NULL);
}

void * t_pusher(void* arg)
{
  for(tPush = 0; tPush < 12; ++tPush)     //Iterates 12 times giving the correct
  {                                       //    ingredients to the correct consumer
    sem_wait(&tobacco);     //Waits until the producer signals its ready
    sem_wait(&mutex);       //Mutex to ensure only one pusher responds to the agent
                            //    at a time
    if(isPaper)             //If the pusher picks up tobacco and paper is already
    {                       //    available, then the correct smoker is notified
      if(mSmoked < 6)       //If the smoker hasn't finished all their cigarettes
      {                     //    then the ingredients available are used...
        sem_post(&notifyM);
        isPaper = false;
      }
      else                  //...otherwise, the ingredient is left and the agent
        sem_post(&agentSem);//      is called again
    }

    else if(isMatch)        //The same happens here except for matches instead of
    {                       //      papers
      if(pSmoked < 6)
      {
        sem_post(&notifyP);
        isMatch = false;
      }
      else
        sem_post(&agentSem);
    }

    else                    //If tobacco was the only available ingredient them
      isTobacco = true;     //    the corresponding boolean variable is set

    sem_post(&mutex);       //The mutex lock is released since the pusher is done
  }

  sem_post(&agentSem);      //When the loop is done, the agent is called and this
  return(arg);              //    function terminates
}

//Another pusher like the one above except dealing with different ingreadients
void * m_pusher(void* arg)
{
  for(mPush = 0; mPush < 12; ++mPush)
  {
    sem_wait(&match);
    sem_wait(&mutex);

    if(isPaper)
    {
      if(tSmoked < 6)
      {
        sem_post(&notifyT);
        isPaper = false;
      }
      else
        sem_post(&agentSem);
    }

    else if(isTobacco)
    {
      if(pSmoked < 6)
      {
        sem_post(&notifyP);
        isTobacco = false;
      }
      else
        sem_post(&agentSem);
    }

    else
      isMatch = true;

    sem_post(&mutex);
  }

  sem_post(&agentSem);
  return(arg);
}

//Another pusher like the one above except with different ingredients
void * p_pusher(void* arg)
{
  for(pPush = 0; pPush < 12; ++pPush)
  {
    sem_wait(&paper);
    sem_wait(&mutex);

    if(isMatch)
    {
      if(tSmoked < 6)
      {
        sem_post(&notifyT);
        isMatch = false;
      }
      else
        sem_post(&agentSem);
    }

    else if(isTobacco)
    {

      if(mSmoked < 6)
      {
        sem_post(&notifyM);
        isTobacco = false;
      }
      else
        sem_post(&agentSem);
    }

    else
      isPaper = true;

    sem_post(&mutex);
  }

  sem_post(&agentSem);
  return(arg);
}

void * agent1(void* arg)
{
  while(true)   //The agent continues to iterate eternally until the function is
  {             //    termiated when the smokers are done
    sem_wait(&agentSem);          //Agent waits until they are needed

    sleep((rand() % 200) / 1000); //Agent waits some amount of time up to 200ms

    if((tPush < 12) || (mPush < 12))  //If smokers that require one of the two
    {                                 //    ingredients are still present then
      sem_post(&tobacco);             //    they are passed to the pusher
      sem_post(&match);
    }

    else                //...otherwise, the function terminates
      return(NULL);
  }
}

//Second agent, which operates like the one above but with different ingredients
void * agent2(void* arg)
{
  while(true)
  {
    sem_wait(&agentSem);
    sleep((rand() % 200) / 1000);

    if((pPush < 12) || (mPush < 12))
    {
      sem_post(&match);
      sem_post(&paper);
    }
    else
      return(NULL);
  }
}

//Third agent, which operates like the one above except with different ingredients
void * agent3(void* arg)
{
  while(true)
  {
    sem_wait(&agentSem);
    sleep((rand() % 200) / 1000);

    if((tPush < 12) || (pPush < 12))
    {
      sem_post(&tobacco);
      sem_post(&paper);
    }
    else
      return(NULL);
  }
}

int main()
{
  srand(time(NULL));    //Random number generator is seeded for use in the threads

  initVariables();      //Calling a function to initialize all the global variables
  initSemaphores();     //Calling a function to delcare and initialize the semaphores

  pthread_t smokers[6];   //Establishing ID arrays for the smokers, pushers, and
  pthread_t pushers[3];   //    agents
  pthread_t agents[3];

  /* Starting the threads for the three agents */
  pthread_create(&agents[0], NULL, agent1, NULL);
  pthread_create(&agents[1], NULL, agent2, NULL);
  pthread_create(&agents[2], NULL, agent3, NULL);

  /* Starting the threads for the three pushers */
  pthread_create(&pushers[0], NULL, t_pusher, NULL);
  pthread_create(&pushers[1], NULL, m_pusher, NULL);
  pthread_create(&pushers[2], NULL, p_pusher, NULL);

  /* Starting the threads for the six smokers */
  pthread_create(&smokers[0], NULL, t_smoker, NULL);
  pthread_create(&smokers[1], NULL, t_smoker, NULL);
  pthread_create(&smokers[2], NULL, p_smoker, NULL);
  pthread_create(&smokers[3], NULL, p_smoker, NULL);
  pthread_create(&smokers[4], NULL, m_smoker, NULL);
  pthread_create(&smokers[5], NULL, m_smoker, NULL);

  /* Once all the threaded functions terminate, each cluster of threads
        are rejoined */
  for(int i = 0; i < 3; i++)
    pthread_join(pushers[i], NULL);

  for(int i = 0; i < 6; i++)
    pthread_join(smokers[i], NULL);

  for(int i = 0; i < 3; i++)
    pthread_join(agents[i], NULL);

  return(0);
}

void initSemaphores()     //Initializing all semaphores
{
  sem_init(&agentSem, 0, 1);
  sem_init(&mutex, 0, 1);
  sem_init(&tobacco, 0, 0);
  sem_init(&paper, 0, 0);
  sem_init(&match, 0, 0);
  sem_init(&notifyM, 0, 0);
  sem_init(&notifyP, 0, 0);
  sem_init(&notifyT, 0, 0);
  sem_init(&smokerMutex, 0, 1);

  return;
}

void initVariables()    //Initializing all global variables
{
  tSmoked = 0;
  pSmoked = 0;
  mSmoked = 0;
  mPush = 0;
  tPush = 0;
  pPush = 0;
  isTobacco  = false;
  isPaper = false;
  isMatch = false;

  return;
}
