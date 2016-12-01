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
#define LIMIT 20


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



/* Function to print the list*/
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

/*Function to add a node at the end of the list*/
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


/*Function to delete a node with pid = f_pid */
int delete_node(struct node **root, int nodes)
{
    struct node *temp;
    struct node *temp1;
    int flag = false;
    for(int i = nodes; i>10;i--)
    {
        temp = *root;
        *root = (*root)->next;
        free(temp);
    }
}

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


/*void parse(char line[], int len, char **arr, int *cnt)
  {	
  int i=0;
  int j=0;
  char my_str[50][50];
  for(int p=0;p<len+1;p++)
  {  
  if(line[p] == ' ' || line[p] == '\r' || line[p] == '\t' || line[p] == '\0' ){
  my_str[i][j] = '\0';
  arr[i] = my_str[i];
  i++;j=0;
  ++(*cnt);
  continue;
  }
  my_str[i][j] = line[p];	
  j++;
  }
  printf("\n");

  }*/
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


/*function returns true it there is any some
  background job running.false otherwise.*/
int check_bckgrnd_process(struct proc proc_arr[])
{
    int ret_value = false;
    for(int i=0;i<SIZE;i++){
        if(proc_arr[i].proc_id != 0){
            ret_value = true;
            break;
        }
    }
    return ret_value;
}


void clear_bakgrnd_procs(struct proc cp_arr[])
{
    int ret;
    int return_status;
    for(int k=0;k<SIZE;k++)
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

    char temp_line[20];
    int return_val,count1 = 0;

    for (int j = 0;j < SIZE; j++){
        cp_arr[j].proc_id = 0;
        strcpy(cp_arr[j].proc_name,"");
    }

    signal(SIGINT, SIG_IGN);

    while(true)
    {
        memset(line,'\0',SIZE);

        len =0;
        back_flag = 0;
        words_count = 0;
        for(int i=0;i<50;i++)
            cmd[i] = NULL;
        printf("%sk_sh >%s", KBLU, KNRM);

        if(fgets(line,sizeof(line),stdin) != NULL)
        {
            if (line[0] == '\n')
                continue;

            len = strlen(line);
            line[len-1] = '\0';
            len = strlen(line);
            strip_white_spaces(line);
            len = strlen(line);
            if(line[len-1] == '&')
            {
                int i;
                back_flag = true;
                line[len-1] = '\0';
                strip_white_spaces(line);
                len = strlen(line);
            }

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
                        printf("atoi error\n");
                    }
                }

            }

            add_node(&root,++count,line);  //add command name to the list to maintain the history
            nodes = count_nodes(&root);
            if (nodes > 10) {
                delete_node(&root, nodes);
            } 

            if(0 == strcmp(line,"exit")) 
                exit(0);

            if(0 == strcmp(line, "history"))
            {
                //display the list
                print_list(&root);
                continue;
            }

            //parse user input and save them as tokens into cmd array
            parse(line,len,cmd,&words_count);

	    if(0 == strcmp(cmd[0],"cd")) {
		return_val = chdir(cmd[1]);
		if (return_val != 0)
			printf("%s\n", strerror(errno));
		continue;
	    }

            cpid1 = fork();
            if(cpid1 < 0) {printf("ERROR in fork.\n");continue;}
            else if(cpid1 == 0)
            {

                /*This is child process*/
                if (0 > execvp(cmd[0], (char **)cmd))
                {
                    printf("ERROR: %s: %s\n", cmd[0], strerror(errno));
                    exit(-1);
                }
            }
            else if(cpid1 > 0)
            {
                if(back_flag != 1)
                {
                    waitpid(cpid1,&return_status,0);
                    //if (WIFEXITED(return_status) || WIFSIGNALED(return_status)) 
                    //{
                    //    printf("\nchild process %d[%s]   Done.   \n",cpid1, name);
                    //}

                    /*check if any pending background processes exist*/
                    status = check_bckgrnd_process(cp_arr);
                    if(status) {
                        clear_bakgrnd_procs(cp_arr);
                    }

                } else if(back_flag == 1)
                {

                    for( s=0;s <SIZE;s++)
                    {
                        if(cp_arr[s].proc_id == 0){
                            break;}
                    }
                    if (s == SIZE)
                    {
                        printf("Maximum limit of background processes running already..\n");
                        printf("Try again after some time..\n");
                        continue;
                    }
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

