#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


//*********Function Declarations *******************
char* makeDirectory();
char ** makeRoomFiles(char*);
struct Room GetRandomRoom(char*);
int CanAddConnectionFrom(struct Room);
int IsSameRoom(struct Room, struct Room);
int ConnectionAlreadyExists(struct Room, struct Room);
void ConnectRoom(struct Room, struct Room);
void AddRandomConnection(char*);
int IsGraphFull(char**);
//***************************************************



struct Room{
   char name[30];
   char type[12];
   int numConnections;
   char * connection[6];
   char path[30];
};


int main()
{
   
   int i;
   srand(time(0));
   char* filePath; 
   int * connectionArr[7];  
   char** allPaths; 
   filePath = makeDirectory();
   allPaths = makeRoomFiles(filePath);  //stores all the paths to the room files 

 while (IsGraphFull(allPaths) != 1)
  {
     AddRandomConnection(filePath);
     

  }
  


    free (filePath);
     for(i = 0; i <7; i++)
    {
       free (allPaths[i]);
    }

    free(allPaths);     
   return 0;
}


char*  makeDirectory ()
{
   char dirName[13] = "arrasm.rooms";
   char* roomsDir;
   roomsDir = malloc(sizeof(char)*50);

   //initialize arrays with all \0's
   memset(roomsDir, '\0', 50);

   //variable for process ID 
   pid_t process_id;

   //get the process id
   process_id = getpid();

   //create the directory name using current process ID  
   sprintf(roomsDir, "%s%d", dirName,process_id);

   //make the directory and check if successful 
   int result;
   result =  mkdir(roomsDir, 0755);
   return roomsDir;
}


char ** makeRoomFiles(char* path)
{
   int numRoomNames = 10;
   int numRooms = 7;
   int numRoomTypes = 3;
   int i;
   char ** fullPath = ( char * * ) malloc ( sizeof ( char * ) * 7 ); 
   for(i = 0; i < 7; i++)
   {
      fullPath[i] = (char *)malloc(sizeof(char) * 50);
   }
  
   int file_descriptor;
   ssize_t nread, nwritten;
   char readBuffer[32];
   const char* endline = "\n";

   //Hard-coded room names, stored in an array.
   const char* roomNames[numRoomNames];
   roomNames[0] = "White_Room";
   roomNames[1] = "Gold_Room";
   roomNames[2] = "Red_Room";
   roomNames[3] = "Pink_Room";
   roomNames[4] = "Lavender_Room";
   roomNames[5] = "Black_Room";
   roomNames[6] = "Magenta_Room";
   roomNames[7] = "Mauve_Room";
   roomNames[8] = "Orange_Room";
   roomNames[9] = "Violet_Room";

   //Strings that hold text to be written to room files
   char rmName[] = "ROOM NAME:";
   char rmType[] = "ROOM TYPE:";
   char roomTypes[3][12];
   strcpy (roomTypes[0], "START_ROOM");
   strcpy (roomTypes[1], "MID_ROOM");
   strcpy (roomTypes[2], "END_ROOM");
  

   char* fileNameText = "room";
   int upper = 9;
   int lower = 0;
   int num = 0;
   int counter = 0;
   int numStart = 0;
   int numEnd = 0;

   //this array keeps track of which room names have been used.
   //For example, if the random number generator uses a 9, then
   //check if the corresponding index in numArray has a 9.  If it
   //has a -1, then that name is taken.  Otherwise, use the name,
   //and set the number in the correct index to -1.

   int numArray[10];
   //populate the array with numbers 1-10
   for (i = 0; i < 10; i++)
   {
      numArray[i] = i;
   }

   //populate the first index with a -1 since
   //there's no checking for duplicates needed
   num = (rand() % (upper - lower + 1)) + lower;
   numArray[num] = -1;

   //make a random number, and if it hasn't been picked
   //already, assign its index a -1;
   //otherwise, don't increment and try again until each
   //of the seven rooms have a unique room name.
   for(i=1; i < numRooms; i++)
   {
      num = (rand() % (upper - lower + 1)) + lower;

      if(numArray[num] != -1)
      {
         numArray[num] = -1;
      }
      else
      {
         i = i-1;
      }
   }

   for(i = 0; i<numRoomNames; i++)
   {
      if(numArray[i] == -1)
      {
          //create string for path to room file
          sprintf(fullPath[counter], "./%s/%s_%d", path, fileNameText, counter);
          file_descriptor = open(fullPath[counter], O_RDWR | O_CREAT | O_TRUNC, 0600);
          counter++;
          nwritten = write(file_descriptor, rmName, strlen(rmName) * sizeof(char));
          nwritten = write(file_descriptor, roomNames[i], strlen(roomNames[i]) * sizeof(char));
          nwritten = write(file_descriptor, endline, strlen(endline) * sizeof(char));
          //close(file_descriptor); 

          //Specify room type:  START_ROOM, MID_ROOM, END_ROOM
          num = (rand() % (3 - 1 + 1)) + 1;

         if(num == 1 && numStart == 0)
         {
            numStart = numStart + 1;
            nwritten = write(file_descriptor, rmType, strlen(rmType) * sizeof(char));
            nwritten = write(file_descriptor, roomTypes[0], strlen(roomTypes[0]) * sizeof(char));
            
            
         }
         else if (num == 3 && numEnd == 0)
         {
            numEnd = numEnd + 1;
            nwritten = write(file_descriptor, rmType, strlen(rmType) * sizeof(char));
            nwritten = write(file_descriptor, roomTypes[2], strlen(roomTypes[2]) * sizeof(char));
            
            
         }
         else
         {
            nwritten = write(file_descriptor, rmType, strlen(rmType) * sizeof(char));
            nwritten = write(file_descriptor, roomTypes[1], strlen(roomTypes[1]) * sizeof(char));
           
            
         }
      }
   }
   return fullPath;
}

