#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage_mgr.h"
#include "buffer_mgr.h"
#include "record_mgr.h"

const int countPage = 100;
const int elementName = 15;
const int attr_size = 20;		   // size of the name of the attribute
const int max_no_of_pages = 10000; // maximum no. of pages
SM_PageHandle pg_handle_var;
SM_FileHandle* filehandle_var;

//Implementation of record manager structure
typedef struct record_Info
{
	int tuple_count;		 // To store total number of tuples in table
	int firstEmptySlot;
	int numberOfFields;			 // it stores the location of first free pages which has empty slots
	int count;			 // it stores the count of the records scanned
	RID recordId;				 // Record ID
	Expr *findField;		 // Condition for scanning the records
	BM_PageHandle pageHandle; // Buffer Manager PageHandle
	BM_BufferPool bmBufferManager;		 // Buffer Manager Buffer Pool
} record_Info;

record_Info *rm;

//Sets the attribute offset
RC attrOffset(Schema *schema, int attrNum, int *endValue)
{
	*endValue = 1;

	int i;
	RC returnFlag = RC_OK;
	
	
	for(i=0; i < attrNum ; i++)
	{
		
		if (schema->dataTypes[i] == DT_INT){
			*endValue = *endValue + sizeof(int);
			}

		else if (schema->dataTypes[i] == DT_BOOL){
			*endValue = *endValue + sizeof(bool);
		}
		else if (schema->dataTypes[i] == DT_STRING){
			*endValue = *endValue + schema->typeLength[i];
		}
		else if (schema->dataTypes[i] == DT_FLOAT){
			*endValue = *endValue + sizeof(float);
		}
	
		
		
	}
	return returnFlag;
}


//The Page_empty_slot function finds a free slot within a page and returns it
int Page_empty_slot(int recordsize, char* data)
{
	int total_num_slot;
	total_num_slot = PAGE_SIZE / recordsize;
	int j=0;
	for (j = 0; j < total_num_slot; j++)
	{
		if (data[j * recordsize] != '$')
		{
			return j;
		}
	}
	
	return -1;
}


void handlepage_(Schema* schema, char page_size_data[PAGE_SIZE])
{
	char* pagehandle_var;
	pagehandle_var = page_size_data;
	int table_result;
	int nullvalue=0;
	int digitvalue=1;
	int i=0;
	
	//Setting the number of tuples to 0
	*(int*)pagehandle_var = nullvalue;
	pagehandle_var = sizeof(int) + pagehandle_var;

	//Setting the page position to 1
	*(int*)pagehandle_var = digitvalue;
	pagehandle_var = sizeof(int) + pagehandle_var;

	*(int*)pagehandle_var = schema->numAttr;
	pagehandle_var = sizeof(int) + pagehandle_var;
	
	*(int*)pagehandle_var = schema->keySize;
	
	for(i=nullvalue; i<schema->numAttr; i++)
	{
		if (i != nullvalue)
			pagehandle_var = sizeof(int) + pagehandle_var;
		
		strncpy(pagehandle_var, schema->attrNames[i], attr_size);
		pagehandle_var = attr_size + pagehandle_var;
		
		*(int*)pagehandle_var = (int)schema->dataTypes[i];
		pagehandle_var = sizeof(int) + pagehandle_var;
		
		*(int*)pagehandle_var = (int)schema->typeLength[i];
	}
}


/*The initRecordManager function is responsible for initializing the record manager
The initRecordManager makes use of Storage manager for initializing the record manager */
extern RC initRecordManager(void* mgmtData)
{
	//Initializing the Storage Manager and returning RC OK
	initStorageManager();
	return RC_OK;
}


//This function shuts down the record manager and frees the record manager memory
extern RC shutdownRecordManager()
{
	if (rm != NULL)
	{
		free(rm);
	}
	return RC_OK;
}

