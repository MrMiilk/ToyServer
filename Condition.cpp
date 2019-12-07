#include "Condition.h"

void Condition::wait() { pthread_cond_wait(&cond_, mutex_.getPThreadMutex()); }
void Condition::notify() { pthread_cond_signal(&cond_); }
void Condition::notifyAll() { pthread_cond_broadcast(&cond_); }