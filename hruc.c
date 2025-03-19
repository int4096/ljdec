#define _GNU_SOURCE
#include <errno.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <sched.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <wait.h>
#include <sys/mount.h>
#include "cJSON.c"


#define PATH_MAX 512

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


static int pivot_root(const char *new_root, const char *put_old)
       {
           return syscall(SYS_pivot_root, new_root, put_old);
       }

int is_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

int cnm;
char* fromleft;

void fmount(char *bufp) {
  char filename[64] = "/root/hruk/config.json";
  char *string = readfile(filename);
  cJSON *bundle = cJSON_Parse(string);
  cnm = 0;
  char *str = malloc(512); 
  fromleft = malloc(65536);
  cJSON *root = cJSON_GetObjectItemCaseSensitive(bundle, "root");
  cJSON *path = cJSON_GetObjectItemCaseSensitive(root, "path");
    char *left = (char *)malloc(1024);
	  char *mid1 = (char *)malloc(1024);
	  char *mid2 = (char *)malloc(1024);
	  char *src = (char *)malloc(1024);

  cJSON *mounts = cJSON_GetObjectItemCaseSensitive(bundle, "mounts");
  cJSON *cmount = NULL;
  cJSON_ArrayForEach(cmount, mounts) {
    cJSON *destination =
        cJSON_GetObjectItemCaseSensitive(cmount, "destination");
    printf("%s", destination->valuestring);
    cJSON *type = cJSON_GetObjectItemCaseSensitive(cmount, "type");
    printf("%s", type->valuestring);
    cJSON *source = cJSON_GetObjectItemCaseSensitive(cmount, "source");
    printf("%s\n", source->valuestring);
    cJSON *options = cJSON_GetObjectItemCaseSensitive(cmount, "options");
    cJSON *option = NULL;
    int newinst = 0;
    int i = 0;
    int rbind = 0;
    int rbind2 = 0; 
    unsigned long mflags = 0;
    cJSON_ArrayForEach(option, options) {
      printf("%s\n", option->valuestring);
      if (strcmp(option->valuestring, "newinstance")) {
        if (!strcmp(option->valuestring, "nosuid"))
    if(mflags) mflags | MS_NOSUID; else mflags && MS_NOSUID;
        if (!strcmp(option->valuestring, "noatime"))
    if(mflags) mflags | MS_NOATIME; else mflags & MS_NOATIME; 
        if (!strcmp(option->valuestring, "nodev"))
    if(mflags) mflags | MS_NODEV; else mflags & MS_NODEV;
        if (!strcmp(option->valuestring, "nodiratime"))
    if(mflags) mflags | MS_NODIRATIME; else mflags & MS_NODIRATIME;
        if (!strcmp(option->valuestring, "noexec"))
	if(mflags) mflags | MS_NOEXEC; else mflags & MS_NOEXEC;
        if (!strcmp(option->valuestring, "ro"))
	if(mflags) mflags |  MS_RDONLY; else mflags &  MS_RDONLY;
        if (!strcmp(option->valuestring, "strictatime"))
	if(mflags) mflags | MS_STRICTATIME; else mflags & MS_STRICTATIME;          
        if (!strcmp(option->valuestring, "nostrictatime"))
	if(mflags) mflags | MS_RELATIME; else mflags & MS_RELATIME;
        if (!strcmp(option->valuestring, "lazytime"))
	if(mflags) mflags | MS_LAZYTIME; else mflags & MS_LAZYTIME;	
        if (!strcmp(option->valuestring, "sync"))
	if(mflags) mflags | MS_SYNCHRONOUS; else mflags & MS_SYNCHRONOUS;
        if (!strcmp(option->valuestring, "nodiratime"))
    if(mflags) mflags | MS_NODIRATIME; else mflags & MS_NODIRATIME;
        if (!strcmp(option->valuestring, "rprivate"))
    {
      rbind++;
    }
    if (strstr(option->valuestring, "rbind"))
    {
      rbind++;
    
    }
	if(strstr(option->valuestring,"mode") || strstr(option->valuestring,"size")){
	    sprintf(mid1,"%s,", option->valuestring); 
		strcat(mid2,mid1);
	}
      } else
        newinst = 1;
	
    }
	sprintf(left, "%s%s", bufp, destination->valuestring);
    if ((!strcmp(type->valuestring, "bind")))

     {
        	mflags = mflags | MS_BIND;
               src = source->valuestring;
		}

	
		if ((strstr(type->valuestring, "tmp")) ||
			(strstr(type->valuestring, "dev")) ||
			(strstr(type->valuestring, "sys")))
			{
				src = destination->valuestring;
				mflags = mflags | MS_BIND;
          mkdir(left,755);
        	
			} else 
			src = source->valuestring;
    if (strstr(type->valuestring, "cgroup"))
    {
		 	src = destination->valuestring;
    	mflags = mflags | MS_BIND;
     
    }
   
   	if (strstr(type->valuestring, "proc"))
      {   
	  mkdir(left,700);
          src = source->valuestring;
	mflags = mflags | MS_BIND;
      }
  //   if ((is_file(left)) && (!access(left, F_OK ))) {
  //       FILE *fd = fopen(left,"w");
  //       fputs(" ",fd);
  //       fclose(fd);
  //    }	
    
//	
//			mkdir(left,700);  
			int err;
    if (newinst)
      mflags = mflags | MS_SLAVE;

     if ((rbind > 1)) {
        err = mount(source->valuestring, left, "ext4", MS_BIND | MS_REC , NULL);
        printf("%s\n%d\n",source->valuestring,left, err);
        err = mount("none", left, NULL, MS_REC | MS_PRIVATE ,NULL);
        printf("%s\n%d\n",left, err);
        rbind = 0;
        sprintf(str,"%s\n",left);
	strcat(fromleft,str);
    	cnm = cnm + strlen(str);

     } else
     {
        sprintf(str,"%s\n",left);
	strcat(fromleft,str);
    	cnm = cnm + strlen(str);

        err = mount(src, left, type->valuestring, mflags, mid2);
        printf("%s\n%d\n",left, err);
     }

  }
fromleft[cnm] = 0;
//free(string);
//free(left);
//free(mid1);
//free(src);
//free(string);
}


