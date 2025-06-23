#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>


int main( int argc, char *argv[]) {

	struct stat st, st2;
	int size, size2;

	stat(argv[1], &st);
	stat(argv[2], &st2);
	size = st.st_size;
	size2 = st2.st_size;
	
	if(size != size2) {
	
	printf("%s %s differ\n", argv[1], argv[2]);
	return 0;
	
	}

	int pid1 = fork();
	
	if(pid1 == 0) {
	
		char buffer1a[1]; 
		char buffer2a[1];
		FILE* file1a;
		FILE* file2a;
		file1a = fopen(argv[1], "r");
		file2a = fopen(argv[2], "r");
		
		for(int i = 0; i < size / 2; i++) {
		
			fread(&buffer1a,sizeof(buffer1a),1,file1a);
			fread(&buffer2a,sizeof(buffer2a),1,file2a);
			
			if(buffer1a[0] != buffer2a[0]) {
			
			fclose(file1a);
	 		fclose(file2a);
			
			
			return 1;
			}
	 	}
	 	
	 	fclose(file1a);
	 	fclose(file2a);
	        
		return 0;
	
	}
	
	int pid2 = fork();
	
	if(pid2 == 0) {
	
		char buffer1a[1]; 
		char buffer2a[1];
		
		FILE* file1a;
		FILE* file2a;
		file1a = fopen(argv[1], "r");
		file2a = fopen(argv[2], "r");
		
		fseek(file1a, (size / 2), SEEK_SET);
		fseek(file2a, (size / 2), SEEK_SET);
		
		for(int i = (size / 2) ; i < size; i++) {
		
			fread(&buffer1a,sizeof(buffer1a),1,file1a);
			fread(&buffer2a,sizeof(buffer2a),1,file2a);
			
			if(buffer1a[0] != buffer2a[0]) {
			
			fclose(file1a);
	 		fclose(file2a);
			
			
			return 1;
			}
	 	}
	 	
	 	fclose(file1a);
	 	fclose(file2a);
		return 0;
	
	}	
	
	int status1, status2;
	
	waitpid( pid1, &status1,0);
	waitpid( pid2, &status2,0);
	
	if( status1 || status2) {
	
		printf("%s %s differ\n", argv[1], argv[2]);
	
	}
	
	return 0;

}
