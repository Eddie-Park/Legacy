#include <ansi_c.h>

#define MB        1024*1024
#define DATA_SIZE 10*MB
int main (int argc, char *argv[])
{
   
	unsigned char* data = malloc(DATA_SIZE);
	FILE* dll = fopen("C:\\Users\\rtaschereau\\Documents\\dllZap\\FlyCapture2.dll","rb");
	
	size_t actualSize = fread(data, 1, DATA_SIZE, dll);
	fclose(dll);
	
	for (int k=0; k<actualSize; k++){
		
		 if ( 
			 data[k]   =='l' &&
			 data[k+1] =='i' &&
			 data[k+2] =='b' &&
			 data[k+3] =='i' &&
			 data[k+4] =='o' &&
			 data[k+5] =='m' &&
			 data[k+6] =='p' &&
			 data[k+7] =='5' &&
			 data[k+8] =='m' &&
			 data[k+9] =='d' &&
			 data[k+10]=='.' &&
			 data[k+11]=='d' &&
			 data[k+12]=='l' &&
			 data[k+13]=='l'
			 ){
			 
			 printf("found at offset %d, actual size is %d\n", k, actualSize);
			 
			 data[k]  ='f';
			 data[k+1]='l';
			 data[k+2]='y';
			 
			 break;
		 }
		
		
	}
	
	dll = fopen("C:\\Users\\rtaschereau\\Documents\\dllZap\\FlyCapture2.dllx","wb");
	fwrite(data, 1, actualSize, dll);
	fclose(dll);
	
    return 0;
}