// Creating a table with table name "name" and with schema "schema"
extern RC createTable(char *name, Schema *schema)
{
	char page_size_data[PAGE_SIZE];
	int table_result;

	//Allocating memory space to the record manager structure that we defined
	rm = (record_Info *)malloc(sizeof(record_Info));
	SM_FileHandle filehandle_var;

	//Initializing the Buffer Pool with 
	initBufferPool(&rm->bmBufferManager, name, max_no_of_pages, RS_LRU, NULL);
	handlepage_(schema, page_size_data);

//Creating a table with the name 'name' using the createPageFile function
	table_result = createPageFile(name);
	if (table_result != RC_OK)
		return table_result;
	//Opening the newly created table by using the openPageFile function
	table_result = openPageFile(name, &filehandle_var);
	if (table_result != RC_OK)
		return table_result;
	//Writing to the new block using writeBlock function
	table_result = writeBlock(0, &filehandle_var, page_size_data);
	if (table_result != RC_OK)
		return table_result;

	//Closing the file after writing to it using the closePageFile function
	table_result = closePageFile(&filehandle_var);
	if (table_result != RC_OK)
		return table_result;

	return RC_OK;
}

int name=900;
// Opens table with table name "name"
extern RC openTable(RM_TableData *rel, char *name)
{
	int i=0, k;
	Schema *schema;
	SM_PageHandle pg_handle_var;

	int count_attribute;

	//PageHandle_pg(rel, name, pg_handle_var, count_attribute, schema);
	
	pinPage(&rm->bmBufferManager, &rm->pageHandle, 0);

	pg_handle_var = (char*)rm->pageHandle.data;

	rm->tuple_count = *(int*)pg_handle_var;

	pg_handle_var =pg_handle_var+ sizeof(int);

	rm->firstEmptySlot = *(int*)pg_handle_var;
	pg_handle_var = pg_handle_var+ sizeof(int);
	count_attribute = *(int*)pg_handle_var;
	pg_handle_var = pg_handle_var+ sizeof(int);
	schema = (Schema*)malloc(sizeof(Schema));

	schema->typeLength = (int*)malloc(sizeof(int) * count_attribute);

	schema->dataTypes = (DataType*)malloc(sizeof(DataType) * count_attribute);

	schema->attrNames = (char**)malloc(sizeof(char*) * count_attribute);
	
	schema->numAttr = count_attribute;


	while (true)
	{
		if (i == count_attribute)
			break;
		schema->attrNames[i] = (char*)malloc(attr_size);
		i++;
	}
	i = 0;
	while (true)
	{
		if (i == schema->numAttr)
			break;
		if (i != 0)
		pg_handle_var = pg_handle_var+ sizeof(int);

		strncpy(schema->attrNames[i], pg_handle_var, attr_size);

		pg_handle_var = pg_handle_var+ attr_size;

		schema->dataTypes[i] = *(int*)pg_handle_var;
		pg_handle_var = pg_handle_var+sizeof(int);

		schema->typeLength[i] = *(int*)pg_handle_var;

		i++;
	}

	rel->name = name;
	rel->mgmtData = rm;
	
	rel->schema = schema;
	
	unpinPage(&rm->bmBufferManager, &rm->pageHandle);

	forcePage(&rm->bmBufferManager, &rm->pageHandle);

	return RC_OK;
}


//This function is responsible for closing the table that is referenced using 'rel' name
extern RC closeTable(RM_TableData* rel)
{
	//Storing the table's meta data before closing it
	record_Info* rm = rel->mgmtData;

	//Shutting down the Buffer Pool
	shutdownBufferPool(&rm->bmBufferManager);
	
	free(rel->schema);
	return RC_OK;

}


//The deleteTable function is responsible for deleting the table that has the name 'name'
extern RC deleteTable(char* name)
{
	if(destroyPageFile(name)!= RC_OK)
	{
		return RC_FILE_NOT_FOUND;
	}
	else
	{
		destroyPageFile(name);
		return RC_OK;
	}
}


/*The getNumTuples function uses the reference variable rel 
The variable rel is used to return the number of tuples / records present in the table*/
extern int getNumTuples(RM_TableData* rel)
{
	record_Info* rm = rel->mgmtData;
	return rm->tuple_count;
	
}
// Handling Records in a Table
/*extern RC insertRecord (RM_TableData *rel, Record *record); done
extern RC deleteRecord (RM_TableData *rel, RID id); done
extern RC updateRecord (RM_TableData *rel, Record *record); done
extern RC getRecord (RM_TableData *rel, RID id, Record *record); done

// scans
extern RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond); done
extern RC next (RM_ScanHandle *scan, Record *record);
extern RC closeScan (RM_ScanHandle *scan); done
*/

