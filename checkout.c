#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <time.h>
#include <sys/stat.h>
int check_empty_folder()
{
  FILE *fp;
  char file_name[16];
  int c = 0;
  system("ls -lsa rcs | awk '{if(NR>3){ print  $10} }' > temp1.txt");
  fp = fopen("temp1.txt","r");

  while(!feof(fp))
  {
    fscanf(fp, "%s", file_name);
    c++;
  }
  fclose(fp);
  return c;
}
int isoption(char *argv[])
{
  char version[50];
  strcpy(version,argv[1]);
  printf("version enabled ->%s\n", version);
  if(version[0]=='-' && version[1]=='v')
  {
    printf("version is detected\n" );
    return 1;
  }
  printf("no version is detected\n" );
  return 0;
}
void change_file_name(char *str)
{
  int i,len;
  len=strlen(str);
  for(i=0;i<len;i++)
  {
    if(str[i+1]=='/')
      str[i+1]='-';
    str[i]=str[i+1];
  }

}

int update_log(char *input_file)
{
  FILE *fp;
  int flag = 0 , ct;
  struct stat buf_i;
  char t[20] , file_name[16] , t_l[20];
  stat(input_file, &buf_i);
  time_t now = buf_i.st_mtime;
  strftime(t, 20, "%Y-%m-%d|%H:%M:%S", localtime(&now));
  ct = check_empty_folder();
  if(ct == 1)
    fp = fopen("rcs/log.rcs","w+");
  else
    fp = fopen("rcs/log.rcs","r+");
  while(!feof(fp))  // removing previous version
  {
    fscanf(fp, "%s", file_name);
    //printf("%s \n", file_name);
    if(strcmp(file_name , input_file) == 0)
    {
      fprintf(fp, " %s", t);
      flag = 1;
      break;
    }
    fscanf(fp, "%s", t_l);
    //printf("%s \n", t_l);
  }
  if(flag == 0)
  {
    fprintf(fp, "%s ", input_file);
    fprintf(fp, "%s\n", t);
  }
  fclose(fp);
}
int copy_file(char * src,char *dest)
{
  FILE *fp1, *fp2;
  char ch;
  int pos;
  if ((fp1 = fopen(src,"r")) == NULL)
  {
      printf("\nFile cannot be opened");
      return 0;
  }
  else
  {
      printf("\nFile opened for copy...\n ");
  }

  fp2 = fopen(dest, "w");
  fseek(fp1, 0L, SEEK_END); // file pointer at end of file
  pos = ftell(fp1);
  fseek(fp1, 0L, SEEK_SET); // file pointer set at start
  while (pos--)
  {
      ch = fgetc(fp1);  // copying file character by character
      fputc(ch, fp2);
  }

  fclose(fp1);
  fclose(fp2);
  printf("finished copying\n" );
  return 0;
}

int main(int count,  char *argv[])
{
  register struct passwd *pw;
  register uid_t uid;
  int version_name;
  char *file_name;
  char *path_with_file,str[30];
  char username[20];
  char cwd[1024];
  char buffer[1024];//used for holding long file name
  char command[200];
  char input_file_name[100];//for using in command
  FILE *fp=NULL,*dest;
  if(count ==1 )
  {
    printf("error\n");
    return 0;
  }
  if (getcwd(cwd, sizeof(cwd)) != NULL)
       fprintf(stdout, "Current working dir: %s\n", cwd);
   else
       perror("getcwd() error");

  path_with_file=(char*)malloc(200*sizeof(char));
  //command=(char*)malloc(200*sizeof(char));
  // check if file is locked or not
  // if(is_file_locked())
  //     printf('file is locked')
  //    return 0;
  if(isoption(argv)==1)
  {
    printf("option is enabled") ;
    file_name=(char*)malloc(50*sizeof(char));
    version_name=atoi(argv[3]);
    printf("version number =%d file name= %s\n",version_name,argv[2] );
    strcpy(file_name,argv[2]);
    printf("\n ---actual file name-> %s\n", file_name);

    strcpy(buffer,file_name);
    strcat(buffer,argv[3]);
    strcat(buffer,".rcs");
    /**********************creating path to RCS folder***************/
    uid = geteuid();
    pw = getpwuid(uid);

    strcpy(path_with_file,cwd);
    strcat(path_with_file,"/rcs/");
    strcat(path_with_file,buffer);
    printf("file-> %s \n path-->%s\n",buffer, path_with_file );
    if( access( path_with_file, F_OK ) != -1 ) {
      printf("--------------------------file is there getting it-------------------------------\n" );
    } else {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!no such file!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );
      return 0;
    }
    /////////////////////////////////////////////////////////////////////////
    //------------------ destination file name-------------------------------
    printf("--------actual file name in the beginning--> %s\n",file_name );
    strcpy(input_file_name,cwd);
    strcat(input_file_name,"/");
    strcat(input_file_name,argv[2]);
    printf("\ninput file-->%s \n cwd--> %s\n",input_file_name,cwd );
    strcpy(command,"cp ");
    strcat(command,path_with_file);
    strcat(command," ");
    strcat(command,input_file_name);
    printf("\ncommand--> %s --- %ld\n",command,sizeof(command) );
  //  system(command);
    copy_file(path_with_file,input_file_name);
    update_log(argv[2]);//need to chang for .exe file

  }
  else
  {
    printf("****option is not enabled\n");
    /****************************for rcs in a folder*******************/

    strcpy(path_with_file,cwd);
    strcat(path_with_file,"/rcs/");
    ///////////////////////////////////////////////////////////////////////
    strcat(path_with_file,argv[1]);
    printf("path to file -->%s\n", path_with_file);
    /*************************** if there is a file *****************************************/
    if( access( path_with_file, F_OK ) != -1 ) {
      printf("--------------------------file is there getting it-------------------------------\n" );
    } else {
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!no such file!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );
      return 0;
    }

    strcpy(input_file_name,cwd);
    strcat(input_file_name,"/");
    strcat(input_file_name,argv[1]);

    update_log(argv[1]);
    copy_file(path_with_file,input_file_name);
  }
  return 0;
}
