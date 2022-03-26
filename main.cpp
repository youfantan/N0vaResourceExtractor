#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <io.h>
#ifdef WIN32
#include <windows.h>
#include <tchar.h>
#endif
#ifdef __linux__
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#endif
void replace(const char* filePath);
void traverse(std::vector<std::string> &vct, char* path);
BOOL GetPrivileges();
int endWith(std::string s,std::string sub){
    return s.rfind(sub)==(s.length()-sub.length())?1:0;
}
int main(int argc,char **argv) {
#ifdef WIN32
    GetPrivileges();
#endif
    if (argc==2){
        char *filePath=argv[1];
        replace(filePath);
        return 0;
    } else{
        char filePath[1024];
        getcwd(filePath,1024);
        std::vector<std::string> dirs;
        traverse(dirs,filePath);
        for (auto it = dirs.begin(); it!=dirs.end() ; ++it) {
            if (endWith(*it,".ndf")){
                printf("Processing: %s\n",it->c_str());
                replace(it->c_str());
            }
        }
        return 0;
    }
}
#ifdef WIN32
BOOL GetPrivileges()
{
    HANDLE hToken;
    LUID sedebugnameValue;
    TOKEN_PRIVILEGES tkp;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        return   FALSE;
    }
    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
    {
        CloseHandle(hToken);
        return false;
    }
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = sedebugnameValue;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
    {
        CloseHandle(hToken);
        return false;
    }
    return true;
}
#endif
void replace(const char* filePath){
    std::ifstream fs(filePath,std::ios::in|std::ios::binary);
    //get content length
    fs.seekg(0,std::ios::end);
    size_t size=fs.tellg();
    size-=2;
    fs.seekg(2,std::ios::beg);
    char *buffer=(char*) malloc(sizeof(char)*size+1);
    fs.read(buffer,size);
    char path[1024]={0};
    strcat(path,filePath);
    strcat(path,"_changed.mp4");
    std::ofstream os(path,std::ios::out|std::ios::binary);
    os.write(buffer,size);
    fs.close();
    os.close();
}
void traverse(std::vector<std::string> &vct, char* path){
#ifdef WIN32
    long handle;
    _finddata_t fi;
    char _path[1024]={0};
    memcpy(_path,path, strlen(path));
    strcat(_path,"\\*");
    int ret=handle=_findfirst(_path,&fi);
    if (handle==-1) {
        MessageBox(nullptr, _T("No files exists in the dirctory"), _T("Error"), MB_OK);
        printf("Error: No files exists in the directory %s", path);
    }
    while (ret!=-1){
        if (strcmp(fi.name,".") != 0&& strcmp(fi.name,"..") !=0){
            std::string s(path);
            s.append("\\");
            s.append(fi.name);
            if ((fi.attrib&_A_NORMAL)==0){
                vct.push_back(s);
            }
        }
        ret=_findnext(handle,&fi);
    }
    _findclose(handle);
#endif
#ifdef __linux__
	struct stat s;
	lstat(path, &s);
	if(! S_ISDIR(s.st_mode))
	{
        printf("Error: No files exists in the directory %s", path);
		return;
	}
	struct dirent *fi;
 	DIR *dir;
	dir = opendir(path);
	while((fi = readdir(dir) ) != NULL )
	{
		if(strcmp(fi->d_name , "." ) == 0 ||
           strcmp(fi->d_name , "..") == 0)
			continue;
        if (fi->d_type == DT_REG){
            std::string s(path);
            s.append("/");
            s.append(fi->d_name);
            vct.push_back(s);
        }
	}

#endif
}