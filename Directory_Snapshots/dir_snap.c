#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <inttypes.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <sys/wait.h>
typedef struct files
{
    ino_t inode;
    char fpath[4096];
    off_t size;
} files;
int fileExists(const char *snap)
{
    struct stat buf;
    if (stat(snap, &buf) == 0)
    {
        return 1;
    }
    return 0;
}
int openFile(char *snapFile)
{
    int snap;
    char buff[] = "Inode|FileName|Size\n";
    if (fileExists(snapFile))
    {
        remove(snapFile);
    }
    if ((snap = open(snapFile, O_RDWR | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR)) < 0)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    write(snap, buff, strlen(buff));
    return snap;
}
void writeToSnap(int fd, ino_t ino, char *filePath, off_t size)
{
    char buff[5000];
    sprintf(buff, "%ju|%s|%ju\n", (uintmax_t)ino, filePath, (uintmax_t)size);
    write(fd, buff, strlen(buff));
}
int TakeSnapshot(const char *nameDir, int snap, const char *InitDir, char *izolatedDir, int dangerFiles)
{
    int pid = 0;
    int pfd[2];
    char buff[PATH_MAX];
    char abspath[PATH_MAX];
    DIR *Din;
    int stdout_s = dup(STDOUT_FILENO);
    if ((Din = opendir(nameDir)) == NULL)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    realpath(nameDir, abspath);
    struct dirent *entry = NULL;
    while ((entry = readdir(Din)) != NULL)
    {
        char path[4353];
        struct stat st;
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            sprintf(path, "%s/%s", abspath, entry->d_name);
            if (stat(path, &st) != 0)
            {
                perror("stat");
                exit(EXIT_FAILURE);
            }
            if (S_ISREG(st.st_mode))
            {
                if (st.st_mode == 32768)
                {
                    mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IROTH;
                    if (chmod(path, mode) < 0)
                    {
                        perror("Error in chmod");
                        exit(EXIT_FAILURE);
                    }
                    if (pipe(pfd) < 0)
                    {
                        perror("Eroare la crearea pipe-ului\n");
                        exit(1);
                    }
                    if ((pid = fork()) < 0)
                    {
                        perror("Fork error");
                        exit(-1);
                    }
                    if (pid == 0)
                    {
                        close(pfd[0]);
                        dup2(pfd[1], 1);
                        close(pfd[1]);
                        if (execl("/bin/bash", "bash", "verify_for_malicious.sh", path, (char *)NULL) == -1)
                        {
                            perror("execl failed: ");
                            exit(EXIT_FAILURE);
                        }
                        exit(pid);
                    }
                    else
                    {
                        close(pfd[1]);
                        wait(NULL);
                        read(pfd[0], buff, PATH_MAX);
                        if (strcmp(buff, "SAFE") != 0)
                        {
                            if (chmod(path, 0) == -1)
                            {
                                perror("chmod error");
                            }
                            dangerFiles++;
                            char newPath[PATH_MAX];
                            sprintf(newPath, "%s/%s", izolatedDir, entry->d_name);
                            if (rename(path, newPath) != 0)
                            {
                                perror("Error moving file");
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                }
                else
                {
                    writeToSnap(snap, st.st_ino, path, st.st_size);
                }
            }
            if (S_ISDIR(st.st_mode))
            {
                dangerFiles=TakeSnapshot(path, snap, InitDir, izolatedDir, dangerFiles);
            }
        }
    }
    closedir(Din);
    dup2(stdout_s, STDOUT_FILENO);
    if (strcmp(nameDir, InitDir) == 0)
    {
        close(snap);
    }
    return dangerFiles;
}

int cmp(const void *a, const void *b)
{
    const files *fa = a;
    const files *fb = b;
    if (fa->inode > fb->inode)
    {
        return 1;
    }
    else if (fa->inode < fb->inode)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
files *addFile(files *f, const char *fileName, int *nr)
{
    int n = 10;
    FILE *fin;
    char linie[5000];
    if ((fin = fopen(fileName, "r")) == NULL)
    {
        perror("fopen");
        printf("%s\n", fileName);
        exit(EXIT_FAILURE);
    }
    if ((f = malloc(sizeof(files) * n)) == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    fgets(linie, 5000, fin);
    while (fgets(linie, 5000, fin))
    {
        if (*nr < n)
        {
            f[(*nr)].inode = atoi(strtok(linie, "|"));
            strcpy(f[(*nr)].fpath, strtok(NULL, "|"));
            f[(*nr)++].size = atoi(strtok(NULL, "|"));
        }
        else
        {
            n = n * 2;
            if ((f = realloc(f, sizeof(files) * n)) == NULL)
            {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            f[(*nr)].inode = atoi(strtok(linie, "|"));
            strcpy(f[(*nr)].fpath, strtok(NULL, "|"));
            f[(*nr)++].size = atoi(strtok(NULL, "|"));
        }
    }
    fclose(fin);
    qsort(f, *nr, sizeof(files), cmp);
    return f;
}
void compareSnapshots(files *f1, int n1, files *f2, int n2, int i, const char *nameOutDir)
{
    FILE *fout;
    char outbuff[200];
    sprintf(outbuff, "%s/out%d.txt", nameOutDir, i);
    if ((fout = fopen(outbuff, "w")) == NULL)
    {
        perror("fopen");
        printf("%s\n", outbuff);
        exit(EXIT_FAILURE);
    }
    int found = 0;
    for (int i = 0; i < n1; i++)
    {
        found = 0;
        for (int j = 0; j < n2; j++)
        {
            if (f1[i].inode == f2[j].inode)
            {
                found = 1;
                if (strcmp(f1[i].fpath, f2[j].fpath) != 0)
                {
                    fprintf(fout, "file %s was renamed to %s\n", f1[i].fpath, f2[j].fpath);
                }
                if (f1[i].size != f2[j].size)
                {
                    fprintf(fout, "File %s changed size from %ju to %ju\n", f1[i].fpath, f1[i].size, f2[j].size);
                }
            }
        }
        if (found == 0)
        {
            fprintf(fout, "File %s was deleted\n", f1[i].fpath);
        }
    }
    for (int i = 0; i < n2; i++)
    {
        found = 0;
        for (int j = 0; j < n1; j++)
        {
            if (f2[i].inode == f1[j].inode)
            {
                found = 1;
            }
        }
        if (found == 0)
        {
            fprintf(fout, "File %s was added\n", f2[i].fpath);
        }
    }
    fclose(fout);
}
int verifyDiff(const char *snapFile, const char *nameDir, int i, const char *nameOutDir, char *izolatedDir)
{
    files *f = NULL;
    files *fa = NULL;
    int dangerFiles=0;
    char sa[200];
    sprintf(sa, "SnapshotActual%d.txt", i);
    int n = 0;
    int na = 0;
    int snap_actual = openFile(sa);
    dangerFiles=TakeSnapshot(nameDir, snap_actual, nameDir, izolatedDir, dangerFiles);
    close(snap_actual);
    f = addFile(f, snapFile, &n);
    fa = addFile(fa, sa, &na);
    compareSnapshots(f, n, fa, na, i, nameOutDir);
    remove(snapFile);
    rename(sa, snapFile);
    free(f);
    free(fa);
    return dangerFiles;
}
unsigned int isDir(const char *nameDir)
{
    struct stat st;
    if (stat(nameDir, &st) != 0)
    {
        perror("stat");
        exit(EXIT_FAILURE);
    }
    if (S_ISDIR(st.st_mode))
    {
        return 1;
    }
    return 0;
}
int main(int argc, char **argv)
{
    if (argc < 3 || argc > 10)
    {
        printf("Usage: ./dir_snap <Output Directory> <Isolated Dir> <Input Directory> <Input Directory> ...");
        printf("Maximum 10 arguments are allowed\n");
        exit(EXIT_FAILURE);
    }
    char *nameOutDir = argv[1];
    for (int i = 3; i < argc; i++)
    {
        int pid;
        int dangerFiles = 0;
        if ((pid = fork()) < 0)
        {
            perror("Fork error");
            exit(EXIT_FAILURE);
        }
        if (pid == 0)
        {
            int dangerFiles = 0;
            char buff[20];
            sprintf(buff, "%s.txt", argv[i]);
            if (fileExists(buff))
            {
                dangerFiles=verifyDiff(buff, argv[i], i, nameOutDir, argv[2]);
                printf("Snapshot for directory %d was verified successfully\n", i);
                printf("Child process %d has terminated with pid %d and found %d dangerous files\n", i, getpid(), dangerFiles);
            }
            else
            {
                sprintf(buff, "%s.txt", argv[i]);
                int snap = openFile(buff);
                dangerFiles = TakeSnapshot(argv[i], snap, argv[i], argv[2], dangerFiles);
                close(snap);
                printf("Snapshot for directory %d was created successfully\n", i);
                printf("Child process %d has terminated with pid %d and found %d dangerous files\n", i, getpid(), dangerFiles);
            }
            exit(pid);
        }
    }
    return 0;
}