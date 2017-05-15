#include <stdio.h>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>

#define MAX_PROC 100
#define MAX_NAME 100
#define MAX_ARGS 100
#define MAX_ARGS_LEN 100

char name[MAX_PROC][MAX_NAME];
char * args[MAX_PROC][MAX_ARGS_LEN + 1];
int respawnable[MAX_PROC];
pid_t pid_list[MAX_PROC];
pid_t pid_list_tries[MAX_PROC];

int global_argc;
char ** global_argv;

int proc_count = 0;

int proc_number = 0;


void handler(int sig) {
    for (int i=0; i<proc_number; i++) {
        if (pid_list[i] > 0) {
            kill(pid_list[i], SIGKILL);
        };
    };
    start(global_argc, global_argv);
};

int createFile(int i) {
	char file[MAX_NAME+MAX_PROC+10];
	sprintf(file, "/tmp/%i%s", i, name[i]);
	int j = 6;
	while(file[j] != 0) {
		if (file[j] == '/') {
			file[j] = '-';
		}
		j++;
	}
	FILE * fp = fopen(file, "w");
	if (fp == NULL) {
        syslog(LOG_WARNING, "Error openning file: /tmp/%i%s\n", i, name[i]);
		exit(1);
	}
	fprintf(fp, "%i", pid_list[i]);
	fclose(fp);
}

int removeFile(int i) {
	char file[MAX_NAME+MAX_PROC+10];
    sprintf(file, "/tmp/%i%s", i, name[i]);
	int j = 6;
	while(file[j] != 0) {
		if (file[j] == '/') {
			file[j] = '-';
		}
		j++;
	}

	if (remove(file) != 0) {
        syslog(LOG_WARNING, "Error: cannot delete /tmp/%i%s\n", i, name[i]);
		exit(1);
	}
}

int spawn(int i, int need_sleep) {
	int cpid = fork();

	switch (cpid) { 
    	case -1:
            syslog(LOG_WARNING, "%s\n", "Error: cannot fork");
    		return -1;
    		break;
    	case 0:
            syslog(LOG_NOTICE, "%d spawned, pid = %d\n", i, cpid);
            if (need_sleep == 1) {
                sleep(3600);
                need_sleep = 0;
            }
            cpid = getpid();
            if (execvp(name[i], args[i]) < 0) {
                syslog(LOG_WARNING, "%s %s", "Error exec programm", name[i]);
                exit(1);
            }
            exit(0);
    	default:
    		pid_list[i] = cpid;
    		createFile(i);
    		proc_count++;
    		return 0;
	}
}

int start(int argc, char * argv []) {
    if (argc != 2) {
        syslog(LOG_WARNING, "Error: incorrect arguments\n");
        exit(1);
    }   

    FILE * fp = fopen(argv[1], "r");

    if (fp == NULL) {
        syslog(LOG_WARNING, "Error: incorect config path\n");
        exit(1);
    }

    int i = 0;

    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char * token;   
    const char delims[4] = ", \n";

    while ((read = getline(&line, &len, fp)) != -1 && i < MAX_PROC) {
        token = strtok(line, delims);
        strcpy(name[i], token);
        
        token = strtok(NULL, delims);
        if (strcmp(token, "wait") == 0) {
            respawnable[i] = 0;
        } else {
            respawnable[i] = 1;
        }

        int j = 0;
        while((token = strtok(NULL, delims)) && j < MAX_ARGS_LEN) {
            args[i][j] = (char*) malloc(MAX_ARGS);
            strcpy(args[i][j], token);
            j++;
        }

        args[i][j] = NULL;

        i++;
    }
    
    proc_number = i;

    fclose(fp);

    for (i = 0; i < proc_number; i++) {
        spawn(i, 0);
    }

    signal(SIGHUP, handler);

    while (proc_count) {
        int stat;
        int i;
        pid_t cpid = wait(&stat);
        for (i = 0; i < proc_number; i++) {
            if (pid_list[i] == cpid) {
                syslog(LOG_NOTICE, "Child number %d pid %d finished\n",i,cpid);
                proc_count--;
                removeFile(i);

                if (respawnable[i]) {
                    if (stat != 0) {
                        pid_list_tries[i]++;
                    }
                    if (pid_list_tries[i] > 50) {
                        spawn(i, 1);
                        pid_list_tries[i] = 0;
                    } else {
                        spawn(i, 0);
                    }
                }
            }
        }
    }
}

int main(int argc, char * argv []) {

    int k;

    for (k=0;k<100;k++) {
        pid_list_tries[k] = 0;
    }


    switch (fork()) {
        case -1:
            syslog(LOG_WARNING, "%s\n", "Error starting daemon");
            exit(1);
        case 0:
            umask(0);
            setsid();
            chdir("/");
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
            global_argc = argc;
            global_argv = argv;
            start(argc, argv);
            return 0;
        default:
            return 0;
    };

	return 0;
}