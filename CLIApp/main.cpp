#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <direct.h>
#include <conio.h>
#include "def.h"
void replace(std::string filePath,int fileType);
void traverse(std::vector<std::string> &vct, char* path);
int endsWith(const std::string&,const std::string& sub);
int getFileType(const char* filePath);
boolean compareFromHead(unsigned char* toBeCompared,size_t nToBeCompared,const unsigned char* pattern,size_t nPattern);
BOOL GetPrivileges();
const unsigned char EXIF_IMAGE_MAGIC[4]={0xff,0xd8,0xff,0xe1};
const unsigned char PNG_IMAGE_MGIC[8]={0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a};
const unsigned char JPEG_IMAGE_MAGIC[4]={0xff,0xd8,0xff,0xe0};
const unsigned char FTYPISOM_VIDEO_MAGIC[10]={0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x66, 0x74, 0x79, 0x70};
const unsigned char FTYPMP42_VIDEO_MAGIC[10]={0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x66, 0x74, 0x79, 0x70};
int main(int argc,char **argv) {
    GetPrivileges();
    if (argc==2){
        std::string filePath=argv[1];
        replace(filePath, getFileType(filePath.c_str()));
    } else{
        char filePath[1024];
        _getcwd(filePath,1024);
        std::vector<std::string> dirs;
        traverse(dirs,filePath);
        for (auto it = dirs.begin(); it!=dirs.end() ; ++it) {
            if (endsWith(*it,".ndf")){
                replace(*it, getFileType(it->c_str()));
            }
        }
    }
    printf("Press any key to continue");
    _getch();
    return 0;
}
boolean compareFromHead(unsigned char* toBeCompared,size_t nToBeCompared,const unsigned char* pattern,size_t nPattern){
    for (int i = 0; i < nPattern; ++i) {
        if (*toBeCompared!=*pattern){
            return false;
        }
        *toBeCompared++;
        *pattern++;
    }
    return true;
}
int getFileType(const char* filePath){
    FILE* file= fopen(filePath,"rb+");
    //get file first 10 bytes
    char* magic= new char[10];
    fread(magic,1,10,file);
    if (compareFromHead(reinterpret_cast<unsigned char *>(magic), 10, EXIF_IMAGE_MAGIC, 4)){
        delete[] magic;
        return EXIF_FILE;
    } else if (compareFromHead(reinterpret_cast<unsigned char *>(magic), 10, PNG_IMAGE_MGIC, 8)) {
        delete[] magic;
        return PNG_FILE;
    } else if (compareFromHead(reinterpret_cast<unsigned char *>(magic), 10, JPEG_IMAGE_MAGIC, 4)){
        delete[] magic;
        return JPEG_FILE;
    } else if (compareFromHead(reinterpret_cast<unsigned char *>(magic), 10, FTYPMP42_VIDEO_MAGIC, 10)||compareFromHead(reinterpret_cast<unsigned char *>(magic), 10, FTYPISOM_VIDEO_MAGIC, 10)){
        delete[] magic;
        return FTYP_VIDEO_FILE;
    } else{
        printf("%x\n",magic);
        delete[] magic;
        return UNRECOGNIZED_FILE;
    }
}
int endsWith(const std::string& s,const std::string& sub) {
    return s.rfind(sub) == (s.length() - sub.length()) ? 1 : 0;
}
BOOL GetPrivileges()
{
    HANDLE hToken;
    LUID sedebugnameValue;
    TOKEN_PRIVILEGES tkp;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        return   FALSE;
    }
    if (!LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &sedebugnameValue))
    {
        CloseHandle(hToken);
        return false;
    }
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = sedebugnameValue;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), nullptr, nullptr))
    {
        CloseHandle(hToken);
        return false;
    }
    return true;
}
void replace(std::string filePath,int fileType){
    FILE* file= fopen(filePath.c_str(),"rb+");
    fseek(file,0,SEEK_END);
    size_t len= ftell(file);
    fseek(file,0,SEEK_SET);
    FILE* modified;
    filePath=filePath+"_formatted";
    if (fileType == FTYP_VIDEO_FILE){
        modified= fopen(filePath.append(".mp4").c_str(),"wb+");
        fseek(file,2,SEEK_SET);
        len-=2;
    } else if (fileType==EXIF_FILE||fileType==JPEG_FILE){
        modified= fopen(filePath.append(".jpg").c_str(),"wb+");
    } else if (fileType==PNG_FILE){
        modified= fopen(filePath.append(".png").c_str(),"wb+");
    } else {
        printf("Unrecognized File :%s\n",filePath.c_str());
        return;
    }
    printf("File Name: %s | Target File Name: %s | File Type: 0x%x| Processed Size: %llu\n",filePath.c_str(),filePath.c_str(),fileType,len);
    char* buffer=new char[len+1];
    fread(buffer,1,len,file);
    fwrite(buffer,1,len,modified);
    delete[] buffer;
    fclose(file);
    fclose(modified);
}
void traverse(std::vector<std::string> &vct, char* path){
    _int64 handle;
    _finddata_t fi{};
    char _path[1024]={0};
    memcpy(_path,path, strlen(path));
    strcat(_path,"\\*");
    _int64 ret=handle=_findfirst(_path,&fi);
    if (handle==-1) {
        MessageBox(nullptr, _T("No files exists in the directory"), _T("Error"), MB_OK);
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
}