//This functions inserts a new record or field to the table
extern RC insertRecord(RM_TableData *rel, Record *record)
{	
	record_Info *mgr = rel->mgmtData;
	RC returnCode = RC_OK;
	char *data, *sp;
	RID *recordId = &record->id;
	int nullValue=0;
	int digitValue=1;
	int adder=0;

	recordId->page = mgr->firstEmptySlot;

	pinPage(&mgr->bmBufferManager, &mgr->pageHandle, recordId->page);
	
	recordId->slot = Page_empty_slot(getRecordSize(rel->schema), mgr->pageHandle.data);

	while (recordId->slot < nullValue)
	{	
		
		unpinPage(&mgr->bmBufferManager, &mgr->pageHandle);
		recordId->page= recordId->page +1;
		pinPage(&mgr->bmBufferManager, &mgr->pageHandle, recordId->page);
		data = mgr->pageHandle.data;
		recordId->slot = Page_empty_slot(getRecordSize(rel->schema), mgr->pageHandle.data);


	}
	
	markDirty(&mgr->bmBufferManager, &mgr->pageHandle);
	
	sp = mgr->pageHandle.data;
	
	adder =(recordId->slot * getRecordSize(rel->schema));

	sp = sp + adder;

	*sp = '$';

	memcpy(sp+1, record->data + 1, getRecordSize(rel->schema) - 1);
	
	unpinPage(&mgr->bmBufferManager, &mgr->pageHandle);
	
	rm->tuple_count= rm->tuple_count+1;
	
	pinPage(&mgr->bmBufferManager, &mgr->pageHandle, nullValue);
	
	return returnCode;
}


//This function deletes a record
extern RC deleteRecord(RM_TableData *rel, RID id)
{
	
	record_Info *manage = rel->mgmtData;
	int recordSize, adder=0;
    BM_BufferPool *const buffManager = &manage->bmBufferManager;
    BM_PageHandle *const currPage = &manage->pageHandle;

	pinPage(buffManager, currPage, id.page);
	
	manage -> firstEmptySlot = id.page;
	char *data;
	data = rm->pageHandle.data;
	
	recordSize = getRecordSize(rel->schema);
	
	adder = id.slot * recordSize;
	data = data + adder;
	
	markDirty(buffManager, currPage);
    unpinPage(buffManager, currPage);
	
	*data = '&';
    
	return RC_OK;
   
	
	
}



// This function updates a record
extern RC updateRecord(RM_TableData *rel, Record *record)
{
	char *dataPointer;
	int valueToBeAdded=0;
	record_Info *mgr = rel->mgmtData;
	BM_BufferPool *const bm = &mgr -> bmBufferManager;
    BM_PageHandle *const cp = &mgr -> pageHandle;
	pinPage(bm, cp, record->id.page);
	dataPointer = mgr->pageHandle.data;
	valueToBeAdded = (record->id.slot * getRecordSize(rel->schema));
	dataPointer = dataPointer + valueToBeAdded;
	*dataPointer = '$';
	markDirty(bm,cp);
	memcpy(++dataPointer, record->data + 1, getRecordSize(rel->schema) - 1);
	unpinPage(bm, cp);
	return RC_OK;
}

//This function gets the record from the position
extern RC getRecord(RM_TableData *rel, RID id, Record *record)
{ RC returnflag= RC_OK;

	char *dataPointer, *data;
	int flag=0;
	record_Info *mgr = rel->mgmtData;
	BM_BufferPool *const bm = &mgr -> bmBufferManager;
    BM_PageHandle *const cp = &mgr -> pageHandle;
    
	pinPage(bm, cp, id.page);

	dataPointer = mgr->pageHandle.data;
	
	flag =id.slot * getRecordSize(rel->schema);
	
	dataPointer = dataPointer + flag;
	if (*dataPointer == '$')
	{
		record->id = id;
		data = record->data;
		memcpy(++data, dataPointer + 1, getRecordSize(rel->schema) - 1);
	}
	else
	{
		return RC_ERROR;
	}
	
	unpinPage(bm, cp);

	return returnflag;
}



