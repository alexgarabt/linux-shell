#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>

#define ERROR_MESSAGE ("An error has occurred\n")
#define EXIT_COMMAND ("exit")
#define CD_COMAND ("cd")
#define SPACE_DELIM (" ")
#define SPACE_CHAR (' ')
#define AMPERSAND_CHAR ('&')
#define AMPERSAND_STRING ("&")
#define REDIRECTION_CHAR ('>')
#define REDIRECTION_STRING (">")
#define NULL_INT (-666)
#define ERROR_INT (-999)
#define LJUM_DELIM ("\n")
#define MAX_TOKENS 100

#define CHECK_POINTER(ptr) \
    do{ \
        if(ptr==NULL){\
            fprintf(stderr,"%s",ERROR_MESSAGE);\
            exit(1);\
        } \
    } while(0)



void interactive_mode();
void batch_mode(char *file);

void run_command(char **command, int ouput_src);
void run_line_command_red(char *input);
void run_line_command_parll(char *input);
void run_line(char *input);

char **split_string(char *cad, char *delimiter);
int count_char(char *cad,char search);

int check_exit(char *input);
void exit_command();

int check_cd(char *input);
int cd_command(char *path);


int redirection_file(char *command);

int bool_all_space(char *input);
void free_command(char **command);


int main(int argc, char* argv[]){
    /*
    ./Uvash -> starts interactive mode.
    ./Uvash file -> starts batch mode and run commands in file.

    other cases is an error of use.
    */

    if(argc<=1){        //Starts interactive mode.
        
        interactive_mode();
        exit(0);
    }
    if(argc==2){        //Starts batch mode.
        
        batch_mode(argv[1]);
        exit(0);
    }
    else{               //Bad use of UVash shell.
        CHECK_POINTER(NULL);
    }
}

/*Function: starts the shell interactive mode.
  */
void interactive_mode(){

    char *input=NULL;                            //To enter in while loop.
    size_t len = 20;                        //Arbitrary size of the inputs can change. 
    int chars_read=0;                       // For the getline().

    while(1){ //Check until the user type exit command.

        //input=NULL;
        printf("UVash> ");
        chars_read = getline(&input, &len, stdin);//Get the typed command. Uses malloc to book memory.
    
        if((chars_read==1)&&(strcmp(input,LJUM_DELIM)==0)){//If users type '\n' or error.
            continue;
        }

        run_line(input);
    }

}    

/*Function: Starts the shell in the bathc mode.
  *file_name: File to execute.
*/
void batch_mode(char *file_name){

    //OPEN THE FILE.

    FILE *file;
    file =(FILE *)fopen(file_name,"r");
    CHECK_POINTER(file);

    char *input=NULL;                     //To enter in while loop.
    size_t len = 20;                        //Arbitrary size of the inputs can change. 
    int chars_read=0;                       // For the getline().

    while(1){ //Check until the user type exit command.
        
        chars_read = getline(&input, &len, file);//Get the typed command. Uses malloc to book memory.

        if(chars_read==-1){
            break;
        }

        if((chars_read==1)&&(strcmp(input,LJUM_DELIM)==0)){//If users type '\n' or error.
            continue;
        }


        run_line(input);
    }
    free(input);   
}

/*Function:  Run one command.
  **command: The command to be executed.
  return:    0 if every thing was okey.
             -1 if the typed command is exit.  
*/
void run_command(char **command, int output_src){
    

    pid_t pid_child;                        //PID of the forks.
        
    pid_child = fork();                 //Fork a child process.

    if(pid_child<0){                    //Error creating the child.
        CHECK_POINTER(NULL);
    }
    else if(pid_child==0){                   //This is the child code.

        if(output_src!=NULL_INT){           //Redirection dup the stdout and stderr.
            if((dup2(output_src,1)==-1)||dup2(output_src,2)==-1){
                fprintf(stderr,"%s",ERROR_MESSAGE);
            }
        }

        if(execvp(command[0],command)==-1){ //Error exec.
            fprintf(stderr,"%s",ERROR_MESSAGE);
        }
        exit(0);
    }
    else{                               //This is the father code.
   
        wait(NULL);
    }
}

