
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>


//***********Function Declarations**************
char * getDir();
void Game();
//*********************************************



//*********Room Struct********************

struct Room
{
    char *name;  
    char ** connections;  //6 rooms, 9 chars long
    char *roomType;
    int numConnections;
};


int main() {

    Game();
    //printf("%s", getDir());
  return 0;
}



char* getDir() 
{

   int counter = 0;
   struct stat holder;
  

  //the dirent struct has a char d_name member
  struct dirent *dirCheck;
  struct dirent *newestDirectory;

  //The DIR file type is a data stream type, of sorts
  DIR *dp;

  //opendir returns a pointer to a DIR, opening a stream
  //to the path specified in the arguments
  dp = opendir(".");


  //read the directory until the end, 
  int i;
  time_t lastMod;
  time_t mostRecent;
  

  while((dirCheck = readdir(dp)) !=NULL)
  {
      //Get stat info on the name of the file in d_name, put it
      //in the "holder" struct
       stat(dirCheck->d_name, &holder);
      
      //if the current file is a directory, get 
      if(S_ISDIR(holder.st_mode) && (strcmp(dirCheck->d_name, ".") != 0) && (strcmp(dirCheck->d_name, "..") != 0))
      {
        //this is the last modified time of the directory being looked at.
        lastMod = holder.st_mtime;
        
        if(counter == 0)
        {
            mostRecent = lastMod;
            newestDirectory = dirCheck;
        }

        else if(mostRecent < lastMod)
        {
            newestDirectory = dirCheck;
            mostRecent = lastMod;
        }
      }

      counter++;
      
  }

  closedir(dp);

  return newestDirectory->d_name;
}



