#define _GNU_SOURCE
#include <errno.h>
#include <linux/sched.h>
#include <linux/types.h>
//#include <linux/wait.h>
#include <sched.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <assert.h>
#include <net/route.h>

void ns_setup(pid_t pid);
void peer(char *bfg);

const int MAX = 26;

typedef struct bbb {
    char* path;
    char* ip;
    char* mask;
    char* route;
    char* exe;


} bbb;


char *rands(int length) {    
    static int mySeed = 25011984;
    char *string = "abcdefghijklmnopqrstuvwxyz";
    size_t stringLen = strlen(string);        
    char *randomString = NULL;

    srand(time(NULL) * length + ++mySeed);

    if (length < 1) {
        length = 1;
    }

    randomString = malloc(sizeof(char) * (length +1));

    if (randomString) {
        short key = 0;

        for (int n = 0;n < length;n++) {            
            key = rand() % stringLen;          
            randomString[n] = string[key];
        }

        randomString[length] = '\0';

        return randomString;        
    }
    else {
        printf("No memory");
        exit(1);
    }
}

int setip(const char *name, const char *addr, const char *netmask) {
    struct ifreq ifr;
    int fd = socket(PF_INET, SOCK_DGRAM, 0);

    if (fd < 0) {
        perror("socket");
        return -1;
    }

    memset(&ifr, 0, sizeof(struct ifreq));

    strncpy(ifr.ifr_name, name, IFNAMSIZ - 1); 

    struct sockaddr_in ip_addr;
    memset(&ip_addr, 0, sizeof(struct sockaddr_in));
    ip_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, addr, &ip_addr.sin_addr) != 1) {
        perror("inet_pton (addr)");
        close(fd);
        return -1;
    }
    memcpy(&ifr.ifr_addr, &ip_addr, sizeof(struct sockaddr_in));
    if (ioctl(fd, SIOCSIFADDR, &ifr) < 0) {
        perror("ioctl(SIOCSIFADDR)");
        close(fd);
        return -1;
    }

    // Установка маски подсети
    memset(&ip_addr, 0, sizeof(struct sockaddr_in));
    ip_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, netmask, &ip_addr.sin_addr) != 1) {
        perror("inet_pton (netmask)");
        close(fd);
        return -1;
    }
    memcpy(&ifr.ifr_addr, &ip_addr, sizeof(struct sockaddr_in));
    if (ioctl(fd, SIOCSIFNETMASK, &ifr) < 0) {
        perror("ioctl(SIOCSIFNETMASK)");
        close(fd);
        return -1;
    }

    if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0) {
        perror("ioctl(SIOCGIFFLAGS)");
        close(fd);
        return -1;
    }

    ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
    if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0) {
        perror("ioctl(SIOCSIFFLAGS)");
        close(fd);
        return -1;
    }

    memset(&ifr.ifr_hwaddr, 0, sizeof(ifr.ifr_hwaddr));
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    ifr.ifr_hwaddr.sa_data[0] = 0xDE;
    ifr.ifr_hwaddr.sa_data[1] = 0xAD;
    ifr.ifr_hwaddr.sa_data[2] = 0xBE;
    ifr.ifr_hwaddr.sa_data[3] = 0xEF;
    ifr.ifr_hwaddr.sa_data[4] = rand() % 256;
    ifr.ifr_hwaddr.sa_data[5] = rand() % 256;

    if (ioctl(fd, SIOCSIFHWADDR, &ifr) < 0) {
        perror("ioctl(SIOCSIFHWADDR)");
        close(fd);
        return -1;
    }

    // Закрываем сокет
    close(fd);

    return 0;
}

int droute(char *dr) {
  int sockfd;
  struct rtentry route;
  struct sockaddr_in *addr;
  int err = 0;

  // create the socket
 sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  memset(&route, 0, sizeof(route));
  addr = (struct sockaddr_in*) &route.rt_gateway;
  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = inet_addr(dr);
  addr = (struct sockaddr_in*) &route.rt_dst;
  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = inet_addr("0.0.0.0");
  addr = (struct sockaddr_in*) &route.rt_genmask;
  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = inet_addr("0.0.0.0");
  route.rt_flags = RTF_UP | RTF_GATEWAY;
  route.rt_metric = 0;
  if ((err = ioctl(sockfd, SIOCADDRT, &route)) != 0) {
    perror("SIOCADDRT failed");
  }
}

char *readfile(char *filename) {
  FILE *f;
  f = fopen(filename, "rb");
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *string = (char *)calloc(fsize + 1, sizeof(char));
  fread(string, fsize, 1, f);
  fclose(f);

  string[fsize] = 0;
  return string;
}

static int pivot_root(const char *new_root, const char *put_old) {
  return syscall(SYS_pivot_root, new_root, put_old);
}

int is_file(const char *path) {
  struct stat path_stat;
  stat(path, &path_stat);
  return S_ISREG(path_stat.st_mode);
}