/*Function: Gets a string and separate into substrings by de given delimiter. 
            
 *cad:      A string .
  return:   A pointer to a array of string.
  free:     Need to free each string in the array and the array. 
*/
char **split_string(char *cad, char *delimiter) {

    if(cad==NULL){
        return NULL;
    }

    char **sliped=(char**)malloc(MAX_TOKENS*sizeof(char*));
    CHECK_POINTER(sliped);


    int len=strlen(cad);
    char *str_cpy=(char*)malloc((len+1)*sizeof(char));
    strcpy(str_cpy,cad);
    
    char *token;
    char *tmp;
    int i=0;

    token = strtok(str_cpy,delimiter);
    
    while(token!=NULL){

        len=strlen(token);
        tmp =(char*)malloc((len+1)*sizeof(char));
        CHECK_POINTER(tmp);
        strcpy(tmp,token);
        sliped[i]=tmp;
        token = strtok(NULL,delimiter);   
        i++;
    }
    
    sliped[i]=NULL;
    free(str_cpy);
    return sliped;
}

/*Function: Check if the provided input is exit.
  return: 1 if is true.
  return: 0 if is false.
*/
int check_exit(char *input){
    if((strcmp(input,EXIT_COMMAND))==0){
        return 1;
    }
    return 0;
}

/*Function: Check if the provided input is cd.
  return: 1 if is true.
  return: 0 if is false.
*/
int check_cd(char *input){
    if((strcmp(input,CD_COMAND))==0){
        return 1;
    }
    return 0;
}

/*Function: Finialized the program.
*/
void exit_command(){
    exit(0);
}

/*Function: Try to move to the provided folder.
  return: 0 if it could.
  return: -1 an error ocurred.
*/
int cd_command(char *path){
    return chdir(path);
}

/*Function: Release reserved memory of slipt_string() function.
  **comand: Pointer to a String array.

  Realease the memory according to the slip_string() function reservation.
*/
void free_command(char **command){
        int i=0;
        while (command[i] != NULL) {
            free(command[i]);
            i++;
        }
        free(command[i]);
        free(command);
}

/*Function: Check if the given input has only spaces
  *input: input to check
  return: 1 if is true.
          0 if is not true.
*/
int bool_all_space(char *input){
    int i=0;
    while(input[i]!='\0'){
        if(input[i]!=SPACE_CHAR){return 0;}
        i++;
    }
    return 1;
}

/*Function: Count how many time the char REDIRECTION_CHAR is in the given string
  *cad:     The string how will be anlyze.
  return:   how may times shows is the string.
*/
int count_char(char *cad,char search){
    int re_counter=0;
    for(int i = 0; i < strlen(cad); i++) {
        if(cad[i]==search){re_counter++;}
    }
    return re_counter;
}

/*Function: Check if the given command has redirection in that case open the file to write.
  *command: Command to check.
  return:   ERROR_INT if are a error.
            NULL_INT if there is no redirection.
            (int) other of the filem to redirect.*/
int redirection_file(char *command){

    int file_output=NULL_INT;
    int re_count =0;
    char **tmp_re;
    char **tmp1;
    
    //Check if the command has redirrection.
    //Check how many > are.
    re_count = count_char(command,REDIRECTION_CHAR);
    if(re_count>1){
        fprintf(stderr,"%s",ERROR_MESSAGE);
        return ERROR_INT;
    }
    else if(re_count==1){
        tmp_re= split_string(command,REDIRECTION_STRING);
    
        //tmp = tmp_re[0];//COMMAND.

        if(tmp_re[1]==NULL){ //There is no file.
            fprintf(stderr,"%s",ERROR_MESSAGE);
            free_command(tmp_re);
            return ERROR_INT;
        }

        //COUNT files. [comand] > [file] [file] ... 
        tmp1 = split_string(tmp_re[1],SPACE_DELIM);
        free_command(tmp_re);

        re_count=0;
        while(tmp1[re_count]!=NULL){re_count++;}//COUNT FILES.

        if(re_count!=1){//ERROR.
            fprintf(stderr,"%s",ERROR_MESSAGE);
            free_command(tmp1);
            return ERROR_INT;
        }
        file_output = open(tmp1[0], O_WRONLY | O_CREAT | O_TRUNC, 0642);
        free_command(tmp1);
        return file_output;
    }
    else{
        file_output = NULL_INT;
        return file_output;
    }
}