//This function starts the scan
extern RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond){

	/* init  RM_ScanHandle data structure */
	scan->rel = rel;
	openTable(rel, "Scan Table");
	record_Info *scan_mgr = (record_Info *)malloc(sizeof(record_Info));
	scan->mgmtData = scan_mgr;
	scan_mgr->recordId.page = 1;
	scan_mgr->findField = cond;
	scan_mgr->recordId.slot = 0;
	scan_mgr->count = 0;
	record_Info *table_mgr = rel->mgmtData;
	table_mgr->tuple_count = attr_size;
	return RC_OK;
}

//This function stores the scanned info into the specified location
extern RC next(RM_ScanHandle *scan, Record *record)
{
	char *data;
	RC returnFlag = RC_RM_NO_MORE_TUPLES;
	char *dataPointer;
    Schema *tableInfo = scan -> rel -> schema;

	int total_slots;
	int increase;
	
	record_Info *manager, *table;
	Schema *schema;
	manager = scan->mgmtData;
	schema = scan->rel->schema;
	increase = manager->count;
	table = scan->rel->mgmtData;
	
	int valueToBeAdded=0;
	int nullvalue=0;
	int digitvalue=1;
	
	Value *res = (Value *)malloc(sizeof(Value));

	BM_BufferPool *const bm = &manager -> bmBufferManager;
    BM_PageHandle *const cp = &manager -> pageHandle;
	while (table->tuple_count>= manager->count)
	{
		if (increase <=nullvalue)
		{
			manager->recordId.slot = nullvalue;
			manager->recordId.page = digitvalue;
			
		}
		else
		{
			manager->recordId.slot=manager->recordId.slot+1;

			if (manager->recordId.slot >= (PAGE_SIZE / getRecordSize(schema)))
			{
				manager->recordId.slot = nullvalue;
				manager->recordId.page=manager->recordId.page+1;
			}
			
		}
		
		pinPage(&table->bmBufferManager, cp, manager->recordId.page);
	
		data = manager->pageHandle.data;
		
		valueToBeAdded = (manager->recordId.slot * getRecordSize(schema));
		
		data = data + valueToBeAdded;
		
		record->id.page = manager->recordId.page;
		
		record->id.slot = manager->recordId.slot;
		dataPointer = record->data;
		
		*dataPointer = '&';
		
		memcpy(++dataPointer, data + 1, getRecordSize(schema) - 1);
	
		manager->count= manager->count+1;
		increase= increase+1;
	
		evalExpr(record, schema, manager->findField, &res);
		
		if (digitvalue== res->v.boolV )
		{
			unpinPage(&table->bmBufferManager, cp);
			return RC_OK;
		}
	}
	
	if (nullvalue ==table->tuple_count  ){
		return returnFlag;
	}
	if ( NULL== manager->findField)
	{
		return RC_SCAN_CONDITION_NOT_FOUND;
	}
	
	
	
	unpinPage(&table->bmBufferManager, cp);
	
	
	
	manager->recordId.slot = nullvalue;
	manager->recordId.page = digitvalue;
	manager->count = nullvalue;
	
	return returnFlag;
	
}

//End of scan
extern RC closeScan(RM_ScanHandle *scan)
{
	scan -> mgmtData = NULL;
	scan -> rel = NULL;
    return RC_OK;
}

//Helps get the record size with all the attributes
extern int getRecordSize(Schema *schema)
{
	int val = 0;
	int i;
	

	i = 0;
	//For Loop for all the attributes in the schema
	for(i=0; i<schema->numAttr; i++)
	
	{
		if (schema->dataTypes[i] == DT_INT){
			val = val + sizeof(int);
		}
		
		else if (schema->dataTypes[i] == DT_BOOL){
			val = val + sizeof(bool);
			}
		else if (schema->dataTypes[i] == DT_FLOAT){
			val = val + sizeof(float);
			}
		else if (schema->dataTypes[i] == DT_STRING){
			val = val + schema->typeLength[i];
		}
		

	}
	return ++val;
}


/***********************************

	@function brief   :createSchema
					  1.When global variables are not allocated to available memory, allocate first
					  2.Schema get the value: numAttr/attrNames/dataTypes/typeLength/keys/keySize
					  3.return schema



***********************************/
extern Schema *createSchema(int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys) {

	Schema *schema = (Schema *)malloc(sizeof(Schema)); //allocate
	
	//get the value of schema
	schema->numAttr = numAttr;
	schema->attrNames = attrNames;
	schema->dataTypes = dataTypes;
	schema->typeLength = typeLength;
	schema->keyAttrs = keys;
	schema->keySize = keySize;
	
	return schema;
}


