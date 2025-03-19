#define _GNU_SOURCE
#include "cJSON.c"
#include <cap-ng.h>
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
#include <sys/capability.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <limits.h>

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)




#define MAXCAP 2048

unsigned long newval;

static pid_t sys_clone3(struct clone_args *args) {
  return syscall(__NR_clone3, args, sizeof(struct clone_args));
}

static int wait_for_pid(pid_t pid) {
  int status, ret;


again:
  ret = waitpid(pid, &status, 0);
  if (ret == -1) {
    if (errno == EINTR)
      goto again;

    return -1;
  }

  if (ret != pid)
    goto again;

  if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
    return -1;

  return 0;
}

char *readfile(char *filename) {
  FILE *f;
  f = fopen(filename, "rb");
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET); /* same as rewind(f); */

  char *string = (char *)calloc(fsize + 1, sizeof(char));
  fread(string, fsize, 1, f);
  fclose(f);

  string[fsize] = 0;
  return string;
}

#define ptr_to_u64(ptr) ((__u64)((uintptr_t)(ptr)))

#ifndef PR_GET_CAPBSET
#define PR_GET_CAPBSET 23
#endif

#ifndef PR_SET_CAPBSET
#define PR_SET_CAPBSET 24
#endif

unsigned long newval;
int cmd_getbcap;

char *captable[] = {"cap_dac_override",
                    "cap_dac_read_search",
                    "cap_fowner",
                    "cap_fsetid",
                    "cap_kill",
                    "cap_setgid",
                    "cap_setuid",
                    "cap_setpcap",
                    "cap_linux_immutable",
                    "cap_net_bind_service",
                    "cap_net_broadcast",
                    "cap_net_admin",
                    "cap_net_raw",
                    "cap_ipc_lock",
                    "cap_ipc_owner",
                    "cap_sys_module",
                    "cap_sys_rawio",
                    "cap_sys_chroot",
                    "cap_sys_ptrace",
                    "cap_sys_pacct",
                    "cap_sys_admin",
                    "cap_sys_boot",
                    "cap_sys_nice",
                    "cap_sys_resource",
                    "cap_sys_time",
                    "cap_sys_tty_config",
                    "cap_mknod",
                    "cap_lease",
                    "cap_audit_write",
                    "cap_audit_control",
                    "cap_setfcap"};

char *inttocap(unsigned long v) {
  char *str = NULL;
  int i;

  str = malloc(1);
  str[0] = '\0';
  for (i = 0; i < 31; i++) {
    if (v & (1 << (i + 1))) {
      char *tmp = captable[i];
      str = realloc(str, strlen(str) + 2 + strlen(tmp));
      sprintf(str + strlen(str), ",%s", tmp);
    }
  }
  return str;
}

int getbcap(void) {
  unsigned long bcap;
  int ret;
  unsigned int ver;

  ret = prctl(PR_GET_CAPBSET, &ver, &bcap);
  if (ret == -1)
    perror("prctl");
  if (ver != _LINUX_CAPABILITY_VERSION)
    printf("wrong capability version: %lu not %lu\n", ver,
           _LINUX_CAPABILITY_VERSION);
  printf("prctl get_bcap returned %lu (ret %d)\n", bcap, ret);
  printf("that is %s\n", inttocap(bcap));
  return ret;
}

int setbcap(unsigned long val) {
  int ret;

  ret = prctl(PR_SET_CAPBSET, _LINUX_CAPABILITY_VERSION, val);
  return ret;
}