/*Function: Run a line command of the shell.
  *input: String with the command input.
  return: 0 if every thing is okey.
          -1 if the typed command is exit.
*/
void run_line_command_red(char *input){

    char **tmp_re;
    char **command;
    int file_output=NULL_INT;

    //DIVIDE the command by redirections
    tmp_re= split_string(input,REDIRECTION_STRING);

    //Check if the command has redirrection.
    //Check how many > are.
    file_output=redirection_file(input);
    if(file_output==ERROR_INT){// FILE CANT BE OPENED.
        free_command(tmp_re);
        return;
    }
    //END OF REDIRECTION

    command = split_string(tmp_re[0], SPACE_DELIM); //Get the command divided. 
                                                      //comand[0]== the command.
                                                      //comand[ith]== the args.
    free_command(tmp_re);
        
    run_command(command,file_output);                  
    free_command(command);                    //Release the memory.        
}

/*Function: Run a command of the shell with redirections and paralles,
*/
void run_line_command_parll(char *input){
    char **tmp1;

    int count_parallel;
    int wstatus;

    pid_t *pid_childs;

    //START PARALLEL ANALYZE.
        
    //DIVIDE BY '&'
    tmp1=split_string(input,AMPERSAND_STRING);

    if(tmp1[0]==NULL){
        fprintf(stderr,"%s",ERROR_MESSAGE);
        free_command(tmp1);
        return;
    }

    //Count how may parallel commands exist.
    count_parallel=0;
    while(tmp1[count_parallel]!=NULL){count_parallel++;}

    //ARRAY OF PIDS.
    pid_childs = (pid_t*)malloc(count_parallel*sizeof(pid_t));
        
    //LAUNCH KIDS AND RUN COMMANDS
    for(int i=0;i<count_parallel;i++){
        pid_childs[i]=fork();

        if(pid_childs[i]<0){//ERROR
                CHECK_POINTER(NULL);
        }
        else if(pid_childs[i]==0){ //CHILD CODE.

            run_line_command_red(tmp1[i]);
            exit(0);
        }
        //PARENT COUNTINES.
    }   

    for (int i = 0; i < count_parallel; i++){
            
        waitpid(pid_childs[i],&wstatus,0);
    }
    free(pid_childs);
    free_command(tmp1);
    return;
}

/*Function: Try to run the given input.*/
void run_line(char *input){

    char **tmp1;
    char **command;
    int size_args;


    tmp1 = split_string(input,LJUM_DELIM);//Take out the jump char.

    if(bool_all_space(tmp1[0])){//Check if the given input is only spaces.
        return;
    }

    //CHECK EXIT AND CD COMMAND;//////////////////////////////////////////////////
    command = split_string(tmp1[0],SPACE_DELIM);

    if(check_exit(command[0])){              //Check if the user typed exit.
        if(command[1]!=NULL){
            fprintf(stderr,"%s",ERROR_MESSAGE);
            free_command(tmp1);
            free_command(command);
            free(input);
            return;
        }
        exit_command();
    }

    size_args=0;
    while(command[size_args]!=NULL){size_args++;} //Count the number of args.
    size_args--; //Because of the NULL end.
    if(check_cd(command[0])){                  //Check if the command is cd and check its cases.
            if((size_args!=1)||(chdir(command[1])!=0)){
            fprintf(stderr,"%s",ERROR_MESSAGE);
            free_command(tmp1);
            free_command(command);
            return;
        }
        free_command(tmp1);
        free_command(command);
        return; //The command has ben executed. 
    }
    free_command(command);
    ///////////////////////////////////////////////////////////////////

    run_line_command_parll(tmp1[0]); //RUN THE COMMAND WITH PARALLEL AND REDIRECTIONS.
    free_command(tmp1);
}