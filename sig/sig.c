#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>


void catcher(int signum) {
  puts("inside signal catcher!");
}

int main(int argc, char *argv[]) {
  struct sigaction sact;
  volatile double count;
  time_t t;
  timer_t timer;

  sigemptyset(&sact.sa_mask);
  sact.sa_flags = 0;
  sact.sa_handler = catcher;
  sigaction(SIGALRM, &sact, NULL);

  struct sigevent sev;
  struct itimerspec its;
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGALRM;
  sev.sigev_value.sival_ptr = &timer;

  if (timer_create(CLOCK_REALTIME, &sev, &timer) == -1)
  {
      perror("error to create");
      exit(1);
  }

  its.it_value.tv_sec = 1;
  its.it_value.tv_nsec = 0;
  its.it_interval.tv_sec = its.it_value.tv_sec;
  its.it_interval.tv_nsec = its.it_value.tv_nsec;

  if (timer_settime(timer, 0, &its, NULL) == -1) {
      printf("error\n");
      exit(1);
  }

  sleep(1);
  printf("start..\n");
  while(1) {
      sleep(1);
  }
}
