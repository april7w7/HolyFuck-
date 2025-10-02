#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TAPE_SIZE 30000
#define MAX_VARS 100

typedef struct {
    char name[50];
    int index;
} Var;

typedef struct {
    char tape[TAPE_SIZE];
    int ptr;
    Var vars[MAX_VARS];
    int var_count;
} VM;

VM vm;

void init_vm() {
    memset(&vm, 0, sizeof(VM));
    vm.ptr = 0;
    vm.var_count = 0;
}

// Busca índice da variável pelo nome
int get_var(const char *name) {
    for(int i=0;i<vm.var_count;i++){
        if(strcmp(vm.vars[i].name,name)==0) return vm.vars[i].index;
    }
    printf("Erro: variável %s não declarada\n", name);
    exit(1);
}

// Adiciona nova variável
int add_var(const char *name) {
    int idx = vm.ptr;
    strcpy(vm.vars[vm.var_count].name,name);
    vm.vars[vm.var_count].index = idx;
    vm.var_count++;
    vm.ptr++;
    return idx;
}

// Executa Brainfuck inline e retorna valor da célula 0 do BF
int exec_bf(const char *code) {
    int bf_ptr = 0;
    int bf_tape[30000] = {0};
    int stack[1000];
    int sp=0;
    int len = strlen(code);

    for(int i=0;i<len;i++){
        char c = code[i];
        switch(c){
            case '>': bf_ptr++; break;
            case '<': bf_ptr--; break;
            case '+': bf_tape[bf_ptr]++; break;
            case '-': bf_tape[bf_ptr]--; break;
            case '[':
                if(bf_tape[bf_ptr]==0){
                    int loop=1;
                    while(loop){
                        i++;
                        if(code[i]=='[') loop++;
                        if(code[i]==']') loop--;
                    }
                } else stack[sp++]=i;
                break;
            case ']':
                if(bf_tape[bf_ptr]!=0) i=stack[sp-1];
                else sp--;
                break;
            case '.': break;
            case ',': break;
        }
    }
    return bf_tape[0];
}

// Operações aritméticas
void fuckadd(const char *a, const char *b){ vm.tape[get_var(a)]+=vm.tape[get_var(b)]; }
void fucksub(const char *a, const char *b){ vm.tape[get_var(a)]-=vm.tape[get_var(b)]; }
void fuckmul(const char *a, const char *b){ vm.tape[get_var(a)]*=vm.tape[get_var(b)]; }
void fuckdiv(const char *a, const char *b){ if(vm.tape[get_var(b)]!=0) vm.tape[get_var(a)]/=vm.tape[get_var(b)]; }

// Compilador
void compile_file(const char *filename){
    FILE *f = fopen(filename,"r");
    if(!f){ perror("Erro ao abrir arquivo"); exit(1); }

    char line[1024];
    while(fgets(line,sizeof(line),f)){
        // Ignora comentários
        if(strncmp(line,"//",2)==0) continue;

        // fuckint
        if(strncmp(line,"fuckint",7)==0){
            char name[50], bf_code[1000];
            if(sscanf(line,"fuckint %49s = %999[^\n]",name,bf_code)==2){
                int idx = add_var(name);
                // Número puro
                if(strspn(bf_code,"0123456789")==strlen(bf_code)) vm.tape[idx]=atoi(bf_code);
                else vm.tape[idx]=exec_bf(bf_code);
            }
        }
        // printhishit
        else if(strncmp(line,"printhishit",11)==0){
            char name[50];
            if(sscanf(line,"printhishit(%49[^)])",name)==1){
                int idx = get_var(name);
                printf("%c",vm.tape[idx]);
            }
        }
        // gimmefuck
        else if(strncmp(line,"gimmefuck",9)==0){
            char name[50];
            if(sscanf(line,"gimmefuck(%49[^)])",name)==1){
                int idx = get_var(name);
                vm.tape[idx] = getchar();
            }
        }
        // Brainfuck inline (qualquer outra linha)
        else {
            int len = strlen(line);
            for(int i=0;i<len;i++){
                char c=line[i];
                switch(c){
                    case '>': vm.ptr++; break;
                    case '<': vm.ptr--; break;
                    case '+': vm.tape[vm.ptr]++; break;
                    case '-': vm.tape[vm.ptr]--; break;
                    case '.': putchar(vm.tape[vm.ptr]); break;
                    case ',': vm.tape[vm.ptr]=getchar(); break;
                }
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