unsigned long captoint(char *cap) {
  if (strcmp(cap, "cap_dac_override") == 0)
    return 1;
  else if (strcmp(cap, "cap_dac_read_search") == 0)
    return 2;
  else if (strcmp(cap, "cap_fowner") == 0)
    return 3;
  else if (strcmp(cap, "cap_fsetid") == 0)
    return 4;
  else if (strcmp(cap, "cap_kill") == 0)
    return 5;
  else if (strcmp(cap, "cap_setgid") == 0)
    return 6;
  else if (strcmp(cap, "cap_setuid") == 0)
    return 7;
  else if (strcmp(cap, "cap_setpcap") == 0)
    return 8;
  else if (strcmp(cap, "cap_linux_immutable") == 0)
    return 9;
  else if (strcmp(cap, "cap_net_bind_service") == 0)
    return 10;
  else if (strcmp(cap, "cap_net_broadcast") == 0)
    return 11;
  else if (strcmp(cap, "cap_net_admin") == 0)
    return 12;
  else if (strcmp(cap, "cap_net_raw") == 0)
    return 13;
  else if (strcmp(cap, "cap_ipc_lock") == 0)
    return 14;
  else if (strcmp(cap, "cap_ipc_owner") == 0)
    return 15;
  else if (strcmp(cap, "cap_sys_module") == 0)
    return 16;
  else if (strcmp(cap, "cap_sys_rawio") == 0)
    return 17;
  else if (strcmp(cap, "cap_sys_chroot") == 0)
    return 18;
  else if (strcmp(cap, "cap_sys_ptrace") == 0)
    return 19;
  else if (strcmp(cap, "cap_sys_pacct") == 0)
    return 20;
  else if (strcmp(cap, "cap_sys_admin") == 0)
    return 21;
  else if (strcmp(cap, "cap_sys_boot") == 0)
    return 22;
  else if (strcmp(cap, "cap_sys_nice") == 0)
    return 23;
  else if (strcmp(cap, "cap_sys_resource") == 0)
    return 24;
  else if (strcmp(cap, "cap_sys_time") == 0)
    return 25;
  else if (strcmp(cap, "cap_sys_tty_config") == 0)
    return 26;
  else if (strcmp(cap, "cap_mknod") == 0)
    return 27;
  else if (strcmp(cap, "cap_lease") == 0)
    return 28;
  else if (strcmp(cap, "cap_audit_write") == 0)
    return 29;
  else if (strcmp(cap, "cap_audit_control") == 0)
    return 30;
  else if (strcmp(cap, "cap_setfcap") == 0)
    return 31;
  else if (strcmp(cap, "cap_chown") == 0)
    return 32;
}

unsigned long parse_cap_string(char *capstring) {
  unsigned long tmp, newval = 0;
  char *token = strtok(capstring, ",");

  while (token) {
    tmp = captoint(token);
    if (tmp < 0)
      return -1;
    newval |= 1 << tmp;
    token = strtok(NULL, ",");
  }
  return newval;
}

int read_args(int argc, char *argv[]) {
  if (strcmp(argv[1], "get") == 0) {
    cmd_getbcap = 1;
    return 0;
  }
  if (strcmp(argv[1], "strset") == 0) {
    newval = parse_cap_string(argv[2]);
    if (newval < 0)
      return newval;
    return 0;
  }
  if (strcmp(argv[1], "set") != 0)
    return 1;
  if (argc != 3)
    return 1;
  newval = strtoul(argv[2], NULL, 10);
  return 0;
}

static void set_ambient_cap(int cap) {
  int rc;

  capng_get_caps_process();
  rc = capng_update(CAPNG_ADD, CAPNG_INHERITABLE, cap);
  if (rc) {
    printf("Cannot add inheritable cap\n");
    exit(2);
  }
  capng_apply(CAPNG_SELECT_CAPS);

  if (prctl(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, cap, 0, 0)) {
    perror("Cannot set cap");
    exit(1);
  }
}

static inline int av_tolower(int c) {
  if (c >= 'A' && c <= 'Z')
    c ^= 0x20;
  return c;
}

#define PR_CAP_AMBIENT 47
#define PR_CAP_AMBIENT_IS_SET 1
#define PR_CAP_AMBIENT_RAISE 2
#define PR_CAP_AMBIENT_LOWER 3
#define PR_CAP_AMBIENT_CLEAR_ALL 4

static int read_caps(int quiet, const char *filename, char *buffer) {
  int i = MAXCAP;
  if (!quiet) {
    fprintf(stderr, "Please enter caps for file [empty line to end]:\n");
  }
  while (i > 0) {
    int j = read(STDIN_FILENO, buffer, i);
    if (j < 0) {
      fprintf(stderr, "\n[Error - aborting]\n");
      exit(1);
    }
    if (j == 0 || buffer[0] == '\n') {
      /* we're done */
      break;
    }
    /* move on... */
    i -= j;
    buffer += j;
  }
  /* <NUL> terminate */
  buffer[0] = '\0';
  return (i < MAXCAP ? 0 : -1);
}

