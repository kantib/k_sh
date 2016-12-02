/*  -------------------------------------------------------------------*/
/*                PROGRAM - UNIX SHELL AND HISTORY FEATURE  
                  This program provides the user with a shell like interface.
                  User is provided "k_sh" prompt to run basic Linux commands.
                  Some other common features we see on the standard shells are
                  supported by this program as well.
                  1. history command - shows last 10 commands executed on the shell.
                  2. & - support for running a process in the background.  
                  3. !! - special command to run the last run command
                  4. !<number> - special command to run a command from the history
                  if it exists in the history.
                  5. cd <absolute directory path> - change directory

                  Version 1.1 - Author - Kanti Bhat(SJSUID - 011488386)  */
/*  --------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

#define true 1
#define false 0
#define SIZE 100

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KBLU  "\033[1m\033[34m"

struct proc{
    int proc_id;
    char proc_name[20];
};

struct node{
    int cmd_cnt;
    char pname[100];
    struct node *next;
};


/* Function to print the list */
void print_list(struct node **root)
{
    struct node *temp;
    temp = *root;
    while(temp !=0)
    {
        printf("%d %s \n",temp->cmd_cnt,temp->pname);
        temp = temp->next;
    }
}

/* Function to add a node at the end of the list */
void add_node(struct node** root,int cmd_count, char f_pname[])
{
    struct node *temp;
    struct node *new_node;
    temp = *root;
    if (*root != NULL) {
        while(temp->next != 0) {
            temp = temp->next;
        }

        new_node = (struct node *)malloc(sizeof(struct node));
        new_node->cmd_cnt = cmd_count;
        strcpy(new_node->pname, f_pname);
        new_node->next = 0;
        temp->next = new_node;
    } else {

        *root = (struct node *)malloc(sizeof(struct node));
        (*root)->cmd_cnt = cmd_count;
        strcpy((*root)->pname, f_pname);		
        (*root)->next = NULL;
    }
}

/*  Function to count the NODES in the list  */
int count_nodes(struct node **root)
{
    int count =0;
    struct node *temp;
    if(*root != NULL){
        temp = *root;
        while(temp != NULL){
            count++;
            temp = temp->next;
        }
    }	
    return count;
}


/*  Function to delete a node in the beginning of the list.
    oldest command entered is removed from the history list.  */
int delete_node(struct node **root, int nodes)
{
    struct node *temp;
    struct node *temp1;
    int flag = false;
    int i; 
    for(i = nodes; i>10;i--)
    {
        temp = *root;
        *root = (*root)->next;
        free(temp);
    }
}

/* Function to find a node from the list.
   If command_count field matches with of any node, function
   returns true. false otherwise.  */
int find_node(struct node **root, int count, char *line)
{
    int ret = false;
    struct node *temp;
    temp = *root;
    while(true) {
        if(*root != NULL) {
            if(temp->cmd_cnt == count) {
                ret = true;
                strcpy(line, temp->pname);
                break;
            }
            else {
                temp = temp->next;
                if(temp == NULL) {
                    ret = false;
                    break;
                }
            }
        }
    }
    return ret;
}

/* Function to parse the user input (command) and save them
   into an array of strings.  */
void parse(char line[], int len, char **arr, int *cnt)
{
    char my_str[50][50];
    int i, j, k;
    i  = 0;
    j = 0;

    do {
        while(line[i] == ' ' || line[i] == '\t' || line[i] == '\n') {
            i++;
        }

        if (line[i] == '\0')
            break;

        k = 0;
        while(line[i] != ' ' && line[i] != '\t' && line[i] != '\n' && line[i] != '\0') {
            my_str[j][k] = line[i];
            i++;
            k++;
        }
        my_str[j][k] = '\0';
        arr[j] = my_str[j];
        ++(*cnt);
        j++;
    } while (line[i] !=  '\0');
}


/* Function to check if any background process 
   exists. function returns true it there is some
   background job running.false otherwise.  */
int check_bckgrnd_process(struct proc proc_arr[])
{
    int ret_value = false;
    int i; 
    for(i=0;i<SIZE;i++){
        if(proc_arr[i].proc_id != 0){
            ret_value = true;
            break;
        }
    }
    return ret_value;
}