struct Room GetRandomRoom(char* filePath)
{

   struct Room A;  //used to hold input from file stream

   int upper = 6;
   int lower = 0;
   int num = 0;
   int file_descriptor;
   char* roomText = "room_";  //used for the text of the room files
   char buffer[30];
   memset(buffer, '\0', 30);
   char fullPath[50];
   char readConnection[24];
   memset(readConnection, '\0', 24);
   ssize_t nread, nwritten;
   memset(fullPath, '\0', 50);
   int counter = 0;
   int numConnections;
   int i;
   char tempBuff[20];

   //This pseudo-random number designates which of the 7 files to open
   num = (rand() % (upper - lower + 1)) + lower;

   //Create the path string to open the correct random file 
   sprintf(fullPath, "./%s/%s%d", filePath, roomText, num);
   

   //Open the file for reading
   FILE * fileStream;
   fileStream = fopen(fullPath, "a+");
   fseek(fileStream, 10, SEEK_SET );
   fgets(A.name, 30, (FILE*)fileStream);
   printf("%s", A.name);


   //This loop cycles through file, starting right after the name field.
   //The remaining lines are counted, which gives the number of connections
   while(!feof(fileStream))
   {
     fgets(buffer, 30, (FILE*)fileStream);
     counter++;
   }
   //Store the number of connections in the room struct.
   A.numConnections = counter - 1;
   

   fclose(fileStream);
   //Allocate space for up to 6 connections for the room
   for(i = 0; i < 6; i++)
   {
     A.connection[i] = (char*) malloc(9);
   }

   fileStream = fopen(fullPath, "r");
   fgets(buffer, 30, (FILE*)fileStream);

   //Write the connection names into the struct if they exist 
   for(i = 0; i <A.numConnections; i++)
   {
      fseek(fileStream, 13, SEEK_CUR);
      fgets(tempBuff, 20, (FILE*)fileStream);
      A.connection[i] = strcpy(A.connection[i], tempBuff);
   }

   //Get the room type and store in the room struct   
   fseek(fileStream, -10, SEEK_END);
   fgets(A.type, 12, (FILE*)fileStream);
   strcat(A.type, "\n");
   fclose(fileStream);

   //put the file path in the room struct
    sprintf(A.path, "%s", fullPath);
   return A;
}

int CanAddConnectionFrom(struct Room inputRoom)
{
   if(inputRoom.numConnections >= 6)
   {
     return 0;
   }
 
      return 1;
}