void set_cap(char *file, cap_t cap_d) {
  int tried_to_cap_setfcap = 0;
  char buffer[MAXCAP + 1];
  int retval, quiet = 0, verify = 0;
  cap_t mycaps;
  cap_value_t capflag;
  // if (argc < 3) {
  // usage();
  //}
  mycaps = cap_get_proc();
  if (mycaps == NULL) {
    fprintf(stderr, "warning - unable to get process capabilities"
                    " (old libcap?)\n");
  }
  // while (--argc > 0) {
  // const char *text;
  /*if (!strcmp(*++argv, "-q")) {
      quiet = 1;
      continue;
  }
  if (!strcmp(*argv, "-v")) {
      verify = 1;
      continue;
  }
  if (!strcmp(*argv, "-r")) {
      cap_d = NULL;
  } else {
      if (!strcmp(*argv,"-")) {
          retval = read_caps(quiet, *argv, buffer);
          if (retval)
              usage();
          text = buffer;
      } else {
          text = *argv;
      }
      cap_d = cap_from_text(text);
      if (cap_d == NULL) {
          perror("fatal error");
          usage();
      } */
  quiet = 0;
  verify = 0;

#ifdef DEBUG
  {
    ssize_t length;
    const char *result;
    result = cap_to_text(cap_d, &length);
    fprintf(stderr, "caps set to: [%s]\n", result);
  }
#endif

  // if (--argc <= 0)
  //    usage();
  /*
   * Set the filesystem capability for this file.
   */
  if (verify) {
    cap_t cap_on_file;
    int cmp;
    if (cap_d == NULL) {
      cap_d = cap_from_text("=");
    }
    cap_on_file = cap_get_file(file);
    if (cap_on_file == NULL) {
      cap_on_file = cap_from_text("=");
    }
    cmp = cap_compare(cap_on_file, cap_d);
    cap_free(cap_on_file);
    if (cmp != 0) {
      if (!quiet) {
        printf("%s differs in [%s%s%s]\n", file,
               CAP_DIFFERS(cmp, CAP_PERMITTED) ? "p" : "",
               CAP_DIFFERS(cmp, CAP_INHERITABLE) ? "i" : "",
               CAP_DIFFERS(cmp, CAP_EFFECTIVE) ? "e" : "");
      }
    }
    if (!quiet) {
      printf("%s: OK\n", file);
    }
  } else {
    if (!tried_to_cap_setfcap) {
      capflag = CAP_SETFCAP;
      /*
       * Raise the effective CAP_SETFCAP.
       */
      if (cap_set_flag(mycaps, CAP_EFFECTIVE, 1, &capflag, CAP_SET) != 0) {
        perror("unable to manipulate CAP_SETFCAP - "
               "try a newer libcap?");
        exit(1);
      }
      if (cap_set_proc(mycaps) != 0) {
        perror("unable to set CAP_SETFCAP effective capability");
        exit(1);
      }
      tried_to_cap_setfcap = 1;
    }
    retval = cap_set_file(file, cap_d);
    if (retval != 0) {
      int explained = 0;
#ifdef linux
      cap_value_t cap;
      cap_flag_value_t per_state;
      for (cap = 0; cap_get_flag(cap_d, cap, CAP_PERMITTED, &per_state) != -1;
           cap++) {
        cap_flag_value_t inh_state, eff_state;
        cap_get_flag(cap_d, cap, CAP_INHERITABLE, &inh_state);
        cap_get_flag(cap_d, cap, CAP_EFFECTIVE, &eff_state);
        if ((inh_state | per_state) != eff_state) {
          fprintf(stderr, "NOTE: Under Linux, effective file capabilities must "
                          "either be empty, or\n"
                          "      exactly match the union of selected permitted "
                          "and inheritable bits.\n");
          explained = 1;
          break;
        }
      }
#endif /* def linux */

      fprintf(stderr, "Failed to set capabilities on file `%s' (%s)\n", file,
              strerror(errno));
    }
  }
  if (cap_d) {
    cap_free(cap_d);
  }
}

int is_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

int cnm;
char *fromleft[128];