static int offspring(void *arg) {
  int status;
  bbb *buf = (bbb *)arg;
  const char *put_old = "/oldrootfs";
  char path[512];
  char *bufp = malloc(256);
  bufp = buf->path;
  printf(bufp);

  if (mount(bufp, bufp, NULL, MS_BIND, NULL) == -1)
    perror("mount-MS_BIND");
  snprintf(path, sizeof(path), "%s/%s", bufp, put_old);
  printf(path);
  if (mkdir(path, 0777) == -1)
    perror("mkdir");
  char *dest = malloc(128);
  sprintf(dest,"%s/dev",bufp);
  mount("/dev",dest,"tmpfs", MS_BIND | MS_NOSUID | MS_STRICTATIME,NULL);
  
  if (pivot_root(bufp, path) == -1)
    perror("pivot_root");
  if (mount("proc", "/proc", "proc",0, NULL) <0)
    printf("error proc mount: %s\n",strerror(errno));
  
  mount("t", "/sys", "sysfs", 0, NULL);
/*  umount("/oldrootfs/dev/pts");
  umount("/oldrootfs/dev/shm");
  umount("/oldrootfs/dev/mqueue");
  umount("/oldrootfs/dev/hugepages");
  umount("/oldrootfs/dev");
  umount("/oldrootfs/run/lock");
  umount("/oldrootfs/run/rpc_pipefs");
  umount("/oldrootfs/run/user/0");
  umount("/oldrootfs/run");
  umount("/oldrootfs/sys/kernel/security");
  umount("/oldrootfs/sys/fs/cgroup");
  umount("/oldrootfs/sys/fs/pstore");
  umount("/oldrootfs/sys/fs/bpf");
  umount("/oldrootfs/sys/kernel/debug");
  umount("/oldrootfs/sys/kernel/config");
  umount("/oldrootfs/sys/fs/fuse/connections");
  umount("/oldrootfs/sys/kernel/tracing");
  umount("/oldrootfs/sys");
  umount("/oldrootfs/proc/sys/fs/binfmt_misc");
  umount("/oldrootfs/proc/fs/nfsd");
  umount("/oldrootfs/proc");
  umount("/oldrootfs/boot"); */
  umount2("/oldrootfs",MNT_DETACH); 
  rmdir("/oldrootfs");

//    char *argh[] = {"/sbin/init","5", NULL};
      setip("bfg1",buf->ip,buf->mask);
      droute(buf->route);
      execlp(buf->exe, buf->exe,  NULL);
//      execlp("systemctl", "systemctl" , "restart", "autofs", NULL);
//      execlp("/bin/bash","/bin/bash", "/start.sh", NULL);  
  wait(NULL);
  
  free(dest);
  return 0;
}


#define STACK_SIZE (1024 * 1024)

int main(int argc, char **argv) {
  int ret;
  int status;
  struct stat *buf;
  char *id = (char*) malloc(4);
  srand(time(0));
  unsigned int i;
    
  bbb *bufb = malloc(sizeof(bbb));
//  char *p = malloc(256);
//  sprintf(bufb, "%s", argv[1]);
  //printf("%s\n", bufp);
//  bbb *bufp = (bbb *);
  bufb->path = argv[1];
  bufb->ip = argv[2];
  bufb->mask = argv[3];
  bufb->route = argv[4];
  bufb->exe =argv[5];
//  char *start = readfile(argv[2]);
//  sprintf(p, "%s/start.sh", bufp);
//  FILE *fp = fopen(p, "wb");
//  fwrite(start, sizeof(char), strlen(start), fp);
//  fclose(fp);
//  char *bn = readfile("buildnumber");
 // sprintf(p, "%s/buildnumebr", bufp);
//  fp = fopen(p, "wb");
//  fwrite(bn, sizeof(char), strlen(bn), fp);
 // fclose(fp);


  system("mount --make-rprivate  /");
  char *stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
  if (stack == MAP_FAILED)
    perror("mmap");
  peer(rands(4));
  pid_t pid = clone(offspring, stack + STACK_SIZE, CLONE_NEWNET | CLONE_NEWUTS | CLONE_NEWIPC | CLONE_NEWPID | CLONE_NEWNS | SIGCHLD, (void *)bufb);
  if(pid<0)
	fprintf(stderr, "clone failed %s\n", strerror(errno));
  ns_setup(pid);

  if (wait(NULL) == -1)
    perror("wait");
 // free(bn);
//  free(start);
  exit(EXIT_SUCCESS);
}


void peer(char *bfg)
{
    char *set_bfg = malloc(64);
    char *set_bfg_up = malloc(64);;
    char *add_bfg_to_bridge = malloc(64);
    printf("bfg is %s\n",bfg);
    sprintf(set_bfg,"ip link add %s type veth peer name bfg1",bfg);
    system(set_bfg);
    sprintf(set_bfg_up,"ip link set %s up",bfg);
    system(set_bfg_up);
    sprintf(add_bfg_to_bridge,"brctl addif br0 %s",bfg);
    system(add_bfg_to_bridge);
}

void ns_setup(pid_t pid)
{
    char *ns = malloc(32);
    sprintf(ns,"ip link set bfg1 netns %d",pid);
    system(ns);
}
