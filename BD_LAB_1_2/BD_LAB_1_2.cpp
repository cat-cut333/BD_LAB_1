#pragma warning(disable : 4996)
#include <fstream>
#include <Windows.h>
#include <iostream>


/*Сделать удаление последнего блока
изменение информации о кол-ве блоков в нулевом блоке при удалении и добавлении студентов*/
struct Zap {
    int id_zachet ;
    int id_gr ;
    char surname[20];
    char name[20] = "NULL";
    char patronymic[30];
};

struct Block {
    Zap zap_block[5];
};

struct Null_Block {
    int size_block = sizeof(Block);
    int size_Zap = sizeof(Zap);
    int count_block = 0;
    int count_Zap_in_block = 5;
};



struct For_search_Block {
    Block block;
    int namber_zap;
    int namber_block;
};

For_search_Block search_Block(int key, char name_file[], Null_Block* null_block);
void search(int key, char name_file[], Null_Block* null_block);
void delete_Zap_in_Block(Zap* zap, char name_file[], Null_Block* null_block);
int write_block(Block* this_block, int Namber_Block, char name_file[], Null_Block* null_block);
int read_block(Block* this_block, int Namber_Block, char name_file[], Null_Block* null_block);
void add_Zap_in_Block(Zap* zap, char name_file[], Null_Block* null_block);

int write_block(Block* this_block,int Namber_Block, char name_file[], Null_Block* null_block) {  //запись блока в определенную позицию файла с начала
    FILE* outfile;
    int size_block = null_block->size_block;
    outfile = fopen(name_file, "ab");
    if (outfile == NULL) {
        fprintf(stderr, "\nError opened file\n");
        return -1;
    }
    
    fseek(outfile,sizeof(struct Null_Block)+ size_block*(Namber_Block-1), SEEK_SET);
    int flag = fwrite(this_block, size_block, 1, outfile);
    if (!flag) {
        printf("NO");
        return -2;
    }
    fclose(outfile);
}

int read_block(Block* this_block, int Namber_Block, char name_file[], Null_Block* null_block) {
    FILE* outfile;
    int size_block = null_block->size_block;
    outfile = fopen(name_file, "rb");
    if (outfile == NULL) {
        fprintf(stderr, "\nError opened file\n");
        return -1;
    }
    fseek(outfile, sizeof(struct Null_Block)+ size_block*(Namber_Block-1), SEEK_SET);
    int flag=  fread(this_block, size_block, 1, outfile);
    fclose(outfile);
    return flag;
}

void add_Zap_in_Block(Zap* zap, char name_file[],Null_Block* null_block) {
   
    Block this_block;
    bool flag = true;
    For_search_Block result_search =  search_Block(zap->id_zachet, name_file,null_block);
    //std::cout << "Все ок_послепоиска";
    if (result_search.namber_zap != -1) //если уже существует запись, она повторно не записывается.
    {
        printf("\n Запись уже существует");
        return;
    }
    this_block = result_search.block;
    for (int i = 0; i++; i < null_block->count_Zap_in_block) {   //нужно ввести  переменую для кол-ва записей в блоке - сделано с помощью нулевого блока
        if (this_block.zap_block[i].name == "NULL") {
            this_block.zap_block[i] = *zap;
            write_block(&this_block,result_search.namber_block, name_file, null_block);
            flag = false;
            break;
        }
    }
    if (flag) {
        Block New_block;
        New_block.zap_block[0] = *zap;
        write_block(&New_block, result_search.namber_block+1, name_file, null_block);
    }
}

void re_write_Zap_in_Block(Zap* zap, char name_file[], Null_Block* null_block) {

    Block this_block;
    For_search_Block result_search = search_Block(zap->id_zachet, name_file, null_block);
    if (result_search.namber_zap == -1) //если не существует записи, то непроисходит запись
   
    this_block = result_search.block;
    this_block.zap_block[result_search.namber_zap] = *zap;
    write_block(&this_block, result_search.namber_block, name_file, null_block);
}


void delete_Zap_in_Block(Zap* zap, char name_file[], Null_Block* null_block) {
    Zap null_zap;
    Block this_block;
    Block next_block;
    int namber_block;
    int namber_zap;
    For_search_Block result_search = search_Block(zap->id_zachet, name_file, null_block);
    if (result_search.namber_zap == -1) //если не существует записи, то непроисходит удаление
    {
        printf("Такой записи не существует");
        return;
    }

    namber_block = result_search.namber_block;
    namber_zap = result_search.namber_zap;
    this_block = result_search.block;
    do {//смещение записей при удалении

        while (namber_zap != null_block->count_Zap_in_block -1)
        {
            this_block.zap_block[namber_zap] = this_block.zap_block[namber_zap + 1];
            namber_zap++;
        }
        if (namber_block != null_block->count_block) {
            read_block(&next_block, namber_block + 1, name_file, null_block);
            this_block.zap_block[4] = next_block.zap_block[0];
            write_block(&this_block, namber_block, name_file, null_block);
            namber_block++;
            this_block = next_block;
            namber_zap = 0;
        }
    } while (namber_block <= null_block->count_block);
    this_block.zap_block[null_block->count_Zap_in_block - 1] = null_zap;
    write_block(&this_block, namber_block, name_file, null_block);
    printf("Удаление успешно");
}