int IsSameRoom(struct Room A, struct Room B)
{
   //compare the names of each room

   if(strcmp(A.name, B.name) == 0)
   {
    return 0;
   }
   else{
     return -1;
    }
}



int ConnectionAlreadyExists(struct Room A, struct Room B)
{
   int i;
   for(i = 0; i < 6; i++)
   {
      if(strcmp(A.name, B.connection[i]) == 0)
      {
         return 0;
      }
    
    }
   return -1;
}

void ConnectRoom(struct Room A, struct Room B)
{
   char buffer[30];
   char tempBuff[20];
   char tempBuff2[20];

   sprintf(tempBuff, "%s", B.name);
   A.connection[A.numConnections] = strcpy(A.connection[A.numConnections], tempBuff);
   sprintf(tempBuff2, "%s", A.name);
   B.connection[B.numConnections] = strcpy(B.connection[B.numConnections], tempBuff2);
   FILE* writeStream;
   FILE* readStream;
   writeStream = fopen("./tempFile", "a");
   readStream = fopen(A.path, "r");
   int numConnectionsInc = A.numConnections + 1;

   //Print the content of Room A into the temp file
   //up to the point of insertion of the new connection
   fgets(buffer, 30, (FILE*)readStream);
   fprintf(writeStream, "%s", buffer);

   //Insert/print the new connection to the temp file with proper formatting
   fprintf(writeStream, "CONNECTION ");
   fprintf(writeStream,  "%d", numConnectionsInc);
   fprintf(writeStream,  ":");
   fprintf(writeStream, "%s", A.connection[A.numConnections]);

   while(!feof(readStream))
   {
      fgets(buffer, 30, (FILE*)readStream);
      fprintf(writeStream, "%s", buffer);
      
   }

   fclose(readStream);
   fclose(writeStream);
   readStream = fopen(A.path, "w+");
   writeStream = fopen("./tempFile", "r");
   
   //Write contents of temp file to the current room file
  while(!feof(writeStream))
   {
       fgets(buffer, 30, (FILE*)writeStream);
       fprintf(readStream, "%s", buffer);
   }
   fclose(readStream);
   fclose(writeStream);
   remove("./tempFile");


}


void AddRandomConnection(char * filePath)
{
   int i;

   struct Room A;
   struct Room B;


   while(1)
   {
      A = GetRandomRoom(filePath);
      if (CanAddConnectionFrom(A) == 1)
      {
         break;
      }
      
   }

   do
  {
    B = GetRandomRoom(filePath);
  }
  while((CanAddConnectionFrom(B) == 0)  || (IsSameRoom (A , B) == 0 ) || (ConnectionAlreadyExists(A, B) == 0));
   
   ConnectRoom(A, B);
   ConnectRoom(B, A);
   
   for(i = 0; i<6; i++)
   {

      free(A.connection[i]);
      free(B.connection[i]);
   }
   
   
}

int IsGraphFull(char** allPaths)
{
  int i;
  int counter = 0;
  int numConnectionsArr[7];
  int tally = 0;
  char buffer[30];
  memset(buffer, '\0', 30);

     FILE * fileStream;

     for(i = 0; i <7; i++)
     {
         fileStream = fopen(allPaths[i], "r");
         while (!feof(fileStream))
         {
            fgets(buffer, 30, fileStream);
            counter++;
            
         }

         numConnectionsArr[i] = counter -2;
         counter = 0; 
         
         fclose(fileStream);
     }
    

    
     for(i = 0; i < 7; i++)
     {

        if (numConnectionsArr[i] >=3 || numConnectionsArr[i] == 6)
        {
           numConnectionsArr[i] = -1;
        }

     }

     if( numConnectionsArr[0] == -1 && 
         numConnectionsArr[1] == -1 &&
         numConnectionsArr[2] == -1 &&
         numConnectionsArr[3] == -1 &&
         numConnectionsArr[4] == -1 &&
         numConnectionsArr[5] == -1 &&
         numConnectionsArr[6] == -1)
      {
         return 1;

      }
                                       
   return 0;
}