void Game()
{
   int moves;  //counter variable for the number of moves taken
   char * gameDirPath;  
   gameDirPath = getDir();  
   int i;
   int j = 0;
   

  char newPath[20];  

  struct Room curLoc;

  //*****Allocate space for curLoc (current room location)*********

      //space for connections
  curLoc.connections = malloc(6* sizeof(char*));
  for(i = 0; i < 6; i++)
  {
      curLoc.connections[i] = malloc(30* sizeof(char));

  }
  curLoc.name = malloc(9* sizeof(char));
  curLoc.roomType = malloc(11* sizeof(char));


  //***********************************************************

  //******Allocate space for allRooms ***************
  struct Room allRooms[7];

  //Allocate for connections

  for(i = 0; i < 7; i++)
  {
     allRooms[i].connections = malloc(6* sizeof(char*));
     for (j = 0; j < 6; j++)
     {
         allRooms[i].connections[j] = malloc(30* sizeof(char));
     }
  }
  
 //Allocate for name and type

 for(i = 0; i < 7; i++)
 {
     allRooms[i].name = malloc(9* sizeof(char));
     allRooms[i].roomType = malloc(11* sizeof(char));

 }



   strcpy(newPath, gameDirPath);
   int check = 0;
 
   //This struct allows access to the directory name  
   struct dirent *dirCheck;

   int numPaths = 9;
   int arrLen = 30;

   //This array of strings holds all the paths
   char** filePathArr;
   
   filePathArr = malloc(9*sizeof(char*));
   for(i = 0; i < 9; i++)
   {
       filePathArr[i] = malloc(30*sizeof(char));
   }
  
   FILE * fileStream;
   char * fullPath;
   char buffer [30];
   

   //counter for number of connections
   int counter = 0;


   
   //open the game directory with the room files.
   DIR * dirStream = opendir(newPath); 
   strcat(newPath, "/");
  


   //This loop creates all the strings that form the file paths


   for (i = -2; i < 7; i++)
   {
      
          dirCheck = readdir(dirStream);
          
          strcpy(buffer, dirCheck->d_name);
          if ( (strcmp(buffer, "..") != 0) &&  (strcmp(buffer, ".") != 0) )
          {
              strcpy(filePathArr[i], "./");
              strcat(filePathArr[i], newPath);
              strcat(filePathArr[i], buffer);
          }   
   }

   
    // Get room info stored into room structs 

  
   for (i = 0; i<7; i++)
   {
      fileStream = fopen(filePathArr[i], "r");
      if (!fileStream)
      {
        printf("File open failed");
        exit(0);
      }
      
      //skip past the text, "ROOM NAME"
      fseek(fileStream, 10, SEEK_SET);
      fgets(allRooms[i].name, 20, fileStream);
      fclose(fileStream);
      }
   
  
    //Establish number of connections by counting
    //number of lines in the file and subtracting for the name field
    //and the type field.  Add the number of conneciton to the correct
    //room in the array of room structs.
    for(i = 0; i < 7; i ++)
    {
        fileStream = fopen(filePathArr[i], "r");

        while(!feof(fileStream))
        {
            fgets(buffer, 30, fileStream);
            counter++;
        }

       
        allRooms[i].numConnections = counter -2;
        counter = 0;
        
        fclose(fileStream);
    }



    for (i = 0; i < 7; i++)
    {
        //open the file, and read in the name field.  This 
        //puts the current location in the file right at the 
        //start of the connection fields.
       
        fileStream = fopen(filePathArr[i], "r");
       
        fgets(buffer, 30, fileStream);
        
       
        //this section populates the connection fields in the room struct
        
        

        for(j = 0; j < allRooms[i].numConnections; j++)
        {

            fseek(fileStream, 13, SEEK_CUR);
            fgets(allRooms[i].connections[j], 30, fileStream);
        }
         fclose(fileStream);


       
    }

    //Copy all the room types to the correct room structs.
    for(i = 0; i < 7; i ++)
    {
        fileStream = fopen(filePathArr[i], "r");
        if (!fileStream)
        {
            exit(1);
        }
        fseek(fileStream, -10, SEEK_END);
        fgets(allRooms[i].roomType, 13, fileStream);
        fclose(fileStream);
        
    }

//  This section establishes a start room, by finding the room with 
//  the room type of "START_ROOM", by doing a string comparison
   for( i = 0; i < 7; i++ )
   {
       if(strcmp(allRooms[i].roomType, "START_ROOM") == 0)
       {
           strcpy(curLoc.name, allRooms[i].name);
           curLoc.numConnections = allRooms[i].numConnections;

           for (j = 0; j < allRooms[i].numConnections; j++)
           {
              strcpy(curLoc.connections[j], allRooms[i].connections[j]); 
             
           }

           
       }
   }

    while (check == 0)
    {
       printf("CURRENT LOCATION:  %s", curLoc.name);
       printf("POSSIBLE CONNECTIONS:  ");

        for(i = 0; i < curLoc.numConnections; i++)
        {
            if(i == (curLoc.numConnections -1))
            {
            strtok( curLoc.connections[i], "\n");   //strtok to remove endline
            printf("%s.", curLoc.connections[i]);
            strcat(curLoc.connections[i], "\n");
            }

            else
            {
                strtok( curLoc.connections[i], "\n");  //strtok to remove endline
                printf("%s, ", curLoc.connections[i]);
                strcat(curLoc.connections[i], "\n");
            }
            
        }


        char *test;
        test = malloc(32 * sizeof(char));
        size_t bufsize = 32;
        size_t characters;
        int k; 

        test = (char *)malloc(bufsize * sizeof(char));

        printf("\nWHERE TO? >");

        //Get the user input, input is stored in "test"
        characters = getline(&test, &bufsize, stdin);

        for(i = 0; i < curLoc.numConnections; i++)
        {
            if (strcmp(test, curLoc.connections[i]) == 0)
            {  
                check = 1;
                moves++;

                //Copy all the data from the chosen room to the new
                //room, forming a new current location
                strcpy(curLoc.name, curLoc.connections[i]);

                for (j = 0; j < 7; j++)
                {
                    if(strcmp(curLoc.name, allRooms[j].name) == 0)
                    {
                        curLoc.numConnections = allRooms[j].numConnections;
                        strcpy(curLoc.roomType, allRooms[j].roomType);
                        for (k = 0; k < curLoc.numConnections; k++)
                        {
                            strcpy(curLoc.connections[k], allRooms[j].connections[k]);
                        }
                    }
                }
            

            }
            
            
            
        }
        if(strcmp(curLoc.roomType, "E:END_ROOM") ==  0)
        {
            printf("you won in %d moves", moves);
            check = 1;

        }

        else
        {
            check = 0;
        }


    }
      
}