void fmount(cJSON *bundle) {
  cnm = 0;
  fromleft[128] = malloc(65536);
  cJSON *root = cJSON_GetObjectItemCaseSensitive(bundle, "root");
  cJSON *path = cJSON_GetObjectItemCaseSensitive(root, "path");

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
    char *left = (char *)malloc(1024);
	  char *mid1 = (char *)malloc(1024);
	  char *mid2 = (char *)malloc(1024);
	  char *src = (char *)malloc(1024);
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
	sprintf(left, "%s%s", path->valuestring, destination->valuestring);
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
      }
     if ((is_file(left)) && (access(left, F_OK ))) {
         FILE *fd = fopen(left,"w");
         fputs(" ",fd);
         fclose(fd);
      }	
    
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
	fromleft[cnm] = left;
	cnm++;
     } else
     {
	fromleft[cnm] = left;
	cnm++;
        err = mount(src, left, type->valuestring, mflags, mid2);
        printf("%s\n%d\n",left, err);
     }
  }
}

void outhatcaps(cJSON *bundle) {
  long int ret;

  cJSON *process = cJSON_GetObjectItemCaseSensitive(bundle, "process");
  cJSON *argvs = cJSON_GetObjectItemCaseSensitive(process, "args");
  cJSON *arg = argvs->child;
  char *exe = malloc(1024);
  sprintf(exe, "%s", arg->valuestring);
  char *op = malloc(1024);
  char *ops = malloc(1024);
  char *fepcaps = malloc(2048);
  cJSON *capabilities =
      cJSON_GetObjectItemCaseSensitive(process, "capabilities");
  cJSON *caps = NULL;
  cJSON *cap = NULL;

  cap_t cap_d = NULL;
  int ecap[32];
  cJSON_ArrayForEach(caps, capabilities) {
    printf(caps->string);
    cap = cJSON_GetObjectItemCaseSensitive(caps, caps->string);
    int *capi = malloc(cJSON_GetArraySize(cap) + 1);
    cap_t capss;
    char *txt = malloc(64);
    int i = 0;
    cJSON_ArrayForEach(cap, caps) {
      if (!strcmp(caps->string, "permitted")) {
        for (i = 0; i < strlen(cap->valuestring); i++)
          op[i] = av_tolower(cap->valuestring[i]);
        op[i] = 0;
        if (ecap[captoint(op)] == 1) {
          sprintf(ops, "%s,", op);
          strcat(fepcaps, ops);
        } else {
          sprintf(ops, "%s+p", cap->valuestring);
          cap_d = cap_from_text(ops);
          printf("%s\n", ops);
          set_cap(exe, cap_d);
        }
      }
      if (!strcmp(caps->string, "effective")) {
        // sprintf(op,"%s=+e",captable[i]);
        // cap_d = cap_from_text(op);
        // set_cap(exe,cap_d);
        for (i = 0; i < strlen(cap->valuestring); i++)
          op[i] = av_tolower(cap->valuestring[i]);
        op[i] = 0;
        ecap[captoint(op)] = 1;
      }
      if (!strcmp(caps->string, "inheritable")) {
        /*for (i = 0; i < strlen(cap->valuestring); i++)
op[i] = av_tolower(cap->valuestring[i]);
        op[i] = 0;
        icap[captoint(op)] = 1;*/

        sprintf(op, "%s=+i", cap->valuestring);
        cap_d = cap_from_text(op);
        set_cap(exe, cap_d);
      }
      if (!strcmp(caps->string, "bounding")) {
        for (i = 0; i < strlen(cap->valuestring); i++)
          op[i] = av_tolower(cap->valuestring[i]);
        newval = parse_cap_string(op);
      }
      ret = setbcap(newval);

      // return execl(arg->valuestring, arg->valuestring, NULL);
      if (!strcmp(caps->string, "ambient")) {
        for (i = 0; i < strlen(cap->valuestring); i++)
          op[i] = av_tolower(cap->valuestring[i]);
        set_ambient_cap(captoint(op));
      }
    }
  }
  fepcaps[strlen(fepcaps) - 1] = 0;
  strcat(fepcaps, "+ep");
  cap_d = cap_from_text(fepcaps);
  set_cap(exe, cap_d);
  printf(fepcaps);
  free(fepcaps);
  free(op);
  free(ops);
}