/***********************************

	@function brief   :freeSchema

	
***********************************/
extern RC freeSchema (Schema *schema){
    free(schema);
    return RC_OK;
}


extern RC createRecord (Record **record, Schema *schema){

    Record *RECORD = (Record *)  malloc(sizeof(Record));//allocate memory for each schema

	int recordSize = getRecordSize(schema);//get the size of schema

	RECORD->data = (char*)malloc(sizeof(char) * recordSize);//set space for record
	//initialize the page and slot
	RECORD->id.page = -99;//random value
	RECORD->id.slot = -99;//random value

	char *p = RECORD->data;
	*p='&';
	p=p+1;
	*(p)='\0';
	*record = RECORD;

    return RC_OK;
}





/***********************************

	@function brief   :freeRecord
					


***********************************/
extern RC freeRecord (Record *record){

    free(record->data);
    free(record);

    return RC_OK;
}


extern RC getAttr(Record *record, Schema *schema, int attrNum, Value **value) {
	//record the size
	int attr_1 = 0;
	attrOffset(schema, attrNum, &attr_1);
	Value *attrValue;
	char *attrData;

	//allocate space to the value data
	attrValue = (Value *)malloc(sizeof(Value));
	attrData = (record->data);
	attrData += attr_1;

	//save DataType
	DataType dt = schema->dataTypes[attrNum];

	//Depend on the type of the attribute
	if (attrNum == 1)
	{
		dt = 1;
	}
	else
	{
		dt = dt;
	}

	*value = attrValue;
	
	if (schema->dataTypes[attrNum] == DT_INT)
	{
		int value;
		value = 0;
		attrValue->dt = DT_INT;
		memmove(&value, attrData, sizeof(int));
		attrValue->v.intV = value;
	}
	else if (schema->dataTypes[attrNum] == DT_FLOAT)
	{
		float value;
		attrValue->dt = DT_FLOAT;
		memmove(&value, attrData, sizeof(float));
		attrValue->v.floatV = value;
	}
	else if (schema->dataTypes[attrNum] == DT_STRING)
	{
		int length;
		attrValue->dt = DT_STRING;
		length = schema->typeLength[attrNum];
		attrValue->v.stringV = (char *)malloc(length + 1);
		strncpy(attrValue->v.stringV, attrData, length);
		attrValue->v.stringV[length] = '\0';
	}
	else if (schema->dataTypes[attrNum] == DT_BOOL)
	{
		bool value;
		attrValue->dt = DT_BOOL;
		memmove(&value, attrData, sizeof(bool));
		attrValue->v.boolV = value;
	}
	else
	{
		printf(" not defined \n");
	}

	return RC_OK;
}
/***********************************

	@function brief   :getAttr
					  1.allocate space for the Value data
					  2.save the data type
					  3.use for function to count the length and add it into the offset
					  4.get the attribute's data type and value of the '*value' parameter

	*/



extern RC setAttr(Record *record, Schema *schema, int attrNum, Value *value) {
	//record the size
	int attr_2 = 0;
	char *attrData;
	attrOffset(schema, attrNum, &attr_2);

	attrData = record->data;
	attrData += attr_2;
	//caculate the size and add it into the offset 
	int i;
	for (i = 0; i < attrNum; i++) {
		attr_2 += schema->typeLength[i];
	}

	//Depend on the datatype of the attribute
	if (value->dt == DT_STRING) {
		
		strncpy(attrData, value->v.stringV, schema->typeLength[attrNum]);
		attrData += schema->typeLength[attrNum];
	}
	else if (value->dt == DT_BOOL) {
		*(bool *)attrData = value->v.boolV;
		attrData += sizeof(bool);
	}
	else if (value->dt == DT_INT) {
		*(int *)attrData = value->v.intV;
		attrData += sizeof(int);
	}
	else if (value->dt == DT_FLOAT) {
		*(float *)attrData = value->v.floatV;
		attrData += sizeof(float);
	}

	return RC_OK;
}
