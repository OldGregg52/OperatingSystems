#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>

using namespace std;

void initSemaphores();
void initVariables();

sem_t mutex, agentSem, tobacco, paper, match;
sem_t notifyM, notifyP, notifyT;
bool isTobacco, isMatch, isPaper;
int tSmoked, pSmoked, mSmoked, mPush, tPush, pPush;

void * t_smoker(void* arg)
{
  cout << "tin" << endl;
  for(tSmoked = 0; tSmoked < 6; ++tSmoked)
  {
      if(tSmoked == 5)
        return(NULL);
      sem_wait(&notifyT);
      sleep((rand() % 50) / 1000);
      cout << "t Smoked:" << tSmoked << endl;
      sem_post(&agentSem);
      sleep((rand() % 50) / 1000);
  }

  cout << "tDone";
  return(NULL);
}

void * m_smoker(void* arg)
{
  cout << "min" << endl;
  for(mSmoked = 0; mSmoked < 6; ++mSmoked)
  {
      if(mSmoked == 5)
        return(NULL);
      sem_wait(&notifyM);
      sleep((rand() % 50) / 1000);
      cout << "m Smoked: " << mSmoked << endl;
      sem_post(&agentSem);
      sleep((rand() % 50) / 1000);
  }

  cout << "mDone";
  return(NULL);
}

void * p_smoker(void* arg)
{
  cout << "pin" << endl;
  for(pSmoked = 0; pSmoked < 6; ++pSmoked)
  {
      if(pSmoked == 5)
        return(NULL);
      sem_wait(&notifyP);
      sleep((rand() % 50) / 1000);
      cout << "p Smoked:" << pSmoked << endl;
      sem_post(&agentSem);
      sleep((rand() % 50) / 1000);
  }

  cout << "pDone";
  return(NULL);
}

void * t_pusher(void* arg)
{
  while(true)
  {
    sem_wait(&tobacco);
    sem_wait(&mutex);
    ++tPush;
    if(isPaper)
    {
      isPaper = false;
      if(mSmoked < 6)
        sem_post(&notifyM);
      else
        sem_post(&agentSem);
      cout << "t_pusher notifying m_smoker" << endl;
    }
    else if(isMatch)
    {
      isMatch = false;
      if(pSmoked < 6)
        sem_post(&notifyP);
      else
        sem_post(&agentSem);
      cout << "t_pusher notifying p_smoker" << endl;
    }
    else
      isTobacco = true;
    sem_post(&mutex);

    if(tPush == 15)
      return(NULL);
  }

  cout << "Done";
  sem_post(&agentSem);
  return(arg);
}

void * m_pusher(void* arg)
{
  while(true)
  {
    sem_wait(&match);
    sem_wait(&mutex);
    ++mPush;
    if(isPaper)
    {
      isPaper = false;
      if(tSmoked < 6)
        sem_post(&notifyT);
      else
        sem_post(&agentSem);
      cout << "m_pusher notifying t_smoker" << endl;
    }
    else if(isTobacco)
    {
      isTobacco = false;
      if(pSmoked < 6)
        sem_post(&notifyP);
      else
        sem_post(&agentSem);
      cout << "m_pusher notifying p_smoker" << endl;
    }
    else
      isMatch = true;
    sem_post(&mutex);
    if(mPush == 15)
      return(NULL);
  }

  cout << "Done";
  sem_post(&agentSem);

  return(arg);
}

void * p_pusher(void* arg)
{
  while(true)
  {
    sem_wait(&paper);
    sem_wait(&mutex);
    ++pPush;
    if(isMatch)
    {
      isMatch = false;
      if(tSmoked < 6)
        sem_post(&notifyT);
      else
        sem_post(&agentSem);
      cout << "p_pusher notifying t_smoker" << endl;
    }
    else if(isTobacco)
    {
      isTobacco = false;
      //sem_wait(&mHold);
      if(mSmoked < 6)
        sem_post(&notifyM);
      else
        sem_post(&agentSem);
      cout << "p_pusher notifying m_smoker" << endl;
    }
    else
      isPaper = true;
    sem_post(&mutex);

    if(pPush == 15)
      return(NULL);
  }

  cout << "Done";
  sem_post(&agentSem);
  return(arg);
}

void * agent(void* arg)
{
  while(true)
  {
    sem_wait(&agentSem);
    sleep((rand() % 200) / 1000);
    int temp = rand() % 3;

    if((temp == 0) && ((pPush < 12) || (tPush < 12)))
    {
      sem_post(&tobacco);
      sem_post(&paper);
    }
    else if((temp == 1) && ((pPush < 12) || (mPush < 12)))
    {
      sem_post(&paper);
      sem_post(&match);
    }
    else if((temp == 2) && ((tPush < 12) || (mPush < 12)))
    {
      sem_post(&match);
      sem_post(&tobacco);
    }
    else
      sem_post(&agentSem);

    if((mSmoked + pSmoked + tSmoked) == 15)
    {
      return(NULL);
      cout << "3";
    }
  }

  return(arg);
}

int main()
{
  srand(time(NULL));

  initVariables();
  initSemaphores();

  pthread_t smokers[6];
  pthread_t pushers[3];
  pthread_t agents[3];

  pthread_create(&agents[0], NULL, agent, NULL);
  pthread_create(&agents[1], NULL, agent, NULL);
  pthread_create(&agents[2], NULL, agent, NULL);

  pthread_create(&pushers[0], NULL, t_pusher, NULL);
  pthread_create(&pushers[1], NULL, m_pusher, NULL);
  pthread_create(&pushers[2], NULL, p_pusher, NULL);

  //cout << "pusher threads created" << endl;

  pthread_create(&smokers[0], NULL, t_smoker, NULL);
  pthread_create(&smokers[1], NULL, t_smoker, NULL);
  pthread_create(&smokers[2], NULL, p_smoker, NULL);
  pthread_create(&smokers[3], NULL, p_smoker, NULL);
  pthread_create(&smokers[4], NULL, m_smoker, NULL);
  pthread_create(&smokers[5], NULL, m_smoker, NULL);

  for(int i = 0; i < 3; i++)
    pthread_join(pushers[i], NULL);

  cout << "pusher threads joined" << endl;

  for(int i = 0; i < 6; i++)
    pthread_join(smokers[i], NULL);

  cout << "smoker threads joined" << endl;
  for(int i = 0; i < 3; i++)
    pthread_join(agents[i], NULL);

  cout << "agent threads joined" << endl;

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

  cout << "Semaphores initialized" << endl;
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
}
