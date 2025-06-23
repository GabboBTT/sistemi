 #include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <string.h>
#include <utime.h>


void stampat (char *ndir) {
	printf("%s\n", ndir);
	DIR *d;
	struct dirent *dir;
	char *ptndir = ndir;
	d = opendir(ptndir);

	if (d) {
	
		while ((dir = readdir(d)) != NULL) {
			printf("%d",d);
			
			if (!strcmp(dir -> d_name, "ager.c")) {
			
		       struct stat buf;
		       struct utimbuf time;
		       time_t cambio = 864000;
			stat(dir -> d_name, &buf);
			time.actime = buf.st_atim.tv_sec - cambio;
			time.modtime = buf.st_mtim.tv_sec - cambio;
			printf("%o\n", buf.st_atim);
			int t = utime(dir -> d_name, &time);
			}
		}
		
		closedir(d);

 	}
}

void stampa (char *ndir) {
	
	struct stat buf;
	
		       struct utimbuf time;
		       time_t cambio = 864000;
			stat(ndir, &buf);
			time.actime = buf.st_atim.tv_sec - cambio;
			time.modtime = buf.st_mtim.tv_sec - cambio;
			printf("%o\n", buf.st_atim);
			int t = utime(ndir, &time);

}


int main(int argc, char* argv[]) {

	if (argc == 1) {
		
	stampat(".");
	
	} else if (argc == 2) {

	stampa(argv[1]);

	}

return 0;

}


