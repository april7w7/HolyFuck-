#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TAPE_SIZE 30000
#define MAX_VARS 100

typedef struct { char name[50]; int index; int is64; } Var;
typedef struct { unsigned char tape[TAPE_SIZE]; long long tape64[TAPE_SIZE]; int ptr; Var vars[MAX_VARS]; int var_count; } VM;

VM vm;

void init_vm() { memset(&vm,0,sizeof(vm)); vm.ptr=0; vm.var_count=0; }
int get_var(const char *name) { for(int i=0;i<vm.var_count;i++) if(strcmp(vm.vars[i].name,name)==0) return i; fprintf(stderr,"Erro: variavel '%s' nao declarada\n",name); exit(1);}
int add_var(const char *name,int is64) { int idx=vm.ptr; strcpy(vm.vars[vm.var_count].name,name); vm.vars[vm.var_count].index=idx; vm.vars[vm.var_count].is64=is64; vm.var_count++; return vm.ptr++; }

void compile_file(const char *filename){
    FILE *f=fopen(filename,"r");
    if(!f){ perror("Erro ao abrir arquivo"); exit(1);}
    char line[1024];
    int run=0;
    while(fgets(line,sizeof(line),f)){
        if(strncmp(line,"†god",4)==0){ run=1; continue; }
        if(strncmp(line,"†devil",6)==0){ break; }
        if(!run) continue;

        if(strncmp(line,"fuckint64",9)==0){
            char name[50], val[1000];
            if(sscanf(line,"fuckint64 %49s = %999[^\n]",name,val)==2){
                int idx=add_var(name,1);
                vm.tape64[idx]=(strspn(val,"0123456789")==strlen(val)) ? atoll(val) : atoi(val);
            }
        } else if(strncmp(line,"fuckint",7)==0){
            char name[50], val[1000];
            if(sscanf(line,"fuckint %49s = %999[^\n]",name,val)==2){
                int idx=add_var(name,0);
                vm.tape[idx]=(strspn(val,"0123456789")==strlen(val)) ? atoi(val) : atoi(val);
            }
        } else if(strncmp(line,"printhishit",11)==0){
            char name[50]; if(sscanf(line,"printhishit(%49[^)])",name)==1){
                int i=get_var(name);
                if(vm.vars[i].is64) putchar((unsigned char)(vm.tape64[vm.vars[i].index]&0xFF));
                else putchar(vm.tape[vm.vars[i].index]);
                fflush(stdout); // garante que o OnlineGDB mostre imediatamente
            }
        } else if(strncmp(line,"gimmefuck",9)==0){
            char name[50]; if(sscanf(line,"gimmefuck(%49[^)])",name)==1){
                int i=get_var(name); int ch=getchar();
                if(vm.vars[i].is64) vm.tape64[vm.vars[i].index]=(ch==EOF)?0:ch;
                else vm.tape[vm.vars[i].index]=(ch==EOF)?0:ch;
            }
        }
    }
    fclose(f);
}

int main(int argc,char **argv){
    if(argc<2){ printf("Uso: %s arquivo.hf\n",argv[0]); return 1; }
    init_vm();
    compile_file(argv[1]);
    return 0;
}