static int child(void *arg)
{
   int status;
           char *bufp = (char *)arg;
           const char *put_old = "/oldrootfs";
           char path[PATH_MAX];
//             FILE *fp = fopen("/log","wb");
          
//           if (mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL) == -1)
	    printf(bufp);
		if (mount(bufp, bufp, NULL, MS_BIND, NULL) == -1)
               perror("mount-MS_BIND");


	          snprintf(path, sizeof(path), "%s/%s", bufp, put_old);
	         printf(path);
            if (mkdir(path, 0777) == -1)
               perror("mkdir");

           
	       if (pivot_root(bufp, path) == -1)
                         perror("pivot_root");

                 FILE *fp = fopen("/log","w");
		fputs("piit_root ok\n", fp);
		fclose(fp);
          	mount("tmpfs","/dev","tmpfs",MS_NOSUID | MS_STRICTATIME,NULL);
	        if (mount("proc", "/proc", "proc",0, NULL) <0)
		        printf("error proc mount: %s\n",strerror(errno));
	            mount("t", "/sys", "sysfs", 0, NULL); 
		//fmount(bufp);
                fp = fopen("/log","w");
		fputs("mount ok\n", fp);
		fclose(fp);

/*           if (umount2(put_old, MNT_DETACH) == -1)
              { 
		FILE *fc = fopen("/log","w");
		fputs("umount not  ok\n", fc);
		fclose(fc);
		} else 
		 { 
		FILE *fc = fopen("/log","w");
		fputs("umount ok\n", fc);
		fclose(fp);
		} */

            
                
           if (rmdir(put_old) == -1)
               perror("rmdir"); 

	       char* argh[] = { "/bin/bash", "/start.sh", NULL };
	        execv("/bin/bash", argh);
	        FILE *fe = fopen("/log","w");
		fputs("execv ok", fe);
		fclose(fe);


           
           wait(NULL);

  /*         fromleft[strlen(fromleft)] = 0;
  fromleft[strlen(fromleft)-1] = 0; */


/*  char *sda = (char *)malloc(512);
  char *end;
  int slen= 0;
 
  while (1)
  
  {
    end = strchr(fromleft, '\n');
    if (!end)
    {
	end = strchr(fromleft, 0);
	break;
    }
    slen = end - fromleft;
    memcpy(sda, fromleft, slen);
    sda[slen] = 0;
    umount2(sda, MNT_FORCE);
    printf("%s\n%d",sda,slen);
    fromleft = end + 1;
 } 

free(sda); */
fclose(fp);
return 0; 
}


#define STACK_SIZE (1024 * 1024)

int main(int argc, char **argv) {
  int ret;
  int status;
  struct stat *buf;
  char filename[64] = "/root/hruk/config.json";
 
  unsigned int i;
 

  char *string = readfile(filename);

  void *bufp = malloc(256);
  char *p = malloc(256);
  sprintf(bufp,"/guests/%s",argv[1]);
  printf("%s\n", bufp); 
 
  char* start = readfile("/root/hruk/start.sh");
  sprintf(p,"%s/start.sh",bufp);
  FILE *fp = fopen(p,"wb");
  fwrite(start,sizeof(char),strlen(start),fp);
  fclose(fp);
//  if (mount(bufp, bufp, NULL, MS_BIND, NULL) == -1)
//               perror("mount-MS_BIND");
	
// chmod(start,777);
          system("mount --make-rprivate  /");
          char *stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
           if (stack == MAP_FAILED)
               perror("mmap");

           if (clone(child, stack + STACK_SIZE,  SIGCHLD | CLONE_NEWNS | CLONE_NEWPID , bufp) == -1)
               perror("clone");

//           if (wait(NULL) == -1)
//               perror("wait");


  free(start);
  exit(EXIT_SUCCESS);
}