For_search_Block search_Block(int key, char name_file[], Null_Block* null_block)//поиск блока с записью, которую нужно заменить
{
    For_search_Block result;
    int flag = 1;//проверка что считан 1 блок;
    int count_Zap_in_Bloc = null_block->count_Zap_in_block;
    Block this_block;
   
    FILE* outfile;
    outfile = fopen(name_file, "rb");

    if (outfile == NULL) {
        fprintf(stderr, "\nError opened file\n");
    }
    int namber_block = 0;
    result.block = this_block;
    result.namber_block = namber_block;
    result.namber_zap = -1;
    //добавить проверку на конец файла
    while (flag != 0) {
      
        int i = 0;
        flag = read_block(&this_block, namber_block, name_file, null_block);
        while (i < count_Zap_in_Bloc) {
            if (this_block.zap_block[i].id_zachet == key) {
                result.block = this_block;
                result.namber_block = namber_block;
                result.namber_zap = i;
                return result;
            }
            i++;
        };
        namber_block++;
    }
   
    printf("End seach");
    return result;

}

void search(int key, char name_file[], Null_Block* null_block) { //поиск информации о студенте по ключу и вывод имени 
    bool flag =false;
    int count_Zap_in_Bloc = null_block->count_Zap_in_block;
    Block this_block;
    FILE* outfile;
    outfile = fopen(name_file, "rb");
   
    if (outfile == NULL) {
        fprintf(stderr, "\nError opened file\n");

    }
    int namber_block = 1; // проверка на считывание блока из файла
   //добавить проверку на конец файла
    while (!flag or not( namber_block>null_block->count_block)) {
        
        int i = 0;
       read_block(&this_block,namber_block, name_file, null_block);
       while( i < count_Zap_in_Bloc) {
           
            if (this_block.zap_block[i].id_zachet == key) {
                printf(this_block.zap_block[i].name);
                flag = true;
                break; }
            i++;
        };
       namber_block++;
       
    };

}




enum var_case {open = 1, add_student, delete_student,seacher_student,change_info_stident, close};

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    Null_Block null_block;
    char name_file[50] = "C:/Users/Azerty/Desktop/test.txt";
    int switch_on=-1;
    while (switch_on != close)
    {
        std::cout << "\n1 - open, \n2 - add_student, \n3 - delete_student, \n4 - seacher_student, \n5 - change_info_stident, \n6 -  close";
        std::cin >> switch_on;
        switch (switch_on)
        {
        case(open): {
            std::cout << "Введите имя файла";
            std::cin >> name_file;
            FILE* outfile;

            outfile = fopen(name_file, "rb");
            if (outfile == NULL) {
                fprintf(stderr, "\nСоздан новый файл\n");
                outfile = fopen(name_file, "wb");
                fwrite(&null_block, sizeof(struct Null_Block), 1, outfile);
                fclose(outfile);
                break;
            }
            if (feof(outfile)) {
                fclose(outfile);
                outfile = fopen(name_file, "ab");
                fwrite(&null_block, sizeof(struct Null_Block), 1, outfile);
                fclose(outfile);
            }
            else {
                fread(&null_block, sizeof(struct Null_Block), 1, outfile);
                fclose(outfile);
            }

            break;

        }
                  //C:/Users/Azerty/Desktop/test.txt
        case(add_student): {
            Zap new_student;
            std::cout << "\nВведите: номер зачетки, ID-группы, имя, отчество, фамилию ";
            std::cin >> new_student.id_zachet;
            std::cin >> new_student.id_gr;
            std::cin >> new_student.name;
            std::cin >> new_student.patronymic;
            std::cin >> new_student.surname;
         
            add_Zap_in_Block(&new_student, name_file, &null_block);
            
            break;
        }
        case(delete_student): {
            Zap id_zachet;
            std::cout << "\nВведите номер зачетки\n ";
            std::cin >> id_zachet.id_zachet;
            delete_Zap_in_Block(&id_zachet, name_file, &null_block);
            break;
        }
        case(seacher_student): {
            int id_zachet;
            std::cout << "\nВведите номер зачетки\n ";
            std::cin >> id_zachet;
            search(id_zachet, name_file, &null_block);
            break;
        }
        default:
            break;
        }
    }
}

//std::cout << "Все ок";