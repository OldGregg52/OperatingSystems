#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>

using namespace std;

void initSemaphores();
void initVariables();

sem_t mutex, smokerMutex, agentSem, tobacco, paper, match;
sem_t notifyM, notifyP, notifyT;
bool isTobacco, isMatch, isPaper;
int tSmoked, pSmoked, mSmoked, mPush, tPush, pPush;

void * t_smoker(void* arg)
{
  for(int i = 0; i < 3; ++i)
  {
      sem_wait(&notifyT);
      sem_wait(&smokerMutex);
      sleep((rand() % 50) / 1000);
      sem_post(&agentSem);
      sleep((rand() % 50) / 1000);
      sem_post(&smokerMutex);
  }
  return(NULL);
}

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
  for(tPush = 0; tPush < 12; ++tPush)
  {
    sem_wait(&tobacco);
    sem_wait(&mutex);
    if(isPaper)
    {
      if(mSmoked < 6)
      {
        sem_post(&notifyM);
        isPaper = false;
      }
      else
        sem_post(&agentSem);
    }
    else if(isMatch)
    {
      if(pSmoked < 6)
      {
        sem_post(&notifyP);
        isMatch = false;
      }
      else
        sem_post(&agentSem);
    }
    else
      isTobacco = true;
    sem_post(&mutex);
  }

  sem_post(&agentSem);
  return(arg);
}

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
  while(true)
  {
    sem_wait(&agentSem);
    sleep((rand() % 200) / 1000);

    if((tPush < 12) || (mPush < 12))
    {
      sem_post(&tobacco);
      sem_post(&match);
    }
    else
      return(NULL);
  }
}

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
  srand(time(NULL));

  initVariables();
  initSemaphores();

  pthread_t smokers[6];
  pthread_t pushers[3];
  pthread_t agents[3];

  pthread_create(&agents[0], NULL, agent1, NULL);
  pthread_create(&agents[1], NULL, agent2, NULL);
  pthread_create(&agents[2], NULL, agent3, NULL);

  pthread_create(&pushers[0], NULL, t_pusher, NULL);
  pthread_create(&pushers[1], NULL, m_pusher, NULL);
  pthread_create(&pushers[2], NULL, p_pusher, NULL);

  pthread_create(&smokers[0], NULL, t_smoker, NULL);
  pthread_create(&smokers[1], NULL, t_smoker, NULL);
  pthread_create(&smokers[2], NULL, p_smoker, NULL);
  pthread_create(&smokers[3], NULL, p_smoker, NULL);
  pthread_create(&smokers[4], NULL, m_smoker, NULL);
  pthread_create(&smokers[5], NULL, m_smoker, NULL);

  for(int i = 0; i < 3; i++)
    pthread_join(pushers[i], NULL);

  for(int i = 0; i < 6; i++)
    pthread_join(smokers[i], NULL);

  for(int i = 0; i < 3; i++)
    pthread_join(agents[i], NULL);

  return(0);

}

void initSemaphores()
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

void initVariables()
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
