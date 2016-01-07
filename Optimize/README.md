参考：
## API：
‘’‘c
#define _GNU_SOURCE             
#include <sched.h>

int sched_setaffinity(pid_t pid, size_t cpusetsize,cpu_set_t *mask);    //设定pid 绑定的cpu， 
int sched_getaffinity(pid_t pid, size_t cpusetsize,cpu_set_t *mask);    //查看pid 绑定的cpu。
cpu_set_t  //是一个掩码数组，一共有1024位，每一位都可以对应一个cpu核心

//以下宏，都是对这个掩码进行操作的。如果需要，一个进程是可以绑定多个cpu的。
void CPU_ZERO(cpu_set_t *set);
void CPU_SET(int cpu, cpu_set_t *set);
void CPU_CLR(int cpu, cpu_set_t *set);
int CPU_ISSET(int cpu, cpu_set_t *set);

//设置线程的cpu亲和性
#define _GNU_SOURCE
#include <pthread.h>
 
int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize,const cpu_set_t *cpuset);
int pthread_getaffinity_np(pthread_t thread, size_t cpusetsize,cpu_set_t *cpuset);
’‘’


http://blog.csdn.net/joker0910/article/details/7484371
http://www.ibm.com/developerworks/cn/linux/l-affinity.html