/* Function to show the status of processes running in the background.
   Erase the array entry of processes that have completed. */
void clear_bakgrnd_procs(struct proc cp_arr[])
{
    int ret, k;
    int return_status;
    for(k=0;k<SIZE;k++)
    {
        if(cp_arr[k].proc_id != 0)
        {
            return_status = 0;
            ret = waitpid(cp_arr[k].proc_id, &return_status,WNOHANG);
            if(ret && (WIFEXITED(return_status) || WIFSIGNALED(return_status)))
            {
                if (return_status == 0){
                    printf("child process [%d][%s]   Done.     \n",cp_arr[k].proc_id, cp_arr[k].proc_name);
                }
                cp_arr[k].proc_id = 0;
                cp_arr[k].proc_name[0] = '\0';
            }
        }
    }
}

/*  Function to strip leading/trailing and extra white spaces
    in between the user command options.  */ 
void strip_white_spaces(char a[])
{
    char b[SIZE];
    int i=0;
    int len;
    int j=0;
    memset(b,'\0',SIZE);
    while(1){
        while(a[i] == ' ' || a[i] == '\t' || a[i] == '\n') {
            i++;
        }
        if(a[i] == '\0'){
            b[j]= '\0';
            break;
        }
        while(a[i] !=  ' ' && a[i] != '\t' && a[i] != '\n' && a[i] != '\0') {
            if (i != 0) {				
                if (a[i-1] == ' ' || a[i-1] == '\t'){
                    b[j] = ' ';
                    j++;
                    b[j] = a[i];
                } else {
                    b[j] = a[i];				
                }
            } else {
                b[j] = a[i];
            }
            i++;
            j++;
        }
    }	
    memset(a, '\0',SIZE);
    i =0; int k;
    len = strlen(b);
    if(b[0] == ' '){
        for(k=1;k<len;k++){
            a[i] = b[k];
            i++;
        }
    } else {
        for(k=0;k<len;k++){
            a[i] = b[k];
            i++;
        }
    }
}

pid_t cur_process = 0;
int stopped = 0;

void* background_handler(void *arg) {
    pid_t *pid = (pid_t*)arg;
    int ret_status;
    waitpid(*pid, &ret_status, 0);
}

void signal_handler(int sig) {
    if (sig == SIGTSTP)
    {
        stopped=1;
        pthread_t tid;
        pid_t *pid = malloc(sizeof(pid_t));
        *pid = cur_process;
        //Start a thread to wait for this pid
        pthread_create(&tid, NULL, background_handler, (void*)pid);
    }

    if (cur_process != 0) {
        kill(cur_process, sig);
    }
}

