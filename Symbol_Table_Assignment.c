#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 26
#define MAX_LENGTH 10
#define ROW_SIZE 50

typedef struct Symbol_Table_item_tag
{
    char* var_name;
    int var_val;
    struct Symbol_Table_item_tag* next;
}ST_item;

typedef struct Symbol_Table_tag
{
    ST_item* entries[TABLE_SIZE];
    struct Symbol_Table_tag* surr_scope;
}Symbol_Table;

ST_item* create_item(char* name,int value)
{
    ST_item* item_ptr = (ST_item*)malloc(sizeof(ST_item));
    if(item_ptr == NULL)
    {
        printf("Memory allocation failed.\n");
    }
    item_ptr->var_name = strdup(name);
    if(item_ptr -> var_name == NULL)
    {
        printf("Memory allocation failed.\n");
    }
    item_ptr -> var_val = value;
    item_ptr -> next = NULL;

    return item_ptr;
}

Symbol_Table* create_table(Symbol_Table* surr_scope)
{
    Symbol_Table* table_ptr = (Symbol_Table*)malloc(sizeof(Symbol_Table));
    if(table_ptr == NULL)
    {
        printf("Memory allocation failed.\n");    
    }
    for(int i=0; i<TABLE_SIZE; i++)
    {
        table_ptr -> entries[i] = NULL;
    }
    table_ptr -> surr_scope = surr_scope;
}
/*Here I've assumed that name of variables is in lowercase since ma'am said that
it's upon you how you want to take it
*/
int hash_func(char* name)
{
    int ret_val = 0;
    for(int i=0; name[i]; i++)
    {
        ret_val = (ret_val + name[i]) - 'a';
    }
    return ret_val;
}

void Insert(Symbol_Table* table_ptr, char* name, int value)
{
    int index = hash_func(name);
    ST_item* item_ptr = create_item(name,value);
    item_ptr -> next = table_ptr -> entries[index];
    table_ptr -> entries[index] = item_ptr;
}

int Lookup(Symbol_Table* table_ptr, char* name)
{
    int index = hash_func(name);
    ST_item* item_ptr = table_ptr -> entries[index];
    while(item_ptr != NULL)
    {
        if(strcmp(item_ptr -> var_name, name)==0)
        {
            return item_ptr -> var_val;
        }
        item_ptr = item_ptr -> next;
    }
    if(table_ptr -> surr_scope != NULL)
    {
        return Lookup(table_ptr -> surr_scope, name);
    }
    printf("Variable not found!\n");
}

void free_table(Symbol_Table* table_ptr)
{
    if(table_ptr == NULL)
    return;
    for(int i=0; i<TABLE_SIZE; i++)
    {
        ST_item* curr_item = table_ptr -> entries[i];
        while(curr_item != NULL)
        {
            ST_item* temp = curr_item;
            curr_item = curr_item -> next;
            free(temp -> var_name);
            free(temp);
        }
    }
    free(table_ptr);
}

int main()
{
    Symbol_Table* current_table = create_table(NULL);
    FILE* file = fopen("sample.txt","r");
    if(file == NULL)
    {
        printf("Error in opening file.\n");
    }
    
    char row[ROW_SIZE];

    while(fgets(row,ROW_SIZE,file) != NULL)
    {
        char instruction[MAX_LENGTH],variable[MAX_LENGTH];
        int value;
        sscanf(row, "%s %s %d", instruction, variable, &value);
        if(strcmp(instruction,"begin")==0)
        {
            Symbol_Table* new_scope = create_table(current_table);
            current_table = new_scope;
        }
        else if(strcmp(instruction,"assign")==0)
        {
            Insert(current_table,variable,value);
        }
        else if(strcmp(instruction,"print")==0)
        {
            int var_val = Lookup(current_table,variable);
            printf("Value of %s is %d\n",variable,var_val);
        }
        else if(strcmp(instruction,"end")==0)
        {
            Symbol_Table* nearby_scope = current_table -> surr_scope;
            free_table(current_table);
            current_table = nearby_scope;
        }
    }
    fclose(file);
    free_table(current_table);
}