void fcgroups(cJSON *bundle) {
  char *op = malloc(1024);
  char *ops = malloc(1024);
  cJSON *lin = cJSON_GetObjectItemCaseSensitive(bundle, "linux");
  cJSON *root = cJSON_GetObjectItemCaseSensitive(bundle, "root");
  cJSON *path = cJSON_GetObjectItemCaseSensitive(root, "path");
  cJSON *cgroupsPath = cJSON_GetObjectItemCaseSensitive(lin, "cgroupsPath");
  int i = 0;
  int s1 = strlen("systems.slice");
  int s2 = strlen("docker");
  int s3 = strlen(cgroupsPath->valuestring);
  char *sysslice = malloc(s1 + 1);
  char *docker = malloc(s2 + 1);
  char *hash = malloc(s3 + 1);

  char *end;

  end = strchr(cgroupsPath->valuestring, ':');
  int slen = end - cgroupsPath->valuestring;
  memcpy(sysslice, cgroupsPath->valuestring, slen);
  sysslice[slen] = 0;

  end = strchr(cgroupsPath->valuestring, ':');
  slen = end - cgroupsPath->valuestring;
  memcpy(docker, cgroupsPath->valuestring, slen);
  docker[slen] = 0;

  end = strchr(cgroupsPath->valuestring, 0);
  slen = end - cgroupsPath->valuestring;
  memcpy(hash, cgroupsPath->valuestring, slen);
  hash[slen] = 0;

  sprintf(op, "/sys/fs/cgroup/%s/%s-%s.scope", sysslice, docker, hash);
  sprintf(ops, "%scgroup", path->valuestring);

  mount(op, ops, "cgroup", 1, NULL);

  free(op);
  free(ops);
}

void fhooks(cJSON *bundle) {
  int j;
  cJSON *hooks = cJSON_GetObjectItemCaseSensitive(bundle, "hooks");
  cJSON *hook = NULL;
  char *const hcs = malloc(1024);
  cJSON_ArrayForEach(hook, hooks) {
    cJSON *hchild = hook->child;
    cJSON *path = cJSON_GetObjectItemCaseSensitive(hchild, "path");
    cJSON *hurgs = cJSON_GetObjectItemCaseSensitive(hchild, "args");
    cJSON *hurg = NULL;
    j = 1;
    hcs[0] = *path->valuestring;
    // char opts[0] = left;
    // opts[0] = path->valuestring;
    cJSON_ArrayForEach(hurg, hurgs) {
      // sprintf(left," %s ",hurg->valuestring);
      // strcat(opts,left);
      hcs[j] = *hurg->valuestring;
      j++;
    }
    execv(path->valuestring, &hcs);
  }
}

cJSON *bundle;


#ifndef CLONE_PIDFD
#define CLONE_PIDFD 0x00001000
#endif

#ifndef __NR_clone3
#define __NR_clone3 -1
#endif

#define ptr_to_u64(ptr) ((__u64)((uintptr_t)(ptr)))






void energise1(cJSON *bundle) {

  cJSON *process = cJSON_GetObjectItemCaseSensitive(bundle, "process");
  cJSON *argvs = cJSON_GetObjectItemCaseSensitive(process, "args");
  cJSON *arg = argvs->child;
  char *exe = malloc(1024);
  sprintf(exe, "%s", arg->valuestring);

  cJSON *root = cJSON_GetObjectItemCaseSensitive(bundle, "root");
  cJSON *path = cJSON_GetObjectItemCaseSensitive(root, "path");
  cJSON *hyrgs;
  char *const hgs = malloc(1024);
  char *const hes = malloc(1024);

  int j = 1;

  cJSON_ArrayForEach(hyrgs, argvs) {
    hgs[j] = *hyrgs->valuestring;
    j++;
  }

  cJSON *env = NULL;
  cJSON *envs = cJSON_GetObjectItemCaseSensitive(process, "env");
  j = 1;
  cJSON_ArrayForEach(env, envs) {
    hes[j] = *env->valuestring;
    j++;
  }
}



static int childFunc(void *arg)
{
    struct utsname uts;
    /* Change hostname in UTS namespace of child. */
        printf("Hi");

    sleep(200);
    return 0;           /* Child terminates now */
}






/*
static int wait_for_pid(pid_t pid)
{
	int status, ret;

again:
	ret = waitpid(pid, &status, 0);
	if (ret == -1) {
		if (errno == EINTR)
			goto again;

		return -1;
	}

	if (ret != pid)
		goto again;

	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
		return -1;

	return 0;
}


*/
typedef struct bbb {
    char* path;
    char* exe;
} bbb;