void main()
{
    pid_t cpid1;
    int s, return_status,status,len = 0;
    int words_count = 0,back_flag = 0;
    int nodes = 0;
    int count = 0;
    char *cmd[50];
    char name[20];
    char c;
    int i;
    char line[SIZE];
    char temp_arr[100];
    struct proc cp_arr[SIZE];
    struct node *root = NULL;
    struct sigaction sa = {
        .sa_handler = signal_handler
    };

    char temp_line[20];
    int return_val,count1 = 0;
    int j;
    for (j = 0;j < SIZE; j++){
        cp_arr[j].proc_id = 0;
        strcpy(cp_arr[j].proc_name,"");
    }

    signal(SIGINT, signal_handler);
    //signal(SIGTSTP, SIG_IGN);
    sigaction(SIGTSTP, &sa, NULL);

    /* loop to execute user commands */
    while(true)
    {
        memset(line,'\0',SIZE);
        cur_process = 0;
        stopped = 0;

        len =0;
        back_flag = 0;
        words_count = 0;
        for(i=0;i<50;i++)
            cmd[i] = NULL;
        printf("%sk_sh >%s", KBLU, KNRM);

        /* save user command in line variable and process */
        if(fgets(line,sizeof(line),stdin) != NULL)
        {
            if (line[0] == '\n')
                continue;

            len = strlen(line);
            line[len-1] = '\0';
            len = strlen(line);
            strip_white_spaces(line);
            len = strlen(line);

            /* set flag is background service is requested */
            if(line[len-1] == '&')
            {
                int i;
                back_flag = true;
                line[len-1] = '\0';
                strip_white_spaces(line);
                len = strlen(line);
            }

            /* Handle Special commands !! and !<number> */
            if(line[0] == '!'){
                char temp_line[SIZE];
                int t;
                for( t = 1; t<strlen(line);t++){
                    temp_line[t-1] =line[t];
                }
                temp_line[t-1] = '\0';
                if((1 == strlen(temp_line)) && (0 == strcmp(temp_line, "!"))) {
                    if(root != NULL){
                        if(!(find_node(&root, count,line))) {
                            printf("command with number %d not found\n", count);
                        } else {
                            len = strlen(line);
                            printf("%s\n",line);
                        }
                    } else {
                        printf("No commands in the history\n");
                        continue;
                    }
                } else {
                    count1 = atoi(temp_line);
                    if(count1){
                        return_val = find_node(&root, count1, line);
                        if (!return_val){
                            printf("Command !%d not Found in history\n",atoi(temp_line));
                            continue;
                        } else {
                            len = strlen(line);
                            printf("%s\n",line);
                        }				
                    }else {
                        printf("Command not supported.\n");
                    }
                }

            }

            //update history list.
            add_node(&root,++count,line);  
            nodes = count_nodes(&root);
            if (nodes > 10) {

                /* delete the oldest commands
                   from the history list to maintain
                   10 latest commands in the history.*/
                delete_node(&root, nodes);
            } 

            /* handle exit command */
            if(0 == strcmp(line,"exit")) 
                exit(0);

            /* handle history command */
            if(0 == strcmp(line, "history"))
            {
                //display the list
                print_list(&root);
                continue;
            }

            //parse user input and save them as tokens into cmd array
            parse(line,len,cmd,&words_count);

            /* handle cd command */
            if(0 == strcmp(cmd[0],"cd")) {
                return_val = chdir(cmd[1]);
                if (return_val != 0)
                    printf("%s\n", strerror(errno));
                continue;
            }

            /* create a new child process
               to handle user command execution.*/
            cur_process = 0;
            stopped = 0;
            cpid1 = fork();
            if(cpid1 < 0) {printf("ERROR in fork.\n");continue;}
            else if(cpid1 == 0)
            {
                if (back_flag)
                    unsetenv("TERM");

                /*This is child process*/
                if (0 > execvp(cmd[0], (char **)cmd))
                {
                    printf("ERROR: %s: %s\n", cmd[0], strerror(errno));
                    exit(-1);
                }
            }
            else if(cpid1 > 0)
            {
                cur_process = cpid1;
                /* Parent process. */
                /* wait untill child process execution
                   is complete. */
                if(back_flag != 1)
                {
                    do {
                        waitpid(cpid1,&return_status,0);
                        usleep(10);
                        if (stopped)
                            break;

                    }while(errno == EINTR);

                    cur_process = 0;
                    stopped = 0;

                    /*check if any pending background processes exist*/
                    status = check_bckgrnd_process(cp_arr);
                    if(status) {
                        clear_bakgrnd_procs(cp_arr);
                    }

                } else if(back_flag == 1)
                {
                    /* If back ground process is requested, save child pid 
                       and loop back to the beginning. */

                    /* loop background process array 
                       to find the empty entry in the array. */
                    for( s=0;s <SIZE;s++)
                    {
                        if(cp_arr[s].proc_id == 0){
                            break;}
                    }
                    /* case for maximum background processes running already. */
                    if (s == SIZE)
                    {
                        printf("Maximum limit of background processes running already..\n");
                        printf("Try again after some time..\n");
                        continue;
                    }
                    /* save child pid in the background process array. */
                    cp_arr[s].proc_id = cpid1;
                    strcpy(cp_arr[s].proc_name,cmd[0]);
                    printf("[%d][%s] child created\n",cpid1, cmd[0]);
                }		
            }

        } else
        {
            continue;
        }
    }	
}
