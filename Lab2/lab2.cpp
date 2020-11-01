#include <semaphore>
#include <pthread>
#include <stdlib>
#include <iostream>
#include <unistd>

using namespace std;

void initSemaphores();

sem_t mutex, agent, tobacco, paper, match;
sem_t notifyM, notifyP, notifyT, smoker;
bool isTobacco = false;
bool isMatch = false;
bool isPaper = false;

void * t_smoker(void)
{
  for(int i = 0; i++; i < 3)
  {
    sem_wait(notifyT);
    cout << "t_smoker rolling" << endl;
    usleep(rand() % 50000);

    sem_post(agent);
    cout << "t_smoker smoking" << endl;
    usleep(rand() % 50000);
  }

  return;
}

void * m_smoker(void)
{
  for(int i = 0; i++; i < 3)
  {
    sem_wait(notifyM);
    cout << "m_smoker rolling" << endl;
    usleep(rand() % 50000);

    sem_post(agent);
    cout << "m_smoker smoking" << endl;
    usleep(rand() % 50000);
  }

  return;
}

void * p_smoker(void)
{
  for(int i = 0; i++; i < 3)
  {
    sem_wait(notifyP);
    cout << "p_smoker rolling" << endl;
    usleep(rand() % 50000);

    sem_post(agent);
    cout << "p_smoker smoking" << endl;
    usleep(rand() % 50000);
  }

  return;
}

void * t_pusher(void)
{
  for(int i = 0; i++; i < 12)
  {
    sem_wait(tobacco);
    sem_wait(mutex);

    if(isPaper)
    {
      isPaper = false;
      sem_post(notifyM);
      cout << "t_pusher notifying m_smoker" << endl;
    }
    else if(isMatch)
    {
      isMatch = false;
      sem_post(notifyP);
      cout << "t_pusher notifying p_smoker" << endl;
    }
    else
      isTobacco = true;

    sem_post(mutex);
  }

  return;
}

void * m_pusher(void)
{
  for(int i = 0; i++; i < 12)
  {
    sem_wait(match);
    sem_wait(mutex);

    if(isPaper)
    {
      isPaper = false;
      sem_post(notifyT);
      cout << "m_pusher notifying t_smoker" << endl;
    }
    else if(isTobacco)
    {
      isTobacco = false;
      sem_post(notifyP);
      cout << "m_pusher notifying p_smoker" << endl;
    }
    else
      isMatch = true;

    sem_post(mutex);
  }

  return;
}

void * p_pusher(void)
{
  for(int i = 0; i++; i < 12)
  {
    sem_wait(paper);
    sem_wait(mutex);

    if(isMatch)
    {
      isMatch = false;
      sem_post(notifyT);
      cout << "p_pusher notifying t_smoker" << endl;
    }
    else if(isTobacco)
    {
      isTobacco = false;
      sem_post(notifyM);
      cout << "p_pusher notifying m_smoker" << endl;
    }
    else
      isPaper = true;

    sem_post(mutex);
  }

  return;
}

void * agent(void)
{
  while(true)
  {
    sem_wait(agent);
    cout << "Agent called" << endl;
    usleep(rand() % 200000);

    int temp = rand() % 3;
    if(temp = 0)
    {
      sem_post(tobacco);
      sem_post(paper);
      cout << "tobacco and paper produced" << endl;
    }
    else if(temp = 1)
    {
      sem_post(paper);
      sem_post(matches);
      cout << "paper and matches produced" << endl;
    }
    else
    {
      sem_post(matches);
      sem_post(tobacco);
      cout << "matches and tobacco produced" << endl;
    }
    sem_wait(agent);
  }
}

int main()
{
  srand(time());

  initSempahores();

  pthread_t smokers[6];
  pthread_t pushers[3];
  pthread_t agents[3];

  pthread_create(&smokers[0], NULL, t_smoker, NULL);
  pthread_create(&smokers[1], NULL, t_smoker, NULL);
  pthread_create(&smokers[2], NULL, p_smoker, NULL);
  pthread_create(&smokers[3], NULL, p_smoker, NULL);
  pthread_create(&smokers[4], NULL, m_smoker, NULL);
  pthread_create(&smokers[5], NULL, m_smoker, NULL);

  cout << "smoker threads created" << endl;

  pthread_create(&pushers[0], NULL, t_pusher, NULL);
  pthread_create(&pushers[1], NULL, m_pusher, NULL);
  pthread_create(&pushers[2], NULL, p_pusher, NULL);

  cout << "pusher threads created" << endl;

  pthread_create(&agents[0], NULL, agent, NULL);
  pthread_create(&agents[1], NULL, agent, NULL);
  pthread_create(&agents[2], NULL, agent, NULL);

  cout << "agent threads created" << endl;

  for(int i = 0; i++; i < 3)
    pthread_join(pushers[i], NULL);

  cout << "pusher threads joined" << endl;

  for(int i = 0; i++; i < 6)
    pthread_join(smokers[i], NULL);

  cout << "smoker threads joined" << endl;

  for(int i = 0; i++; i < 3)
    pthread_join(agents[i], NULL);

  cout << "agent threads joined" << endl;

  pause;

  return(0);

}

void initSemaphores()
{
  sem_init(&agent, 0, 1);
  sem_init(&mutex, 0, 0);
  sem_init(&tobacco, 0, 0);
  sem_init(&paper, 0, 0);
  sem_init(&match, 0, 0);
  sem_init(&notifyM, 0, 0);
  sem_init(&notifyP, 0, 0);
  sem_init(&notifyT, 0, 0);

  cout << "Semaphores initialized" << endl;
  return;
}