static int offspring(void *arg) {
  int status;
  bbb *buf = (bbb *)arg;
  const char *put_old = "/oldrootfs";
  char path[512];
  bbb *bufp = malloc(256);
  printf(bufp->path);
  char *exe = malloc(512);
  exe = bufp->exe;
  if (mount(bufp->path, put_old, NULL, MS_BIND, NULL) == -1)
    perror("mount-MS_BIND");
  snprintf(path, sizeof(path), "%s/%s", bufp, put_old);
  printf(path);
  if (mkdir(path, 0777) == -1)
    perror("mkdir");
  char *dest = malloc(128);
  sprintf(dest,"%s/dev",bufp);
  mount("/dev",dest,"tmpfs", MS_BIND | MS_NOSUID | MS_STRICTATIME,NULL);
  
  syscall(SYS_pivot_root, path, put_old);
  if (mount("proc", "/proc", "proc",0, NULL) <0)
    printf("error proc mount: %s\n",strerror(errno));
  
 
//    char *argh[] = {"/sbin/init","5", NULL};

      execlp(bufp->exe, bufp->exe,  NULL);
//      execlp("systemctl", "systemctl" , "restart", "autofs", NULL);
//      execlp("/bin/bash","/bin/bash", "/start.sh", NULL);  
  wait(NULL);
  
//  free(dest);
  return 0;
}


#define STACK_SIZE (1024 * 1024)

int main(int argc, char **argv) {
  int ret;
  int status;
  struct stat *buf;
  char filename[64] = "config.json";

  unsigned int i;

  bbb *bufp = malloc(sizeof(bbb));

  char *string = readfile(filename);

  cJSON *bundle = cJSON_Parse(string);
  cJSON *process = cJSON_GetObjectItemCaseSensitive(bundle, "process");
  cJSON *argvs = cJSON_GetObjectItemCaseSensitive(process, "args");
  cJSON *arg = argvs->child;
  cJSON *root = cJSON_GetObjectItemCaseSensitive(bundle, "root");
  cJSON *path = cJSON_GetObjectItemCaseSensitive(root, "path");
  cJSON *lin = cJSON_GetObjectItemCaseSensitive(bundle, "linux");
  printf("%s\n", path->valuestring);
//  char *str = cJSON_Print(bundle);
//  printf("%s", str);

  cJSON *cwd = cJSON_GetObjectItemCaseSensitive(process, "cwd");
  if (chdir(path->valuestring)) {
    printf("Can't chdir");
  }
 energise1(bundle);
  
  
//  outhatcaps(bundle);
//  fhooks(bundle);



    struct clone_args args = {0}; 

  bufp->path=path->valuestring;
  bufp->exe=argvs->valuestring;
   char *stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
  if (stack == MAP_FAILED)
    perror("mmap");
  
   pid_t pid = clone(offspring, stack + STACK_SIZE, CLONE_NEWNET | CLONE_NEWUTS | CLONE_NEWIPC | CLONE_NEWPID | CLONE_NEWNS | SIGCHLD, (void *)bufp);
  if(pid<0)
	fprintf(stderr, "clone failed %s\n", strerror(errno));
   exit(EXIT_SUCCESS);
/*
    args.parent_tid = ptr_to_u64(&parent_tid); 
	  args.pidfd = ptr_to_u64(&pidfd); 
	  args.flags = CLONE_PARENT_SETTID | CLONE_NEWUTS | SIGCHLD;
	  args.exit_signal = SIGCHLD;
*/

  //  pid = sys_clone3((childFunc(&args) "ami"));
//     pid = clone(childFunc, stackTop, CLONE_NEWUTS | SIGCHLD | CLONE_NEWNS , "ami");
//    if (pid == -1)
//        errExit("clone");
//    printf("clone() returned %jd\n", (intmax_t) pid);
    /* Parent falls through to here */
               /* Give child time to change its hostname */
    /* Display hostname in parent's UTS namespace. This will be
       different from hostname in child's UTS namespace. */
  //  if (uname(&uts) == -1)
  //      errExit("uname");
  //  printf("uts.nodename in parent: %s\n", uts.nodename);
  //  while (1 == 1);
  //  if (waitpid(pid, NULL, 0) == -1)    /* Wait for child */
  //      errExit("waitpid");
  //  printf("child has terminated\n");

//  for(i = 0;i <= cnm; i++)
//  umount2(fromleft[i], MNT_FORCE);


  free(string);
  


}